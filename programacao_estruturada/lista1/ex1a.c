#include <stdio.h>

int main (){
    char produto1[20];
    char produto2[20];
    float valor1;
    float valor2;
    
    printf("Nome do produto 1 : ");
    scanf("%s", produto1);
    printf ("Nome do produto 2 : ");
    scanf("%s", produto2);
    
    printf("valor do produto 1 ex:(20.50) : ");
    scanf("%f", &valor1);
    printf("valor do produto 2 ex:(20.50) : ");
    scanf("%f", &valor2);

    if(valor1 > 20.00){
        printf("O produto %s tem um preço superior a 20 reais.\n", produto1);
    }

    if(valor2 > 20.00){
        printf("O produto %s tem um preço superior a 20 reais.\n", produto2);
    }

    if(valor1 < 10.00){
        printf("O produto %s tem um preço: %f\n", produto1, valor1);
    }

    if(valor2 < 10.00){
        printf("O produto %s tem um preço: %f\n", produto2, valor2);
    }


}


