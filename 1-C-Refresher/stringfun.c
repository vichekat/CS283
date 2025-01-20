#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUFFER_SZ 50

// Prototypes
void usage(char *);
void print_buff(char *, int);
int setup_buff(char *, char *, int);

// Prototypes for functions to handle required functionality
int count_words(char *, int, int);
void reverse_string(char *, int);
void word_print(char *, int);
void search_replace(char *, int, char *, char *);

int setup_buff(char *buff, char *user_str, int len) {
    int i = 0, j = 0;
    char prev = ' ';

    while (user_str[i] != '\0' && j < len) {
        if (user_str[i] != ' ' && user_str[i] != '\t') {
            buff[j++] = user_str[i];
            prev = user_str[i];
        } else if (prev != ' ') {
            buff[j++] = ' ';
            prev = ' ';
        }
        i++;
    }

    if (user_str[i] != '\0') return -1; // String too large

    // Fill remainder with dots
    while (j < len) {
        buff[j++] = '.';
    }

    return i; // Return length of user string
}

void print_buff(char *buff, int len) {
    printf("Buffer: ");
    for (int i = 0; i < len; i++) {
        putchar(*(buff + i));
    }
    putchar('\n');
}

void usage(char *exename) {
    printf("usage: %s [-h|c|r|w|x] \"string\" [other args]\n", exename);
}

int count_words(char *buff, int len, int str_len) {
    int count = 0;
    int in_word = 0;

    for (int i = 0; i < str_len; i++) {
        if (buff[i] != ' ' && !in_word) {
            in_word = 1;
            count++;
        } else if (buff[i] == ' ') {
            in_word = 0;
        }
    }

    return count;
}

void reverse_string(char *buff, int str_len) {
    char temp;
    for (int i = 0; i < str_len / 2; i++) {
        temp = buff[i];
        buff[i] = buff[str_len - 1 - i];
        buff[str_len - 1 - i] = temp;
    }
}

void word_print(char *buff, int str_len) {
    int word_start = 0;
    int word_count = 0;

    printf("Word Print\n----------\n");

    for (int i = 0; i <= str_len; i++) {
        if (buff[i] == ' ' || buff[i] == '.' || i == str_len) {
            if (i > word_start) {
                word_count++;
                printf("%d. ", word_count);
                for (int j = word_start; j < i; j++) {
                    putchar(buff[j]);
                }
                printf(" (%d)\n", i - word_start);
            }
            word_start = i + 1;
        }
    }
}

void search_replace(char *buff, int str_len, char *search, char *replace) {
    int search_len = 0;
    int replace_len = 0;
    int i, j, k;

    while (search[search_len] != '\0') search_len++;
    while (replace[replace_len] != '\0') replace_len++;

    for (i = 0; i <= str_len - search_len; i++) {
        for (j = 0; j < search_len; j++) {
            if (buff[i + j] != search[j]) break;
        }
        if (j == search_len) {
            // Found a match
            if (str_len - search_len + replace_len > BUFFER_SZ) {
                printf("Error: Buffer overflow\n");
                return;
            }
            // Shift the rest of the string
            for (k = str_len - 1; k >= i + search_len; k--) {
                buff[k + replace_len - search_len] = buff[k];
            }
            // Insert the replacement
            for (k = 0; k < replace_len; k++) {
                buff[i + k] = replace[k];
            }
            return; // Replace only the first occurrence
        }
    }
    printf("Search string not found\n");
}

int main(int argc, char *argv[]) {
    char *buff;
    char *input_string;
    char opt;
    int rc;
    int user_str_len;

    // TODO: #1. This is safe because the if statement checks if argc < 2,
    // which ensures that argv[1] exists before attempting to access it.

    if ((argc < 2) || (*argv[1] != '-')) {
        usage(argv[0]);
        exit(1);
    }

    opt = (char)*(argv[1] + 1);

    if (opt == 'h') {
        usage(argv[0]);
        exit(0);
    }

    // TODO: #2. This if statement ensures that there are at least 3 arguments
    // provided (program name, option, and input string) for all options except 'h'.

    if (argc < 3) {
        usage(argv[0]);
        exit(1);
    }

    input_string = argv[2];

    // TODO: #3. Allocate space for the buffer
    buff = (char *)malloc(BUFFER_SZ * sizeof(char));
    if (buff == NULL) {
        printf("Error: Memory allocation failed\n");
        exit(99);
    }

    user_str_len = setup_buff(buff, input_string, BUFFER_SZ);
    if (user_str_len < 0) {
        printf("Error setting up buffer, error = %d\n", user_str_len);
        free(buff);
        exit(2);
    }

    switch (opt) {
        case 'c':
            rc = count_words(buff, BUFFER_SZ, user_str_len);
            if (rc < 0) {
                printf("Error counting words, rc = %d\n", rc);
                free(buff);
                exit(2);
            }
            printf("Word Count: %d\n", rc);
            break;

        case 'r':
            reverse_string(buff, user_str_len);
            printf("Reversed String: ");
            for (int i = 0; i < user_str_len; i++) {
                putchar(buff[i]);
            }
            printf("\n");
            break;

        case 'w':
            word_print(buff, user_str_len);
            break;

        case 'x':
            if (argc < 5) {
                printf("Error: Not enough arguments for -x option\n");
                free(buff);
                exit(1);
            }
            search_replace(buff, user_str_len, argv[3], argv[4]);
            printf("Modified String: ");
            for (int i = 0; i < BUFFER_SZ && buff[i] != '.'; i++) {
                putchar(buff[i]);
            }
            printf("\n");
            break;

        default:
            usage(argv[0]);
            free(buff);
            exit(1);
    }

    print_buff(buff, BUFFER_SZ);
    free(buff);
    exit(0);
}

// TODO: #7. Providing both the pointer and the length is a good practice because:
// 1. It prevents buffer overflows by explicitly stating the buffer size.
// 2. It allows functions to work with buffers of different sizes.
// 3. It makes the code more robust and less prone to errors.
// 4. It improves readability and maintainability of the code.
