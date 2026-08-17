#include <stdio.h>
int main(void){
	float x, y;

//interface inicial
	printf("Digite o valor de x:\n");
	scanf("%f", &x);

//operacao soma y = (x^2)
	y = (x*x)-(2*x)+3;
	printf("O valor de y é: %.2f\n", y);

}
