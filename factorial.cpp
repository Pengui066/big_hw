#include "functions.h"

int factorial(int n) {
    int tmp = n;
    while (--n)
    {
        tmp *= n;
    }
    return tmp;
}