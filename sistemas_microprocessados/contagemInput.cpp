const int pin2 = 2; // Define o número do pino como 2
unsigned long startTime; // Variável para armazenar o tempo inicial
unsigned int contagem = 0; // Variável de controle para armazenar o número de vezes que o pino recebeu entrada

void setup() {
  pinMode(pin2, INPUT); // Define o pino como entrada
  Serial.begin(9600); // Inicia comunicação serial
  startTime = millis(); // Inicia contagem de tempo
}

void loop() {
  if (millis() - startTime <= 10000) { // Executa por 10 segundos
    if (digitalRead(pin2) == HIGH) { // Se o pino receber entrada
      contagem++; // Incrementa a contagem
      delay(50); // Pequeno atraso para evitar leituras repetidas
    }
  } else { // Após 10 segundos
    Serial.print("Número de vezes que o pino recebeu entrada em 10 segundos: ");
    Serial.println(contagem); // Exibe a contagem
    contagem = 0; // Reinicia a contagem
    startTime = millis(); // Reinicia a contagem de tempo
  }
}
