#include <LittleFS.h>
#include <WebServer.h>
#include <WebSocketsServer.h> // Instalar via Library Manager
#include <WiFi.h>
#include <math.h>

const char *ssid = "ESP32_Meteorologia";
const char *password = "";

WebServer server(80);
WebSocketsServer webSocket(81);
File uploadFile;

// --- INSIRA O HTML AQUI (Use o html anterior, mas com o JS atualizado abaixo)
// ---
const char index_html[] PROGMEM = R"rawliteral(...)rawliteral";

// Funções de Meteorologia
float pontoOrvalho(float T, float UR) {
  float a = 17.27;
  float b = 237.7;
  float alpha = ((a * T) / (b + T)) + log(UR / 100.0);
  return (b * alpha) / (a - alpha);
}

float windChill(float T, float V) {
  if (T > 10.0 || V <= 4.8)
    return NAN;
  return 13.12 + 0.6215 * T - 11.37 * pow(V, 0.16) + 0.3965 * T * pow(V, 0.16);
}

float heatIndex(float T, float UR) {
  if (T < 26.7)
    return NAN;
  float tf = (T * 9.0 / 5.0) + 32.0;
  float c1 = -42.379, c2 = 2.04901523, c3 = 10.14333127, c4 = -0.22475541;
  float c5 = -0.00683783, c6 = -0.05481717, c7 = 0.00122874;
  float c8 = 0.00085282, c9 = -0.00000199;

  float hi_f = c1 + c2 * tf + c3 * UR + c4 * tf * UR + c5 * tf * tf +
               c6 * UR * UR + c7 * tf * tf * UR + c8 * tf * UR * UR +
               c9 * tf * tf * UR * UR;
  return (hi_f - 32.0) * 5.0 / 9.0;
}
void handleRoot() { server.send(200, "text/html", index_html); }

void handleCalcularManual() {
  if (!server.hasArg("T") || !server.hasArg("UR") || !server.hasArg("V")) {
    server.send(400, "application/json", "{\"error\":\"Parâmetros faltando\"}");
    return;
  }
  float T = server.arg("T").toFloat();
  float UR = server.arg("UR").toFloat();
  float V = server.arg("V").toFloat();

  float po = pontoOrvalho(T, UR);
  float wc = windChill(T, V);
  float hi = heatIndex(T, UR);

  char buffer[256];
  snprintf(buffer, sizeof(buffer),
           "{\"po\":%.2f, \"wc\":\"%s\", \"hi\":\"%s\"}", po,
           isnan(wc) ? "N/A" : String(wc, 2).c_str(),
           isnan(hi) ? "N/A" : String(hi, 2).c_str());

  server.send(200, "application/json", buffer);
}

void handleFileUpload() {
  HTTPUpload &upload = server.upload();
  if (upload.status == UPLOAD_FILE_START) {
    String filename = upload.filename;
    if (!filename.startsWith("/"))
      filename = "/" + filename;
    uploadFile = LittleFS.open("/temp.csv", FILE_WRITE);
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    if (uploadFile) {
      uploadFile.write(upload.buf, upload.currentSize);
    }
  } else if (upload.status == UPLOAD_FILE_END) {
    if (uploadFile) {
      uploadFile.close();
    }
  }
}
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
  server.sendContent(
      "Temperatura,Umidade,Vento,Ponto_Orvalho,Wind_Chill,Heat_Index\n");

  bool headerSkip = true;
  char buffer[128];
  int lastProgress = 0;

  while (file.available()) {
    String line = file.readStringUntil('\n');
    processedBytes += line.length() + 1; // +1 para o \n removido
    line.trim();

    if (line.length() == 0)
      continue;
    if (headerSkip) {
      headerSkip = false;
      continue;
    }

    float T = 0, UR = 0, v = 0;
    if (sscanf(line.c_str(), "%f,%f,%f", &T, &UR, &v) == 3) {
      float po = pontoOrvalho(T, UR);
      float wc = windChill(T, v);
      float hi = heatIndex(T, UR);

      snprintf(buffer, sizeof(buffer), "%.2f,%.2f,%.2f,%.2f,%s,%s\n", T, UR, v,
               po, isnan(wc) ? "" : String(wc, 2).c_str(),
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

void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload,
                    size_t length) {
  // Apenas mantemos a conexão, o ESP32 atua como emissor (broadcast) do status
}

void setup() {
  Serial.begin(115200);
  LittleFS.begin(true);

  // Configurando um IP fixo para o ESP32 (192.168.4.1)
  IPAddress local_ip(192, 168, 4, 1);
  IPAddress gateway(192, 168, 4, 1);
  IPAddress subnet(255, 255, 255, 0);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  WiFi.softAP(ssid, password);

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  server.on("/", HTTP_GET, handleRoot);
  server.on("/calcular", HTTP_GET, handleCalcularManual);
  server.on("/processar_csv", HTTP_POST, handleProcessCSV, handleFileUpload);
  server.begin();
}

void loop() {
  server.handleClient();
  webSocket.loop();
}