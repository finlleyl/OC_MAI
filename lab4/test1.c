#include <stdio.h>
#include "e_lib.h"

int main() {
    int x;
    printf("Insert X: ");
    scanf("%d", &x);
    float result = E(x);
    printf("E(%d) = %f\n", x, result);

    return 0;
}