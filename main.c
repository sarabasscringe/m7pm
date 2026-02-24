#include <stdio.h>

int main() {
    printf("MIN7 PACKAGE MANAGER SHELL\n");
    printf("%s", ">- ");
    char cmd[64];
    fgets(cmd,65,stdin);
    printf("%s\n",cmd);
    return 0;
}