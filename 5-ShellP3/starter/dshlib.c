#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "dshlib.h"

/*
 * Implement your exec_local_cmd_loop function by building a loop that prompts the
 * user for input. Use the SH_PROMPT constant from dshlib.h and then
 * use fgets to accept user input.
 *
 * while(1){
 *   printf("%s", SH_PROMPT);
 *   if (fgets(cmd_buff, ARG_MAX, stdin) == NULL){
 *       printf("\n");
 *       break;
 *   }
 *   //remove the trailing \n from cmd_buff
 *   cmd_buff[strcspn(cmd_buff,"\n")] = '\0';
 *
 *   //IMPLEMENT THE REST OF THE REQUIREMENTS
 * }
 *
 * Also, use the constants in the dshlib.h in this code.
 * SH_CMD_MAX maximum buffer size for user input
 * EXIT_CMD constant that terminates the dsh program
 * SH_PROMPT the shell prompt
 * OK the command was parsed properly
 * WARN_NO_CMDS the user command was empty
 * ERR_TOO_MANY_COMMANDS too many pipes used
 * ERR_MEMORY dynamic memory management failure
 *
 * errors returned
 * OK No error
 * ERR_MEMORY Dynamic memory management failure
 * WARN_NO_CMDS No commands parsed
 * ERR_TOO_MANY_COMMANDS too many pipes used
 *
 * console messages
 * CMD_WARN_NO_CMD print on WARN_NO_CMDS
 * CMD_ERR_PIPE_LIMIT print on ERR_TOO_MANY_COMMANDS
 * CMD_ERR_EXECUTE print on execution failure of external command
 *
 * Standard Library Functions You Might Want To Consider Using (assignment 1+)
 * malloc(), free(), strlen(), fgets(), strcspn(), printf()
 *
 * Standard Library Functions You Might Want To Consider Using (assignment 2+)
 * fork(), execvp(), exit(), chdir()
 */

 int alloc_cmd_buff(cmd_buff_t *cmd_buff) {
    cmd_buff->_cmd_buffer = malloc(SH_CMD_MAX);
    if (cmd_buff->_cmd_buffer == NULL) {
        return ERR_MEMORY;
    } // ADD THIS CLOSING BRACE
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
    int argc = 0;
    char *token = strtok(cmd_line, " \t\n");
    
    while (token != NULL && argc < CMD_ARGV_MAX - 1) {
        cmd_buff->argv[argc++] = token;
        token = strtok(NULL, " \t\n");
    }
    
    cmd_buff->argv[argc] = NULL;
    cmd_buff->argc = argc;
    
    return (argc > 0) ? OK : WARN_NO_CMDS;
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
            if (cmd->argc == 1 || (cmd->argc == 2 && strcmp(cmd->argv[1], "~") == 0)) {
                const char *home_dir = getenv("HOME");
                if (home_dir == NULL) {
                    fprintf(stderr, "cd: HOME not set\n");
                    return BI_NOT_BI;
                }
                if (chdir(home_dir) != 0) {
                    perror("cd");
                    return BI_NOT_BI;
                }
            } else if (cmd->argc == 2) {
                if (chdir(cmd->argv[1]) != 0) {
                    perror("cd");
                    return BI_NOT_BI;
                }
            } else {
                fprintf(stderr, "cd: too many arguments\n");
                return BI_NOT_BI;
            }
            char cwd[SH_CMD_MAX];
            if (getcwd(cwd, sizeof(cwd)) != NULL) {
                printf("%s\n", cwd);
            } else {
                perror("getcwd");
                return BI_NOT_BI;
            }
            return BI_EXECUTED;
        default:
            return BI_NOT_BI;
    }
}

