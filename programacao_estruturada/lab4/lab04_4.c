#include <stdio.h>
int decideSePrimo(int num, int div);

int main(void){
    int entrada;
    int resultado;
    printf("Digite um valor inteiro:\n ");
    scanf("%d", &entrada);
    
    resultado = decideSePrimo(entrada, entrada-1);
    
    if(resultado == 1){
        printf ("O número %d é primo.", entrada);
    }
    
    else if (resultado == 0){
        printf ("O número %d não é primo.", entrada);
   
    }
    
}

int decideSePrimo(int num, int div){
    if (div == 1 ){
        return 1;
    }
    else if (num%div == 0){
        return 0;
    }
    else {
        return decideSePrimo(num, (div-1));
    }
    
}
