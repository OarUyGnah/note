#include "mm.h"
#include <stdio.h>


int main() {
    mm_init();
    int *arr = (int*)mm_malloc(10 * sizeof(int));
    for (int i = 0; i < 10; i++) {
        arr[i] = i;
    }
    for (int i = 0; i < 10; i++) {
        printf("%d\n",arr[i]);
    }
    fprintf(stderr,"%d\n",1 << 12);
    return 0;
}