#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int MAX_CHARS = 16;
int MAX_ARGS = 8;

/* expect this to be buggy, do mind it splits by spaces except quoted spaces
   escaped quotes do not function like a quote in the parser, but as a character */
char parse(char *pnt_cmd, int cmd_len) {
    // boring memory allocation and setup
    int quoted = 0;
    int escaped = 0;
    char (*buffer)[MAX_ARGS] = malloc(sizeof(char[MAX_CHARS][MAX_ARGS]));
    int alloc_size = sizeof(char[MAX_CHARS]); // o(n^2)
    char *buffer_string = malloc(alloc_size);
    int i = 0; // i use 2 indexes, i represents index of command, buffer_i represents character in the buffer
    int buffer_i = 0;
    // fun logic stuff
    while (i < cmd_len) {
        if (pnt_cmd[i] == ' ') {
            
            memset(buffer_string, 0, alloc_size);
        }
        if (pnt_cmd[i] == '\\') {
            escaped = 1;
            continue;
        }
        if (!(escaped || quoted || pnt_cmd[i] == '\n' || pnt_cmd[i] == '\\')) {
            buffer[i] = pnt_cmd[i];
        }
        if (escaped) {
            escaped = 0;
            continue;
        }
        i++;
        buffer_i++;
    }
    return buffer;
}

int main() {
    printf("MIN7 PACKAGE MANAGER SHELL\n");

    while (true) { // main loop
        // get commands
        printf("%s", ">- ");
        char cmd[64];
        fgets(cmd,65,stdin);
        printf("command: \n",cmd);
        int cmd_len = strlen(cmd);
        char splt_cmd[MAX_CHARS][MAX_ARGS] = parse(cmd, cmd_len);
        
        // commands
        if (splt_cmd[0] == "lping") {
            printf("%s\nlocal pong - swish!");
        } else {
            printf("%s\ncommand not found");
        }
    }

    return 0;
}