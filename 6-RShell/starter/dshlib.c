#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "dshlib.h"



#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "dshlib.h"

/**** 
 **** FOR REMOTE SHELL USE YOUR SOLUTION FROM SHELL PART 3 HERE
 **** THE MAIN FUNCTION CALLS THIS ONE AS ITS ENTRY POINT TO
 **** EXECUTE THE SHELL LOCALLY
 ****
 */

/*
 * Implement your exec_local_cmd_loop function by building a loop that prompts the 
 * user for input.  Use the SH_PROMPT constant from dshlib.h and then
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
 */

 int alloc_cmd_buff(cmd_buff_t *cmd_buff) {
    cmd_buff->_cmd_buffer = malloc(SH_CMD_MAX);
    if (cmd_buff->_cmd_buffer == NULL) {
        return ERR_MEMORY;
    }
    cmd_buff->argc = 0;
    memset(cmd_buff->argv, 0, sizeof(cmd_buff->argv));
    cmd_buff->input_file = NULL;
    cmd_buff->output_file = NULL;
    cmd_buff->append_mode = 0;
    cmd_buff->input_fd = STDIN_FILENO;
    cmd_buff->output_fd = STDOUT_FILENO;
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
    cmd_buff->input_file = NULL;
    cmd_buff->output_file = NULL;
    cmd_buff->append_mode = 0;
    cmd_buff->input_fd = STDIN_FILENO;
    cmd_buff->output_fd = STDOUT_FILENO;
    return OK;
}

int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd_buff) {
    strncpy(cmd_buff->_cmd_buffer, cmd_line, SH_CMD_MAX - 1);
    cmd_buff->_cmd_buffer[SH_CMD_MAX - 1] = '\0';
    int argc = 0;
    
    // We need a more robust tokenization approach for handling quoted arguments
    char *p = cmd_buff->_cmd_buffer;
    char *start = p;
    int in_quotes = 0;
    char quote_char = 0;
    
    while (*p && argc < CMD_ARGV_MAX - 1) {
        // Skip leading whitespace if not in quotes
        if (!in_quotes && (*p == ' ' || *p == '\t')) {
            *p = '\0';
            if (p > start && *(p-1) != '\0') {
                cmd_buff->argv[argc++] = start;
            }
            p++;
            start = p;
            continue;
        }
        
        // Handle quotes
        if (*p == '\'' || *p == '"') {
            if (!in_quotes) {
                // Start of quoted section
                in_quotes = 1;
                quote_char = *p;
                start = p + 1;  // Skip the opening quote
            } else if (*p == quote_char) {
                // End of quoted section
                *p = '\0';  // Replace closing quote with null terminator
                cmd_buff->argv[argc++] = start;
                in_quotes = 0;
                start = p + 1;
            }
        }
        
        p++;
    }
    
    // Add the last argument if there is one
    if (p > start && *start != '\0') {
        cmd_buff->argv[argc++] = start;
    }
    
    cmd_buff->argv[argc] = NULL;
    cmd_buff->argc = argc;
    return (argc > 0) ? OK : WARN_NO_CMDS;
}

Built_In_Cmds match_command(const char *input) {
    if (strcmp(input, EXIT_CMD) == 0) return BI_CMD_EXIT;
    if (strcmp(input, "cd") == 0) return BI_CMD_CD;
    return BI_NOT_BI;
}

Built_In_Cmds exec_built_in_cmd(cmd_buff_t *cmd) {
    Built_In_Cmds bi_cmd = match_command(cmd->argv[0]);
    switch (bi_cmd) {
        case BI_CMD_EXIT:
            printf("exiting...\n");
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
            return BI_EXECUTED;
        default:
            return BI_NOT_BI;
    }
}

int exec_cmd(cmd_buff_t *cmd) {
    if (cmd->input_file) {
        cmd->input_fd = open(cmd->input_file, O_RDONLY);
        if (cmd->input_fd < 0) {
            perror("open input");
            return ERR_EXEC_CMD;
        }
        dup2(cmd->input_fd, STDIN_FILENO);
        close(cmd->input_fd);
    }
    if (cmd->output_file) {
        int flags = O_WRONLY | O_CREAT | (cmd->append_mode ? O_APPEND : O_TRUNC);
        cmd->output_fd = open(cmd->output_file, flags, 0644);
        if (cmd->output_fd < 0) {
            perror("open output");
            return ERR_EXEC_CMD;
        }
        dup2(cmd->output_fd, STDOUT_FILENO);
        close(cmd->output_fd);
    }
    execvp(cmd->argv[0], cmd->argv);
    perror("execvp");
    exit(ERR_EXEC_CMD);
}

