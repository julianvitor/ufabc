#include <stdio.h>
int main (void){
        float nota;
        printf("Digite sua nota:\n");
        scanf("%f", &nota);

        if(nota<5){
                printf("Conceito F - reprovado\n");
        }
        else if(nota>=5 && nota<6){
                printf("Conceito D - desempenho mínimo\n");
        }
        
        else if(nota>=6 && nota<7){
                printf("Conceito C - desempenho adequado\n");
        }
        
        else if(nota>=7 && nota<8.5){
                printf("Conceito B - bom desempenho\n");
        }

        else if(nota>=8.5 && nota <=10){
                printf("Conceito A - desempenho excepcional\n");
        }

        else{
                printf("Valor incompativel\n");
        }
return 0;
}
