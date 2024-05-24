#include <stdio.h>

int main(void)
{
    printf("FEDE SE LA COME DOBLADA\n");
    
    int x;
    printf("Digite numero: ");
    scanf("%i", &x);
    printf("%i", x);

    int num;
    char edad[40];
    printf("\nDigite DNI y edad: ");
    scanf("%i %39s", &num, edad);
    printf("DNI: %i - Edad: %s", num, edad);

    return 0;
}