int exec_cmd(cmd_buff_t *cmd) {
    // Handle input redirection
    if (cmd->input_redirect) {
        int fd = open(cmd->input_redirect, O_RDONLY);
        if (fd < 0) {
            perror("open input");
            return ERR_EXEC_CMD;
        }
        dup2(fd, STDIN_FILENO);
        close(fd);
    }

    // Handle output redirection
    if (cmd->output_redirect) {
        int flags = O_WRONLY | O_CREAT | (cmd->append_mode ? O_APPEND : O_TRUNC);
        int fd = open(cmd->output_redirect, flags, 0644);
        if (fd < 0) {
            perror("open output");
            return ERR_EXEC_CMD;
        }
        dup2(fd, STDOUT_FILENO);
        close(fd);
    }

    // Execute the command
    execvp(cmd->argv[0], cmd->argv);
    perror("execvp");
    exit(ERR_EXEC_CMD);
}

int execute_pipeline(command_list_t *clist) {
    int i, status;
    pid_t pid;
    int prev_pipe = -1;
    int pipes[2];

    // Close all pipes first to prevent leaks
    for (i = 0; i < CMD_MAX - 1; i++) {
        close(clist->pipe_fds[i][0]);
        close(clist->pipe_fds[i][1]);
    }

    for (i = 0; i < clist->num; i++) {
        if (i < clist->num - 1) {
            if (pipe(pipes) == -1) {
                perror("pipe");
                return ERR_EXEC_CMD;
            }
            clist->pipe_fds[i][0] = pipes[0];
            clist->pipe_fds[i][1] = pipes[1];
        }

        pid = fork();
        if (pid < 0) {
            perror("fork");
            return ERR_EXEC_CMD;
        } 
        else if (pid == 0) { // Child
            // Input from previous
            if (i > 0) {
                dup2(prev_pipe, STDIN_FILENO);
                close(prev_pipe);
            }
            
            // Output to next
            if (i < clist->num - 1) {
                close(pipes[0]);
                dup2(pipes[1], STDOUT_FILENO);
                close(pipes[1]);
            }
            
            // Execute command
            exec_cmd(&clist->commands[i]);
            exit(EXIT_FAILURE);
        }
        else { // Parent
            // Cleanup previous pipe
            if (i > 0) close(prev_pipe);
            
            // Store current pipe for next iteration
            if (i < clist->num - 1) {
                close(pipes[1]);
                prev_pipe = pipes[0];
            }
            
            // Wait only for the last command
            if (i == clist->num - 1) {
                waitpid(pid, &status, 0);
            }
        }
    }
    return OK;
}

int build_cmd_list(char *cmd_line, command_list_t *clist) {
    char *token;
    char *saveptr;
    int cmd_count = 0;

    token = strtok_r(cmd_line, "|", &saveptr);
    while (token != NULL && cmd_count < CMD_MAX) {
        build_cmd_buff(token, &clist->commands[cmd_count]);
        cmd_count++;
        token = strtok_r(NULL, "|", &saveptr);
    }

    clist->num = cmd_count;
    return (cmd_count > 0) ? OK : WARN_NO_CMDS;
}

int free_cmd_list(command_list_t *cmd_lst){
    for (int i = 0; i < cmd_lst->num; i++){
        free_cmd_buff(&cmd_lst->commands[i]);
    }
    return OK;
}

int exec_local_cmd_loop() {
    command_list_t clist;
    char input[SH_CMD_MAX];
    while (1) {
        printf("%s", SH_PROMPT);
        fflush(stdout);
        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("\n");
            break;
        }
        input[strcspn(input, "\n")] = '\0';

        int build_result = build_cmd_list(input, &clist);
        if (build_result != OK || clist.num == 0) {
            if (build_result == WARN_NO_CMDS) {
                continue;
            }
            printf("%s\n", CMD_WARN_NO_CMD);
            continue;
        }

        if (strcmp(clist.commands[0].argv[0], EXIT_CMD) == 0) {
            printf("exiting...\n");
            break;
        }

        int exec_result = execute_pipeline(&clist);
        if (exec_result != OK) {
            printf("%s\n", CMD_ERR_EXECUTE);
        }
    }
    return OK;
}