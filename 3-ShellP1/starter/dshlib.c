#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "dshlib.h"

/*
 *  build_cmd_list
 *    cmd_line:     the command line from the user
 *    clist *:      pointer to clist structure to be populated
 *
 *  This function builds the command_list_t structure passed by the caller
 *  It does this by first splitting the cmd_line into commands by spltting
 *  the string based on any pipe characters '|'.  It then traverses each
 *  command.  For each command (a substring of cmd_line), it then parses
 *  that command by taking the first token as the executable name, and
 *  then the remaining tokens as the arguments.
 *
 *  NOTE your implementation should be able to handle properly removing
 *  leading and trailing spaces!
 *
 *  errors returned:
 *
 *    OK:                      No Error
 *    ERR_TOO_MANY_COMMANDS:   There is a limit of CMD_MAX (see dshlib.h)
 *                             commands.
 *    ERR_CMD_OR_ARGS_TOO_BIG: One of the commands provided by the user
 *                             was larger than allowed, either the
 *                             executable name, or the arg string.
 *
 *  Standard Library Functions You Might Want To Consider Using
 *      memset(), strcmp(), strcpy(), strtok(), strlen(), strchr()
 */
int build_cmd_list(char *cmd_line, command_list_t *clist)
{
    char *token;
    char *saveptr;
    int cmd_count = 0;
    char *cmd_copy = strdup(cmd_line);

    // Initialize clist
    memset(clist, 0, sizeof(command_list_t));

    // Split command line by pipe character
    token = strtok_r(cmd_copy, PIPE_STRING, &saveptr);
    while (token != NULL && cmd_count < CMD_MAX)
    {
        char *cmd = token;
        char *args = strchr(cmd, SPACE_CHAR);

        // Trim leading spaces
        while (*cmd == SPACE_CHAR) cmd++;

        if (args != NULL)
        {
            *args = '\0';
            args++;
            // Trim leading spaces in args
            while (*args == SPACE_CHAR) args++;
        }

        // Check if command is not empty
        if (strlen(cmd) > 0)
        {
            strncpy(clist->commands[cmd_count].exe, cmd, EXE_MAX - 1);
            clist->commands[cmd_count].exe[EXE_MAX - 1] = '\0';

            if (args != NULL)
            {
                strncpy(clist->commands[cmd_count].args, args, ARG_MAX - 1);
                clist->commands[cmd_count].args[ARG_MAX - 1] = '\0';
            }

            cmd_count++;
        }

        token = strtok_r(NULL, PIPE_STRING, &saveptr);
    }

    free(cmd_copy);

    clist->num = cmd_count;

    if (cmd_count == 0)
    {
        return WARN_NO_CMDS;
    }
    else if (cmd_count > CMD_MAX)
    {
        return ERR_TOO_MANY_COMMANDS;
    }

    return OK;
}