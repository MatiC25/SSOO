#include <stdio.h>

void suma(int a, int b, int* c)
{
    *c = a + b;

}

int main(void)
{   
    int x = 3;
    int y = 4;
    int c = 10;
    suma (x, y, &c);
    printf("%d", c);
    return 0;
}