#include <stdio.h>

int funcaoT(int n);

int main (void){
    int numero;
    int t;
    printf("Digite o valor de N:\n");
    scanf("%d", &numero);
    t = funcaoT(numero);
    printf("%d\n", t);
}
    
int funcaoT(int n){
    int resultado;
    if (n < 10){
        return 10;
    }
    else if (n >= 10){
        int resultado = funcaoT(n / 5) + funcaoT(7 * n / 10 + 1) + n;        
        return resultado;
    }
}
