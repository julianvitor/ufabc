#include <stdio.h>

// Função para encontrar os números primos mais próximos abaixo e acima de n
void encontrarPrimos(int n, int *primoAbaixo, int *primoAcima);

int main(void) {
    int primoAbaixo, primoAcima;

    // Chamada da função encontrarPrimos com endereços de primoAbaixo e primoAcima
    encontrarPrimos(499, &primoAbaixo, &primoAcima);

    // Exibição dos resultados
    printf("%d\n", primoAbaixo);
    printf("%d\n", primoAcima);

    return 0;
}

// Implementação da função encontrarPrimos
void encontrarPrimos(int n, int *primoAbaixo, int *primoAcima) {
    // Verificar se n é primo
    int nPrimo = 1;  // Assumindo que n é primo

    for (int i = 2; i < n; i++) {
        if (n % i == 0) {
            nPrimo = 0;  // n não é primo, marca como não primo
            break;       // Sai do loop se n não é primo
        }
    }

    if (nPrimo) {
        *primoAbaixo = n;  // Atribui o valor de n a *primoAbaixo
        *primoAcima = n;   // Atribui o valor de n a *primoAcima
        return;            // Sai da função se n é primo
    }

    // Encontrar o número primo abaixo de n
    for (int i = n - 1; i > 1; i--) {
        int primo = 1;

        for (int j = 2; j < i; j++) {
            if (i % j == 0) {
                primo = 0;
                break;
            }
        }

        if (primo) {
            *primoAbaixo = i;
            break;
        }
    }

    // Encontrar o número primo acima de n
    for (int k = n + 1;; k++) {
        int primo = 1;

        for (int l = 2; l < k; l++) {
            if (k % l == 0) {
                primo = 0;
                break;
            }
        }

        if (primo) {
            *primoAcima = k;
            break;
        }
    }
}
