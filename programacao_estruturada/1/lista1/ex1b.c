#include <stdio.h>
int main(void){
    int valor, prestacoes;
    float valor_prestacoes;
    printf("insira valor do produto:\n");
    scanf("%d", &valor);
    printf("insira o numero de prestações\n");
    scanf("%d", &prestacoes);
    if (prestacoes == 3){
        valor_prestacoes = ((valor*1.1)/3);
    }
    else if (prestacoes == 5){
        valor_prestacoes = ((valor*1.2)/5);
    }

    printf("o valor da prestacao e: R$%f\n", valor_prestacoes);

    return 0;
}