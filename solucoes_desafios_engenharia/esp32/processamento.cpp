#include <WiFi.h>
#include <WebServer.h>
#include <LittleFS.h>
#include <math.h>
#include <WebSocketsServer.h> // Instalar via Library Manager

const char* ssid = "ESP32_Meteorologia";
const char* password = "";

WebServer server(80);
WebSocketsServer webSocket(81);
File uploadFile;

// --- INSIRA O HTML AQUI (Use o html anterior, mas com o JS atualizado abaixo) ---
const char index_html[] PROGMEM = R"rawliteral(...)rawliteral";

// (Mantenha as funções pontoOrvalho, windChill e heatIndex iguais...)

void handleRoot() {
    server.send(200, "text/html", index_html);
}

// (Mantenha handleCalcularManual e handleFileUpload iguais...)

void handleProcessCSV() {
    File file = LittleFS.open("/temp.csv", FILE_READ);
    if (!file) {
        server.send(500, "text/plain", "Erro FS");
        return;
    }

    size_t totalBytes = file.size();
    size_t processedBytes = 0;

    server.setContentLength(CONTENT_LENGTH_UNKNOWN);
    server.send(200, "text/csv", "");
    server.sendContent("Temperatura,Umidade,Vento,Ponto_Orvalho,Wind_Chill,Heat_Index\n");

    bool headerSkip = true;
    char buffer[128];
    int lastProgress = 0;
    
    while (file.available()) {
        String line = file.readStringUntil('\n');
        processedBytes += line.length() + 1; // +1 para o \n removido
        line.trim();
        
        if (line.length() == 0) continue;
        if (headerSkip) { headerSkip = false; continue; }

        float T = 0, UR = 0, v = 0;
        if (sscanf(line.c_str(), "%f,%f,%f", &T, &UR, &v) == 3) {
            float po = pontoOrvalho(T, UR);
            float wc = windChill(T, v);
            float hi = heatIndex(T, UR);

            snprintf(buffer, sizeof(buffer), "%.2f,%.2f,%.2f,%.2f,%s,%s\n", 
                     T, UR, v, po, 
                     isnan(wc) ? "" : String(wc, 2).c_str(), 
                     isnan(hi) ? "" : String(hi, 2).c_str());
            
            server.sendContent(buffer);
        }

        // Calcula progresso e envia via WebSocket a cada 5% para não flodar a rede
        int progress = (processedBytes * 100) / totalBytes;
        if (progress >= lastProgress + 5) {
            lastProgress = progress;
            String msg = "{\"progress\":" + String(progress) + "}";
            webSocket.broadcastTXT(msg);
        }

        // Mantém as conexões vivas e evita reset do Watchdog (WDT)
        webSocket.loop(); 
        yield();
    }
    
    file.close();
    LittleFS.remove("/temp.csv");
    
    // Avisa o front que terminou
    webSocket.broadcastTXT("{\"progress\":100, \"done\":true}");
    server.sendContent("");
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
    // Apenas mantemos a conexão, o ESP32 atua como emissor (broadcast) do status
}

void setup() {
    Serial.begin(115200);
    LittleFS.begin(true);

    WiFi.softAP(ssid, password);
    
    webSocket.begin();
    webSocket.onEvent(webSocketEvent);

    server.on("/", HTTP_GET, handleRoot);
    server.on("/processar_csv", HTTP_POST, handleProcessCSV, handleFileUpload);
    server.begin();
}

void loop() {
    server.handleClient();
    webSocket.loop();
}