const int sensorPin = A0;  // Pino analógico onde o sensor está conectado
int ponto_alto = 4300;    // Valor pré-estabelecido para ponto alto
int ponto_baixo = 3000;   // Valor pré-estabelecido para ponto baixo
int max_leituras = 150;   // Número máximo de leituras da tensão

int picos = 0;
int ciclos_completos = 0;
unsigned long tempo_inicial;
unsigned long tempo_final;

void setup() {
  Serial.begin(9600);  // Inicializa a comunicação serial
  tempo_inicial = millis();  // Registra o tempo inicial
}

void loop() {
  int tensao_atual = analogRead(sensorPin);  // Lê o valor da tensão do sensor

  if (tensao_atual >= ponto_alto) {
    picos++;
  } else if (tensao_atual <= ponto_baixo && picos > 0) {
    ciclos_completos++;
    picos = 0;
  }

  delay(50);  // Aguarda 50 milissegundos
  tempo_final = millis();  // Registra o tempo final

  if (ciclos_completos >= max_leituras) {
    float tempo_total_segundos = (tempo_final - tempo_inicial) / 1000.0;  // Calcula o tempo total em segundos
    float batidas = ciclos_completos / 2.0;
    float tempo_total_minutos = tempo_total_segundos / 60.0;
    float frequencia_cardiaca = batidas / tempo_total_minutos;  // Converte para bpm

    if (frequencia_cardiaca < 40) {
      Serial.println("ERRO: MEDIDA INFERIOR AO MINIMO");
    } else if (frequencia_cardiaca > 220) {
      Serial.println("ERRO: MEDIDA SUPERIOR AO MAXIMO");
    } else {
      Serial.print("Frequência cardíaca: ");
      Serial.print(frequencia_cardiaca);
      Serial.println(" bpm");
    }

    while (true) {
      // Permanece em um loop infinito após o cálculo
    }
  }
}
