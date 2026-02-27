#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_ARGS 16
#define MAX_CHARS 64
// thanks to whoever posted these on stack overflow youre cool
#define col_red     "\x1b[31m"
#define col_grn   "\x1b[32m"
#define col_ylw  "\x1b[33m"
#define col_blu    "\x1b[34m"
#define col_mag "\x1b[35m"
#define col_cyn    "\x1b[36m"
#define col_reset   "\x1b[0m"


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
        if (pnt_cmd[i] == ' ' && !(buffer_string[0] == '\0' || escaped || quoted)) {
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
                i++;
                continue;
            }
        }
        if (pnt_cmd[i] == '\\') {
            escaped = 1;
            continue;
        }
        if (!(escaped || pnt_cmd[i] == '\n' || pnt_cmd[i] == '\\' || (pnt_cmd[i] == ' ' && !quoted))) {
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

int r_error(int type, char *error) { // error system
    if (type == 1) {
        printf(col_red"[err1]"col_reset " > %s", error);
    } else {
        printf("undetermined error\nerr type: %d\nerr message: %s");
    }
    printf("\n");
    return 0;
}

int printls() {
    char parent_dir[1024];
    getcwd(parent_dir, sizeof(char [1024]));
    return 0;
}

char *getfile(char *filename, long int *filesz) { // remember to free the pointer to buffer
    FILE *fptr = fopen(filename,"rb");
    fseek(fptr,0,SEEK_END);
    *filesz = ftell(fptr);
    rewind(fptr);
    char *buffer = malloc(*filesz + 1);
    size_t read = fread(buffer, 1, *filesz, fptr); // i honestly have no idea what size_t does i just found it on stackoverflow
    buffer[read] = '\0'; // to my credit i understand this line to an extent
    return buffer;
}

int main() {
    printf("MIN7 PACKAGE MANAGER SHELL\nuse command `help` for a list of commands\n");
    // init config
    long int fsz;
    char *fptr = getfile("resources/config.txt", &fsz);
    char (*config)[MAX_CHARS][MAX_ARGS];
    int configlen;
    config = parse(fptr, fsz, &configlen);
    free(fptr);
    while (true) { // main loop
        // make useful stuff ahead of time
        char wd[1024];
        getcwd(wd, sizeof(char [1024]));
        // get commands
        printf("%s>- ", wd);
        char cmd[64];
        fgets(cmd,64,stdin);
        cmd[strcspn(cmd, "\n")] = '\0';
        // printf("command: %s\n",cmd);
        int cmd_len = strlen(cmd);
        int cmd_sz = sizeof(cmd);
        char (*splt_cmd)[MAX_CHARS][MAX_ARGS];
        int spltcmdl;
        splt_cmd = parse(cmd, cmd_len, &spltcmdl); // spltcmdl is the length of the splt_cmd array
        /* printf("cmd args amt: %d\n", spltcmdl); // debugging purpose only just ignore it i guess
        for (int i = 0;i < spltcmdl;i++) {
            printf("cmd part %d",i);
            printf(" = |%s|\n",(*splt_cmd)[i]);
        }
        printf("\n"); */
        // commands
        if (strcmp((*splt_cmd)[0], "lping") == 0) {
            printf("local pong - swish!\n");
        } else 
        if (strcmp((*splt_cmd)[0], "help") == 0) {
            long int fsz;
            char *fptr = getfile("resources/help.txt", &fsz); // remember to free fptr, its a pointer to the file as a string
            printf("%s",fptr);
            free(fptr);
        } else
        if (strcmp((*splt_cmd)[0], "cd") == 0) {
            if (chdir((*splt_cmd)[1]) != 0) {
                printf("error in::%s;\ndirectory might not exist, or there was an unspecified error\n", cmd);
                printf("take contact at saraespedal8@gmail.com / sarabasscringe (discord)\nor open an issue on github.com/sarabasscringe/m7pm\n");
            } else {
                printf("changed directory to %s\n", (*splt_cmd)[1]);
            }
        } else
        if (strcmp((*splt_cmd)[0], "pwd") == 0) {
            printf("current working directory: %s\n", wd);
        } else
        if (strcmp((*splt_cmd)[0], "ls") == 0) {
            if (strcmp((*splt_cmd)[1], "os") == 0) {
                if (strcmp((*config)[0], "w") == 0) {
                    printf("using ls in powershell\n");
                    system("dir");
                } else
                if (strcmp((*config)[0], "lx") == 0) {
                    printf("using ls in shell\n");
                    system("ls");
                } else {
                    r_error('1',"error in the config file, or invalid operating system in resources/config.txt");
                }
            
            } else {
                if (printls() != 0) {

                }
            }
        }
        // 404
        else {
            char buffer[cmd_sz];
            snprintf(buffer, cmd_sz, "command <%s> is not valid", cmd);
            r_error(1,buffer);
        }
        free(splt_cmd);
        free(config);
        if ((*config)[1] == "y") {
            long int fsz;
            char *fptr = getfile("resources/config.txt", &fsz);
            char (*config)[MAX_CHARS][MAX_ARGS];
            int configlen;
            config = parse(fptr, fsz, &configlen);
            free(fptr);
        }
    }

    return 0;
}