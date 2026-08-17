#include <stdio.h>

void read_vec(int vetor[], int N) {
    for (int i = 0; i < N; i++) {
        scanf("%d", &vetor[i]);
    }
}

void show_vec(int vetor[], int N) {
    printf("O vetor é: ");
    for (int i = 0; i < N; i++) {
        printf("%d ", vetor[i]);
    }
    printf("\n");
}

void square_vec(int vetor[], int N) {
    for (int i = 0; i < N; i++) {
        vetor[i] = vetor[i] * vetor[i];
    }
}

int main() {
    int N;

    printf("Digite o tamanho do vetor: \n");
    scanf("%d", &N);

    int vetor[N];

    printf("Digite os elementos do vetor: \n");
    read_vec(vetor, N);

   
    show_vec(vetor, N);

    square_vec(vetor, N);

 
    show_vec(vetor, N);

    return 0;
}
