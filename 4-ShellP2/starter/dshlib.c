#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <errno.h>
#include <ctype.h>
#include "dshlib.h"

/*
 * Implement your exec_local_cmd_loop function by building a loop that prompts the 
 * user for input.  Use the SH_PROMPT constant from dshlib.h and then
 * Implement your exec_local_cmd_loop function by building a loop that prompts the
 * user for input. Use the SH_PROMPT constant from dshlib.h and then
 * use fgets to accept user input.
 * 
 *      while(1){
 *        printf("%s", SH_PROMPT);
 *        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL){
 *           printf("\n");
 *           break;
 *        }
 *        //remove the trailing \n from cmd_buff
 *        cmd_buff[strcspn(cmd_buff,"\n")] = '\0';
 * 
 *        //IMPLEMENT THE REST OF THE REQUIREMENTS
 *      }
 * 
 *   Also, use the constants in the dshlib.h in this code.  
 *      SH_CMD_MAX              maximum buffer size for user input
 *      EXIT_CMD                constant that terminates the dsh program
 *      SH_PROMPT               the shell prompt
 *      OK                      the command was parsed properly
 *      WARN_NO_CMDS            the user command was empty
 *      ERR_TOO_MANY_COMMANDS   too many pipes used
 *      ERR_MEMORY              dynamic memory management failure
 * 
 *   errors returned
 *      OK                     No error
 *      ERR_MEMORY             Dynamic memory management failure
 *      WARN_NO_CMDS           No commands parsed
 *      ERR_TOO_MANY_COMMANDS  too many pipes used
 *   
 *   console messages
 *      CMD_WARN_NO_CMD        print on WARN_NO_CMDS
 *      CMD_ERR_PIPE_LIMIT     print on ERR_TOO_MANY_COMMANDS
 *      CMD_ERR_EXECUTE        print on execution failure of external command
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 1+)
 *      malloc(), free(), strlen(), fgets(), strcspn(), printf()
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 2+)
 *      fork(), execvp(), exit(), chdir()
 *
 * while(1){
 *   printf("%s", SH_PROMPT);
 *   if (fgets(cmd_buff, ARG_MAX, stdin) == NULL){
 *     printf("\n");
 *     break;
 *   }
 *   //remove the trailing \n from cmd_buff
 *   cmd_buff[strcspn(cmd_buff,"\n")] = '\0';
 *
 *   //IMPLEMENT THE REST OF THE REQUIREMENTS
 * }
 *
 * Also, use the constants in the dshlib.h in this code.
 *   SH_CMD_MAX          maximum buffer size for user input
 *   EXIT_CMD            constant that terminates the dsh program
 *   SH_PROMPT           the shell prompt
 *   OK                  the command was parsed properly
 *   WARN_NO_CMDS        the user command was empty
 *   ERR_TOO_MANY_COMMANDS too many pipes used
 *   ERR_MEMORY          dynamic memory management failure
 *
 * errors returned
 *   OK                  No error
 *   ERR_MEMORY          Dynamic memory management failure
 *   WARN_NO_CMDS        No commands parsed
 *   ERR_TOO_MANY_COMMANDS too many pipes used
 *
 * console messages
 *   CMD_WARN_NO_CMD     print on WARN_NO_CMDS
 *   CMD_ERR_PIPE_LIMIT  print on ERR_TOO_MANY_COMMANDS
 *   CMD_ERR_EXECUTE     print on execution failure of external command
 *
 * Standard Library Functions You Might Want To Consider Using (assignment 1+)
 *   malloc(), free(), strlen(), fgets(), strcspn(), printf()
 *
 * Standard Library Functions You Might Want To Consider Using (assignment 2+)
 *   fork(), execvp(), exit(), chdir()
 */
int exec_local_cmd_loop()
{
    cmd_buff_t cmd;
    
    alloc_cmd_buff(&cmd);

    while (1) {
        printf(SH_PROMPT);
        fflush(stdout); // Ensure prompt is displayed

        char input[SH_CMD_MAX];
        
        if (fgets(input, sizeof(input), stdin) == NULL) { 
            printf("\n"); 
            break; 
        }

        input[strcspn(input, "\n")] = '\0'; // Remove newline

        clear_cmd_buff(&cmd);

        if (build_cmd_buff(input, &cmd) != OK || cmd.argc == 0) continue;

        if (strcmp(cmd.argv[0], EXIT_CMD) == 0) break;

        exec_cmd(&cmd);
    }

    free_cmd_buff(&cmd);
    
    return OK_EXIT;
}

int alloc_cmd_buff(cmd_buff_t *cmd_buff) {
    cmd_buff->_cmd_buffer = malloc(SH_CMD_MAX);
    if (cmd_buff->_cmd_buffer == NULL) {
        return ERR_MEMORY;
    }
    return OK;
}

