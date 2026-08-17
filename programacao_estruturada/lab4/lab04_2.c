#include <stdio.h>
#include <math.h>

float calculaDistancia(float Px1, float Px2, float Py1, float Py2);

int main(){
	float Px1, Px2, Py1, Py2;
	printf("Digite as coordenadas X e Y do ponto 1:\n");
	scanf("%f %f", &Px1, &Py1);
	printf("Digite as coordenadas X e Y do ponto 2:\n");
        scanf("%f %f", &Px2, &Py2);
	printf ("A distancia entre os pontos eh: %.2f.\n",calculaDistancia(Px1,Px2,Py1,Py2));
	return 0;
}

float calculaDistancia(float Px1, float Px2, float Py1, float Py2){
	float distancia;
	
	distancia = sqrt(pow(Px2-Px1,2)+pow(Py2-Py1, 2)), 2; 
	return distancia;
	

}
