const int ledPin = 2; // Define o pino do LED
const int freq = 1000; // Frequência em Hz
const int ledChannel = 0; // Canal PWM

void setup() {
  pinMode(ledPin, OUTPUT);
  
  // Configuração do canal PWM
  ledcSetup(ledChannel, freq, 8); // Configura o canal com frequência de 1 kHz e resolução de 8 bits
  ledcAttachPin(ledPin, ledChannel); // Associa o canal PWM ao pino
}

void loop() {
  // Define o ciclo de trabalho (duty cycle) do PWM
  // Um ciclo de trabalho de 50% gerará um pulso de 1 kHz
  int dutyCycle = 128; // 50% de 255 (resolução de 8 bits)
  
  // Atualiza o ciclo de trabalho do PWM
  ledcWrite(ledChannel, dutyCycle);
  
  // Aguarda um curto período de tempo
  delay(10);
}
