// Definindo as dimensões das matrizes
const int linhas = 3;
const int colunas = 3;

// Matrizes de entrada e saída
int matrizA[linhas][colunas];
int matrizB[linhas][colunas];
int matrizC[linhas][colunas];

void setup() {
  Serial.begin(115200); // Abre a porta serial com taxa de transmissão de 115200 bps
  
  // Preencher as matrizes A e B com números aleatórios
  randomSeed(analogRead(0)); // Inicializa a semente do gerador de números aleatórios com um valor variável
  preencherMatriz(matrizA);
  preencherMatriz(matrizB);

  // Somar as matrizes
  somarMatrizes(matrizA, matrizB, matrizC);

  // Exibir a matriz resultante via Serial
  exibirMatrizSerial("Matriz Resultante C:", matrizC);
}

void loop() {
  // Este código não faz nada no loop, pois a operação é feita apenas uma vez na função setup()
}

// Função para preencher uma matriz com números aleatórios
void preencherMatriz(int matriz[][colunas]) {
  for (int i = 0; i < linhas; i++) {
    for (int j = 0; j < colunas; j++) {
      matriz[i][j] = random(10); // Gera números aleatórios entre 0 e 9
    }
  }
}

// Função para somar duas matrizes
void somarMatrizes(int matrizA[][colunas], int matrizB[][colunas], int matrizC[][colunas]) {
  for (int i = 0; i < linhas; i++) {
    for (int j = 0; j < colunas; j++) {
      matrizC[i][j] = matrizA[i][j] + matrizB[i][j];
    }
  }
}

// Função para exibir uma matriz via Serial
void exibirMatrizSerial(const char* mensagem, int matriz[][colunas]) {
  Serial.println(mensagem);
  for (int i = 0; i < linhas; i++) {
    for (int j = 0; j < colunas; j++) {
      Serial.print(matriz[i][j]);
      Serial.print("\t");
    }
    Serial.println();
  }
}
