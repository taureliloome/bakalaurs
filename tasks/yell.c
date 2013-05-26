#include <stdio.h>
#include <stdlib.h>

int a = 2;
int yell(int val){
    return val;
}

int main(){
    int a = 0;
    printf("%d %d",a,yell(2));
    return 0;
}
