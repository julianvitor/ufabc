#include <stdio.h>
int compara(int a, int b);

int main(){
	printf("Digite dois valores: \n");
	int a;
	int b;
	scanf("%d %d", &a, &b);
	printf("O menor número eh %d.\n", compara(a,b));

}
int compara(int a, int b){
     if (a<b){
     	return a;
     }

     else {
	return b;
     }

}
