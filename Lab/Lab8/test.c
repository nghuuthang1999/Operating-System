#include <stdio.h>
#include <stdlib.h>

#define SIZE (1 << 28)

int main(){
    while (1)
    {
        malloc(SIZE);
    }
    return 0;
}