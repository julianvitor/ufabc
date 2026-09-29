#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define LINHAS 10
#define COLUNAS 5

void exibeMatriz(int a, int b, int m[a][b]) {
    for (int i = 0; i < a; i++) {
        for (int j = 0; j < b; j++) {
            printf("%d ", m[i][j]);
        }
        printf("\n");
    }
}

int main() {
    int matriz[LINHAS][COLUNAS];
    int transposta[COLUNAS][LINHAS];
    int seed;

    printf("Digite o valor da seed: ");
    scanf("%d", &seed);
    srand(seed);

    for (int i = 0; i < LINHAS; i++) {
        for (int j = 0; j < COLUNAS; j++) {
            matriz[i][j] = rand() % 10;
        }
    }

    printf("A matriz é:\n");
    exibeMatriz(LINHAS, COLUNAS, matriz);

    for (int i = 0; i < COLUNAS; i++) {
        for (int j = 0; j < LINHAS; j++) {
            transposta[i][j] = matriz[j][i];
        }
    }

    printf("\nA transposta é:\n");
    exibeMatriz(COLUNAS, LINHAS, transposta);

    return 0;
}
