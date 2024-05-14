#include<stdio.h>

int main(void)
{   
    int *ptr = NULL;
    int **uwu = NULL;

    int valor = 40;

    int sustituto = 2;

    ptr = &valor;
    printf("%i", *ptr);



    uwu = &ptr;
    printf("\n%i", **uwu);
    printf("\n");

    


    return 0;
}