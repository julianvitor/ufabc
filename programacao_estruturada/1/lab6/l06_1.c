#include <stdio.h>
//variaveis globais
int x, y, r;
//ponteiros globais
int *px = &x;
int *py = &y;
int *pr = &r;
//prototipos de funções
int soma(int *px,int *py);
int subtracao(int *px, int *py);
int multiplicacao(int *px, int *py);
int divisao(int *px, int*py);
int resto(int *px, int *py);


int main (void){
//io
        printf("Digite dois valores inteiros:\n");
        scanf("%d", &x);
        scanf("%d", &y);
        printf("A soma é: %d\n", soma(px, py));
        printf("A subtração é: %d\n", subtracao(px,py));
        printf("A multiplicação é: %d\n", multiplicacao(px, py));
        printf("A divisão é: %d\n",divisao(px, py));
        printf("O resto é: %d\n", resto(px, py));
return 0;
}

//funcoes matematicas
int soma(int *px, int *py){
r = *px + *py;
return r;
}

int subtracao(int *px, int*py){
r = *px - *py;
return r;
}

int multiplicacao(int *px, int*py){
r = *px * *py;
return r;
}

int divisao(int *px, int*py){
r = (*px)/(*py);
return r;
}

int resto(int *px, int *py){
r = *px % *py;
return r;
}
