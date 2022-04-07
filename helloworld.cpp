#include <stdio.h>

int isRelativePrime( int a , int b)
{
    int min_value = a < b ? a : b;

    for( int i=2; i<= min_value; ++i)
        if( a%i == 0 && b%i==0 )
            return 0;

    return 1;
}


int main()
{
    printf(" \n\n Relative Prime \n\n");

    int a = 5;
    int b = 8;

    printf( "a = %d ", a);
    printf( "b = %d ", b);

    printf(" GCD(a,b) == 1 ?? %s", isRelativePrime(a,b) ? "TRUE" : "FALSE");

    printf("\n\n\n");

    return 0;
}