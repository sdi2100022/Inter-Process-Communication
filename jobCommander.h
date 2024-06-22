/* @author : Athanasios Giavridis -- sdi2100022 */
#include "jobCommands.h"

#define EXEC_SERVER "./jobExecutorServer"


/* Helper Function to check if the command given was a valid command */
int handleCommand(char *command) {

    if (strcmp(command, "issueJob") == 0) { // check if its the issueJob command
        return 0;
    } else if (strcmp(command, "setConcurrency") == 0) { // check if its the setConcurrency command
        return 0;
    } else if (strcmp(command, "stop") == 0) { // check if its the stop command
        return 0;
    } else if (strcmp(command, "poll") == 0) { // check if its the poll command
        return 0;
    } else if (strcmp(command, "exit") == 0) { // check if its the exit command
        return 0;
    }

    printf("Unknown command <%s> \n", command);
    return ERROR_CODE;
}

/* Helper Function to turn the command line arguments of the program 
 * to a string so it can be passed through a pipe.
 * The string will have the form IXX...X<command>
 * where "I" will be a single digit that will indicate the length of the buffer size number,
 * "XX...X" will be a I-length number indicating the buffer size for the rest of the command,
 * "<command>" will be the command that is passed in the arguments of our main
 */
char* concatArguments(int argc, char **argv) {

    unsigned int length = 0; // counting the total lenght of the arguments

    for (int i = 1; i < argc; i++) {
        length += strlen(argv[i]) + 1; // adding one for space or null terminator
    }

    int indicator = snprintf(NULL, 0, "%d", length); //calculating the length of the integer for the buffer indicators
    
    if(indicator >= 10){ //too large of an argument
        printf("Character Limit Surpassed!\n");
        return NULL;
    }

    char *result = (char*)malloc((length + indicator + 1) * sizeof(char)); // allocate memory (lenght is for the arg string,indicator is for the number of the arg string,+1 is for the indicator itself)

    if (result == NULL) {
        printf("Memory Error!\n");
        printf("Allocation Failed!\n");
        return NULL;
    }

    char *first_char = (char*)malloc(2 * sizeof(char)); // allocate memory for a single character and the null character

    if (first_char == NULL) {
        free(result);
        printf("Memory Error!\n");
        printf("Allocation Failed!\n");
        return NULL;
    }

    sprintf(first_char, "%d", indicator); // turn the indicator in to a string

    char *buffer_size = (char*)malloc((indicator + 1) * sizeof(char)); // allocate memory for the buffer size

    if (buffer_size == NULL) {
        free(result);
        free(first_char);
        printf("Memory Error!\n");
        printf("Allocation Failed!\n");
        return NULL;
    }

    snprintf(buffer_size,indicator + 1,"%d",length - 1); // turn the buffer size in to a string 
    
    strcpy(result,first_char); // copy the indicator for the length of the buffer size
    strcat(result,buffer_size); // copy the buffer size
    strcat(result, argv[1]); // copy the first argument

    for (int i = 2; i < argc; i++) {
        strcat(result, " "); // put gaps between arguments
        strcat(result, argv[i]); // copy the next argument
    }

    free(first_char);
    free(buffer_size);

    return result;
}