int execute_pipeline_commands(command_list_t *cmd_list) {
    int pipefds[2 * (CMD_MAX - 1)];
    pid_t pids[CMD_MAX];
    int i;

    // Check for empty pipeline segments explicitly
    for (i = 0; i < cmd_list->num; i++) {
        if (cmd_list->commands[i].argc == 0 || cmd_list->commands[i].argv[0] == NULL) {
            fprintf(stderr, "error: empty pipeline segment\n");
            return ERR_CMD_ARGS_BAD;
        }
    }

    // Create pipes
    for (i = 0; i < cmd_list->num - 1; i++) {
        if (pipe(pipefds + i * 2) < 0) {
            perror("pipe failed");
            return ERR_EXEC_CMD;
        }
    }

    // Execute commands
    for (i = 0; i < cmd_list->num; i++) {
        Built_In_Cmds bi_cmd = match_command(cmd_list->commands[i].argv[0]);

        // Handle built-in commands only if single command without pipes
        if (bi_cmd != BI_NOT_BI && cmd_list->num == 1) {
            Built_In_Cmds res = exec_built_in_cmd(&cmd_list->commands[i]);
            return (res == BI_EXECUTED || res == BI_CMD_EXIT) ? OK : ERR_EXEC_CMD;
        }

        pids[i] = fork();
        
        if (pids[i] < 0) {
            perror("fork failed");
            return ERR_EXEC_CMD;
        }

        if (pids[i] == 0) { // Child process
            // Redirect input from previous command's pipe
            if (i > 0) {
                dup2(pipefds[(i - 1) * 2], STDIN_FILENO);
            }
            
            // Redirect output to next command's pipe
            if (i < cmd_list->num - 1) {
                dup2(pipefds[i * 2 + 1], STDOUT_FILENO);
            }

            // Close all pipe fds in child process
            for (int j = 0; j < 2 * (cmd_list->num - 1); j++)
                close(pipefds[j]);

            execvp(cmd_list->commands[i].argv[0], cmd_list->commands[i].argv);
            
            // If execvp fails:
            fprintf(stderr, "execvp: %s: command not found\n", cmd_list->commands[i].argv[0]);
            exit(ERR_EXEC_CMD);
        }
    }

    // Close all pipe fds in parent process
    for (i = 0; i < 2 * (cmd_list->num - 1); i++)
        close(pipefds[i]);

    int status;
    int final_status = OK;

    for (i = 0; i < cmd_list->num; i++) {
        waitpid(pids[i], &status, 0);
        
        if (!WIFEXITED(status) || WEXITSTATUS(status) != OK)
            final_status = ERR_EXEC_CMD;
    }

    return final_status;
}

int build_cmd_list(char *cmd_line, command_list_t *clist) {
    char *token;
    char *saveptr;
    int cmd_count = 0;

    token = strtok_r(cmd_line, PIPE_STRING, &saveptr);
    
    while (token != NULL) {
        
        while (*token == SPACE_CHAR)
            token++;

        char *end = token + strlen(token) - 1;
        
        while (end > token && *end == SPACE_CHAR)
            *end-- = '\0';

        if(strlen(token)==0){
           fprintf(stderr,"error: empty pipeline segment\n");
           return ERR_CMD_ARGS_BAD;
        }

        if (cmd_count >= CMD_MAX) {
           fprintf(stderr,CMD_ERR_PIPE_LIMIT,CMD_MAX);
           return ERR_TOO_MANY_COMMANDS;
        }

        alloc_cmd_buff(&clist->commands[cmd_count]);
        
        build_cmd_buff(token, &clist->commands[cmd_count]);
        
        cmd_count++;

        token = strtok_r(NULL, PIPE_STRING, &saveptr);
    }

    clist->num = cmd_count;

    return (cmd_count > 0)? OK : WARN_NO_CMDS;
}

int free_cmd_list(command_list_t *cmd_lst) {
    for (int i = 0; i < cmd_lst->num; i++) {
        free_cmd_buff(&cmd_lst->commands[i]);
    }
    return OK;
}

int exec_local_cmd_loop(void) {
    char line[SH_CMD_MAX];
    command_list_t cmd_list = {0};
    int is_interactive = isatty(STDIN_FILENO);

    while (1) {
        if (is_interactive) {
            printf("%s", SH_PROMPT);
            fflush(stdout);
        }

        if (!fgets(line, SH_CMD_MAX, stdin)) {
            if (!feof(stdin)) {
                perror("fgets failed");
                return ERR_MEMORY;
            }
            break; // EOF reached
        }

        line[strcspn(line, "\n")] = '\0';

        if (is_interactive && strcmp(line, EXIT_CMD) == 0) {
            printf("exiting...\n");
            break;
        }

        if (strlen(line) == 0)
            continue;

        int parse_result = build_cmd_list(line, &cmd_list);
        if (parse_result == WARN_NO_CMDS)
            continue;

        if (parse_result == ERR_TOO_MANY_COMMANDS) {
            fprintf(stderr, CMD_ERR_PIPE_LIMIT, CMD_MAX);
            free_cmd_list(&cmd_list);
            continue;
        } else if (parse_result != OK) {
            fprintf(stderr, "error: command parsing failed\n");
            continue;
        }

        int result = execute_pipeline_commands(&cmd_list);

        free_cmd_list(&cmd_list);
        cmd_list.num = 0;

        if (result == BI_CMD_EXIT)
            break;
    }

    return OK;
}
