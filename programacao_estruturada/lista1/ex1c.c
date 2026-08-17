#include <stdio.h>
int main(void){
    int a,b,c;
    printf("insira a,b,c:\n");
    scanf("%d%d%d",&a,&b,&c);
    printf("%d\n",a);
    printf("%d\n",b);
    printf("%d\n",c);
    if(a>b && a>c){
        printf("%d\n",a);
        if (b>c){
            printf("%d\n",b);
            printf("%d\n",c);
        }
        else{
            printf("%d\n",c);
            printf("%d\n",b);
        }
    }

    else if(b>a && b>c){
        printf("%d\n",b);
        if (a>c){
            printf("%d\n",a);
            printf("%d\n",c);
        }
        else{
            printf("%d\n",c);
            printf("%d\n",a);
        }
    }

    else if(c>a && c>b){
        printf("%d\n",c);
        if(a>b){
            printf("%d\n",a);
            printf("%d\n",b);
        }
        else{
            printf("%d\n",b);
            printf("%d\n",a);
        }
    }
    return 0;
}