int free_cmd_buff(cmd_buff_t *cmd_buff) {
    free(cmd_buff->_cmd_buffer);
    return OK;
}

int clear_cmd_buff(cmd_buff_t *cmd_buff) {
    cmd_buff->argc = 0;
    memset(cmd_buff->argv, 0, sizeof(cmd_buff->argv));
    memset(cmd_buff->_cmd_buffer, 0, SH_CMD_MAX);
    return OK;
}

int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd_buff) {
    char *token;
    int argc = 0;

    // Trim leading and trailing spaces
    while (isspace(*cmd_line)) cmd_line++;
    char *end = cmd_line + strlen(cmd_line) - 1;
    while (end > cmd_line && isspace(*end)) end--;
    *(end + 1) = '\0';

    // Replace multiple spaces with a single space in-place
    char *src = cmd_line, *dst = cmd_line;
    int space_found = 0;

    while (*src != '\0') {
        if (isspace(*src)) {
            if (!space_found) { 
                *dst++ = ' ';
                space_found = 1; 
            }
        } else {
            *dst++ = *src;
            space_found = 0;
        }
        src++;
    }
    *dst = '\0';
    int in_quotes = 0;
    char *start = cmd_line;

    while (*start && argc < CMD_ARGV_MAX-1) {
        if (*start == '"') {
            in_quotes = !in_quotes;
            start++;
            continue;
        }
    
        if (!in_quotes && isspace(*start)) {
            start++;
            continue;
        }

        char *end = start;
        while (*end && (in_quotes || !isspace(*end))) {
            if (*end == '"' && in_quotes) {
                in_quotes = 0;
                end++;
                break;
            }
            end++;
        }
    
    char *token = strndup(start, end-start);
    cmd_buff->argv[argc++] = token;
    start = end;
}

    // Parse tokens
    while (*start != '\0' && argc < CMD_ARGV_MAX - 1) {
        token = start;
        while (!isspace(*start) && *start != '\0') start++;
        if (*start != '\0') *start++ = '\0';

    // the cd command should chdir to the provided directory; if no directory is provided, do nothing
        cmd_buff->argv[argc++] = strdup(token);

    // for example, if the user input is "ls -l", you would fork/exec the command "ls" with the arg "-l"
        while (isspace(*start)) start++; // Skip spaces
    }

    cmd_buff->argv[argc] = NULL;
    cmd_buff->argc = argc;

    return OK;
}

Built_In_Cmds match_command(const char *input) {
    if (strcmp(input, "exit") == 0) return BI_CMD_EXIT;
    if (strcmp(input, "cd") == 0) return BI_CMD_CD;
    return BI_NOT_BI;
}

Built_In_Cmds exec_built_in_cmd(cmd_buff_t *cmd) {
    Built_In_Cmds bi_cmd = match_command(cmd->argv[0]);

    switch (bi_cmd) {
        case BI_CMD_EXIT:
            return BI_CMD_EXIT;

        case BI_CMD_CD:
            if (cmd->argc == 1) {
                char cwd[SH_CMD_MAX];
                if (getcwd(cwd, sizeof(cwd)) != NULL) {
                    if (strncmp(cwd, "/private/", 9) == 0) {
                        printf("%s\n", cwd + 8);
                    } else {
                        printf("%s\n", cwd);
                    }
                }
                return BI_EXECUTED;
            }
            else if (cmd->argc == 2) {
            // Change to specified directory
            if (chdir(cmd->argv[1]) != 0) {
                perror("cd");
                return BI_RC;
            }
            } else {
                fprintf(stderr, "cd: too many arguments\n");
                return BI_RC;
            }

            // Print current directory once
            char cwd[SH_CMD_MAX];
            if (getcwd(cwd, sizeof(cwd)) != NULL) {
                if (strncmp(cwd, "/private/", 9) == 0) {
                    printf("%s\n", cwd + 8);
                } else {
                    printf("%s\n", cwd);
                }
            } else {
                perror("getcwd");
            }
            return BI_EXECUTED;

        default:
            return BI_NOT_BI;
    }
}

int exec_cmd(cmd_buff_t *cmd) {
    Built_In_Cmds bi_cmd = exec_built_in_cmd(cmd);

    if (bi_cmd == BI_EXECUTED || bi_cmd == BI_RC) {
        return OK; // Built-in command executed successfully
    }

    pid_t pid = fork();

    if (pid == 0) { // Child process
        execvp(cmd->argv[0], cmd->argv);
        perror("execvp"); // If execvp fails
        exit(ERR_EXEC_CMD);

    } else if (pid > 0) { // Parent process
        int status;
        waitpid(pid, &status, 0);
        return WIFEXITED(status) ? WEXITSTATUS(status) : ERR_EXEC_CMD;

    } else { // Fork failed
        perror("fork");
        return ERR_EXEC_CMD;
    }
}