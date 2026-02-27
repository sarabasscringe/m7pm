#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

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

int r_error(int type, char *error) { // error system
    long int fsz;
    char *fptr = getfile("resources/config.txt", &fsz);
    char (*config)[MAX_CHARS][MAX_ARGS];
    int configlen;
    config = parse(fptr, fsz, &configlen);
    free(fptr);
    if (type == 1) {
        printf(col_grn"[err1]"col_reset " > %s", error);
    } else if (type == 2) {
        printf(col_ylw"[err2]"col_reset " > %s", error);
    } else if (type == 3) {
        printf(col_red"[err3]"col_reset " > %s", error);
    } else if (type == 4) {
        printf(col_blu"[err4]"col_reset " > %s", error);
    } else if (type == 5) {
        printf(col_mag"[err5]"col_reset " > %s", error);
    } else {
        printf("undetermined error\nerr type: %d\nerr message: %s");
    }
    if (type != 5 && strcmp((*config)[2],"y") == 0) {
        printf("\ntake contact at saraespedal8@gmail.com / sarabasscringe (discord)\nor open an issue on github.com/sarabasscringe/m7pm if you believe this was a mistake in the program");
    }
    printf("\n");
    free(config);
    return 0;
}

int printls(char *parent_dir) {
    DIR *dir = opendir(parent_dir);
    struct dirent *dp;
    if (!dir) {
        char buffer[128];
        snprintf(buffer, sizeof(char [128]), "directory `%s` has an issue when opening", parent_dir);
        r_error(5, buffer);
        return 1;
    }
    int count = 0;
    while ((dp = readdir(dir)) != NULL) {
        if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0) {continue;}
        count++;
        char path[1024];
        snprintf(path, sizeof(path), "%s/%s", parent_dir, dp->d_name);
        struct stat st;
        stat(path, &st);
        if (stat(dp->d_name, &st) == 0) {
            if (S_ISDIR(st.st_mode)) {
                printf(col_cyn" [dir]:: %s\n"col_reset, dp->d_name);
            } else {
                printf(col_grn"[file]:: %s\n"col_reset, dp->d_name);
            }
        }
    }
    if (count == 0) {
        char buffer[128];
        snprintf(buffer, sizeof(char [128]), "directory `%s` is empty, no error", parent_dir);
        r_error(5, buffer);
        return 1;
    }
    closedir(dir);
    return 0;
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
            if (strcmp((*splt_cmd)[1], "config") == 0) {
                long int fsz;
                char *fptr = getfile("resources/config_help.md", &fsz); // remember to free fptr, its a pointer to the file as a string
                printf(col_ylw"%s\n"col_reset,fptr);
                free(fptr);
            } else if (strcmp((*splt_cmd)[1], "error") == 0) {
                long int fsz;
                char *fptr = getfile("resources/error_help.md", &fsz); // remember to free fptr, its a pointer to the file as a string
                printf(col_red"%s\n"col_reset,fptr);
                free(fptr);
            } else {
                long int fsz;
                char *fptr = getfile("resources/help.txt", &fsz); // remember to free fptr, its a pointer to the file as a string
                printf(col_blu"%s\n"col_reset,fptr);
                free(fptr);
            }
        } else

        if (strcmp((*splt_cmd)[0], "cd") == 0) {
            if (chdir((*splt_cmd)[1]) != 0) {
                char *str_const = "directory `%s` does not exist or there was an unknown error";
                char buffer[cmd_sz + sizeof(str_const)];
                snprintf(buffer, cmd_sz, str_const, (*splt_cmd)[1]);
                r_error(1,buffer);
            } else {
                printf("changed directory to `%s`\n", (*splt_cmd)[1]);
            }
        } else

        if (strcmp((*splt_cmd)[0], "pwd") == 0) {
            printf("current working directory: `%s`\n", wd);
        } else

        if (strcmp((*splt_cmd)[0], "pconfig") == 0) {
            if (strcmp((*config)[1], "y") == 0) {
                r_error(1,"you dont have to push configs silly");
            } else {
                printf("pushing config from config.txt..");
                long int fsz;
                char *fptr = getfile("resources/config.txt", &fsz);
                char (*config)[MAX_CHARS][MAX_ARGS];
                int configlen;
                config = parse(fptr, fsz, &configlen);
                free(fptr);
            }
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
                    r_error(2,"error trying to get operating system");
                }
            } else {
                if (printls(wd) != 0) {
                    r_error(4, "error in ls function (printls())");
                }
            }
        } else
        // 404
        {
            char *str_const = "command `%s` is not valid";
            int sz = cmd_sz + sizeof(str_const);
            char buffer[sz];
            snprintf(buffer, sz, str_const, cmd);
            r_error(1,buffer);
        }
        // finish up
        free(splt_cmd);
        if ((*config)[1] == "y") {
            free(config);
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