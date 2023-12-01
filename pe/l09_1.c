                 #include<stdlib.h>

struct Elemento{
                int valor;
                struct Elemento *proximo;
                };
struct Pilha{
                struct Elemento *topo; //aponta para o elemento que es>
        };

int main(void){
        typedef struct Elemento Elemento;

        typedef struct  Pilha Pilha;

        Pilha*cria() {
                pilha *pi = (Pilha *)malloc(sizeof(Pilha));//alocação >
                pi->topo = NULL;
                return pi;
        }
return 0;
}






