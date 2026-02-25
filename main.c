#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_ARGS 8
#define MAX_CHARS 16

/* expect this to be buggy, do mind it splits by spaces except quoted spaces
   escaped quotes do not function like a quote in the parser, but as a character */
char (*parse(char *pnt_cmd, int cmd_len, int *depth))[MAX_CHARS][MAX_ARGS] {
    // boring memory allocation and setup
    int quoted = 0;
    int escaped = 0;
    char (*buffer)[MAX_CHARS][MAX_ARGS] = malloc(sizeof(char[MAX_CHARS][MAX_ARGS]));
    int alloc_size = sizeof(char[MAX_CHARS]); // o(n^2)
    char *buffer_string = malloc(alloc_size);
    (*depth) = 0; // length of array
    int i = 0; // i use 2 indexes, i represents index of raw command string, buffer_i represents character in the string buffer
    int buffer_i = 0;
    // fun logic stuff
    while (i < cmd_len) {
        if (pnt_cmd[i] == ' ' && !buffer_string[0] == '\0') {
            buffer_string[buffer_i] = '\0';
            strncpy((*buffer)[*depth], buffer_string, MAX_CHARS-1);
            (*buffer)[*depth][MAX_CHARS-1] = '\0';
            (*depth)++;
            memset(buffer_string, 0, alloc_size);
            buffer_i = 0;
            escaped = 0;
            i++;
            continue;
        }
        if (pnt_cmd[i] == '"') {
            if (escaped) {
                escaped = 0;
            } else {
                if (quoted) {
                    quoted = 0;
                } else {
                    quoted = 1;
                }
                continue;
            }
        }
        if (pnt_cmd[i] == '\\') {
            escaped = 1;
            continue;
        }
        if (!(escaped || quoted || pnt_cmd[i] == '\n' || pnt_cmd[i] == '\\' || pnt_cmd[i] == ' ')) {
            buffer_string[buffer_i] = pnt_cmd[i]; // append to string
            buffer_i++;
        }
        if (escaped) {
            escaped = 0;
            continue;
        }
        i++;
    }
    if (!buffer_string[0] == '\0') {
        buffer_string[buffer_i] = '\0';
        strncpy((*buffer)[*depth], buffer_string, MAX_CHARS-1);
        (*buffer)[*depth][MAX_CHARS-1] = '\0';
        (*depth)++;
    }
    free(buffer_string);
    return buffer;
}

int main() {
    printf("MIN7 PACKAGE MANAGER SHELL\n");

    while (true) { // main loop
        // get commands
        printf("%s", ">- ");
        char cmd[64];
        fgets(cmd,64,stdin);
        cmd[strcspn(cmd, "\n")] = '\0';
        printf("command: %s\n",cmd);
        int cmd_len = strlen(cmd);
        char (*splt_cmd)[MAX_CHARS][MAX_ARGS];
        int spltcmdl;
        splt_cmd = parse(cmd, cmd_len, &spltcmdl); // spltcmdl is the length of the splt_cmd array
        printf("cmd args amt: %d\n", spltcmdl); // debugging purpose only
        for (int i = 0;i < spltcmdl;i++) {
            printf("cmd part %d",i);
            printf(" = |%s|\n",(*splt_cmd)[i]);
        }
        printf("\n");
        // commands
        if (strcmp((*splt_cmd)[0], "lping") == 0) {
            printf("local pong - swish!\n");
        } else {
            printf("command not found\n");
        }
        free(splt_cmd);
    }

    return 0;
}