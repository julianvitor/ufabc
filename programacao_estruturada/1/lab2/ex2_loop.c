#include <stdio.h>

int main(void) {
    int numero;
    int contador = 1;
    int primo = 1;

    printf("Digite um número inteiro:\n");
    scanf("%d", &numero);

    for (contador; contador <= numero; contador++) {
        if (numero % contador == 0) {
            printf("O número %d é divisível por %d.\n", numero, contador);

            if (numero % contador == 0 && contador != 1 && contador != numero) {
                primo = 0;
            }
        }
    }

    if (primo == 1 && contador-1 == numero) {
        printf("O número %d é primo.\n", numero);
    }

    return 0;
}
