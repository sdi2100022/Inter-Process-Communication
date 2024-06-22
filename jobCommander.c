/* @author : Athanasios Giavridis -- sdi2100022 */

#include "jobCommander.h"

void signal_handler(int signum){
    if(signum == SIGUSR1){
        return;
    }
}

int main(int argc,char** argv){

    struct sigaction act;
    act.sa_handler = signal_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGUSR1, &act, NULL);

    int fd; // the server's file descriptor

    if(argc < 2){ //check the count of the given arguments if its enough
        printf("Incorrect Syntax!\n");
        printf("Correct Syntax: %s <command>\n",argv[0]);
        return ERROR_CODE;
    }

    if(handleCommand(argv[1]) == -1){ //if handleCommand failed,not valid command
        return ERROR_CODE; 
    }

    char *pipe_arg = concatArguments(argc,argv); //turn the arguments of the command line to a string
    if(pipe_arg == NULL){ // if concatArguments failed,it returns NULL
        return ERROR_CODE;
    }

    fd = open(SERVER_FILE, O_RDONLY); // open the server's file

    if(fd == -1){ //call the server and create it
        pid_t pid = fork();
        if(pid == -1){
            printf("Fork Failed!\n");
            return ERROR_CODE;
        }else if(pid == 0){ // child process creates the server
            execl("./jobExecutorServer", "./jobExecutorServer", (char *)NULL);
            printf("Execl Failed!\n");
            return ERROR_CODE;
        }else{
            pause(); // sleep just for one second so the pipe are open
        }
    }

    

    int read_pd = open(EX_TO_COM,O_RDONLY | O_NONBLOCK); // pipe descriptor for reading
    int write_pd = open(COM_TO_EX, O_WRONLY | O_NONBLOCK);// pipe descriptor for writing

    fd = open(SERVER_FILE, O_RDONLY);

    char *buffer_size = (char*)malloc((2)*sizeof(char));
    read(fd,buffer_size,sizeof(char));
    buffer_size[1] = '\0'; // null-terminate the string
    int buffer_size_int = atoi(buffer_size);
    char *buffer = (char*)malloc((buffer_size_int + 1)*sizeof(char));
    read(fd,buffer,(buffer_size_int + 1)*sizeof(char));
    buffer[buffer_size_int] = '\0'; // null-terminate the string
    pid_t serv_pid = (pid_t)atoi(buffer);
    free(buffer_size);
    free(buffer);
    
    pid_t com_pid = getpid();
    int pid_size = snprintf(NULL,0,"%d",com_pid);

    char *first_write = (char*)malloc((strlen(pipe_arg) + pid_size + 3)*sizeof(char));
    snprintf(first_write,(strlen(pipe_arg) + pid_size + 2),"%d%d%s",pid_size,com_pid,pipe_arg);
    first_write[strlen(pipe_arg) + pid_size + 1] = '\0'; // null-terminate the string

    int write_result = write(write_pd,first_write,(strlen(first_write))*sizeof(char));
    if(write_result == -1){
        perror("open COM_TO_EX for writing");
    }
    free(pipe_arg);
    free(first_write);

    
    
    int kill_result = kill(serv_pid,SIGUSR2);
    if (kill_result == -1) {
        perror("kill");
    }

    pause();
     
    char *msg_buffer_size = (char*)malloc(2 * sizeof(char)); // allocate space for one digit and a null terminator
    read(read_pd, msg_buffer_size, 1 * sizeof(char)); // read one character for the size
    msg_buffer_size[1] = '\0'; // null-terminate the string

    int msg_buffer_size_int = atoi(msg_buffer_size);
    char *msg_buffer = (char*)malloc((msg_buffer_size_int + 1) * sizeof(char)); // allocate space for the message buffer and null terminator
    read(read_pd, msg_buffer, msg_buffer_size_int * sizeof(char)); // read the message
    msg_buffer[msg_buffer_size_int] = '\0'; // null-terminate the message

    int msg_size = atoi(msg_buffer); // convert the message size to an integer

    char *message = (char*)malloc((msg_size + 1) * sizeof(char)); // allocate space for the message and null terminator
    read(read_pd, message, msg_size * sizeof(char)); 
    message[msg_size] = '\0'; // null-terminate the message


    if(strlen(message) != 0){
        printf("%s\n",message);
    }

    free(msg_buffer_size);
    free(msg_buffer);
    free(message);

    return 0;
}