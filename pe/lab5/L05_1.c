#include <stdio.h>

// Função para ler os valores do vetor
void read_vec(int vetor[], int N) {
    printf("Digite os elementos do vetor:\n");
    for (int i = 0; i < N; i++) {
        scanf("%d", &vetor[i]);
    }
}

// Função para exibir os valores do vetor
void show_vec(int vetor[], int N) {
    printf("O vetor é:\n");
    for (int i = 0; i < N; i++) {
        printf("%d ", vetor[i]);
    }
    printf("\n");
}

int main() {
    int N;

    printf("Digite o tamanho do vetor: ");
    scanf("%d", &N);

    int vetor[N];

    read_vec(vetor, N);
    show_vec(vetor, N);

    return 0;
}
