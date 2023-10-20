#include <stdio.h>
int somaDivisores(int numero_positivo);
int inteiroPositivo(int numero_entrada);
int main(){
	int numero_entrada = 0;

	printf("Digite o  1 número:\n");
        scanf("%d", &numero_entrada);
	printf("A soma dos divisores: %d\n", somaDivisores(inteiroPositivo(numero_entrada)));
	
	printf("Digite o  2 número:\n");
        scanf("%d", &numero_entrada);
        printf("A soma dos divisores: %d\n", somaDivisores(inteiroPositivo(numero_entrada)));
        
	printf("Digite o  3 número:\n");
        scanf("%d", &numero_entrada);
        printf("A soma dos divisores: %d\n", somaDivisores(inteiroPositivo(numero_entrada)));
	
	printf("Digite o  4 número:\n");
        scanf("%d", &numero_entrada);
        printf("A soma dos divisores: %d\n", somaDivisores(inteiroPositivo(numero_entrada)));

        printf("Digite o  5 número:\n");
        scanf("%d", &numero_entrada);
        printf("A soma dos divisores: %d\n", somaDivisores(inteiroPositivo(numero_entrada)));

	return 0;
}

int inteiroPositivo(int numero_entrada){
	if (numero_entrada > 0){
		return numero_entrada;
	}
}

int somaDivisores(int numero_positivo){
	int contador_divisor = 1;
	int soma = 0;
	if(numero_positivo != 0){
		for (int contador_divisor = 1; contador_divisor < numero_positivo; contador_divisor++){
			if ((numero_positivo % contador_divisor) == 0){
				soma += contador_divisor;}
		}
	return soma;
	}
}
