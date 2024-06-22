/* @author : Athanasios Giavridis -- sdi2100022 */

#include "jobExecutorServer.h"

bool flag_com = false;
bool flag_child = false;

jobQueue *finished;

void signal_handler(int signum){
    if(signum == SIGUSR2){
        flag_com = true;
        return;
    }
    if(signum == SIGCHLD){
        flag_child = true;
        int cid;
        while((cid = waitpid(-1,NULL,WNOHANG)) > 0){

            jobtuple *tuple = (jobtuple*)(malloc(sizeof(jobtuple)));
            jobtuple_constructor(tuple,NULL,NULL);
            tuple->pid = cid;

            enqueue(finished,tuple);
        }
    }
}

int main(int argc,char **argv){

    struct sigaction act;
    act.sa_handler = signal_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGUSR2, &act, NULL);
    sigaction(SIGCHLD, &act, NULL);

    int fd = open(SERVER_FILE, O_WRONLY | O_CREAT,0666); // server creates its .txt file
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    mkfifo(EX_TO_COM,0666);
    mkfifo(COM_TO_EX,0666);

    int read_pd = open(COM_TO_EX,O_RDONLY | O_NONBLOCK); // pipe descriptor for reading
    int write_pd = open(EX_TO_COM, O_WRONLY | O_NONBLOCK);// pipe descriptor for writing

    pid_t serv_pid = getpid();

    int len_pid = snprintf(NULL, 0, "%d", serv_pid); // calculate the number of digits
    char *pid_str = (char*)malloc((len_pid + 2)*sizeof(char)); // create a buffer to hold the pid string
    if (pid_str == NULL) {

        close(fd);
        unlink(EX_TO_COM);
        unlink(COM_TO_EX);

        printf("Memory Error!\n");
        printf("Allocation Failed!\n");
        return ERROR_CODE;
    }

    snprintf(pid_str,len_pid + 2,"%d%d",len_pid,serv_pid);

    write(fd,pid_str,(len_pid + 2)*sizeof(char)); // pass the pid to the jobExecutorServer.txt


    server *ex_serv = (server*)malloc(sizeof(server));
    ex_serv->concurrency = 1;
    ex_serv->queued = (jobQueue*)malloc(sizeof(jobQueue));
    ex_serv->running = (jobQueue*)malloc(sizeof(jobQueue));
    finished = (jobQueue*)malloc(sizeof(jobQueue));

    jobQueue_constructor(ex_serv->queued);
    jobQueue_constructor(ex_serv->running);
    jobQueue_constructor(finished);

    bool flag_exit = false;

    kill(getppid(),SIGUSR1);

    while(true){
        
        if(flag_com == false && flag_child == false){
            pause();
        }

        if(flag_com == true){

            flag_com = false;
            char *pid_size = (char*)malloc((2)*sizeof(char));
            read(read_pd,pid_size,sizeof(char));
            pid_size[1] = '\0'; // null-terminate the string
            int pid_size_int = atoi(pid_size);
            char *com_pid_str = (char*)malloc((pid_size_int + 1)*sizeof(char));
            read(read_pd,com_pid_str,(pid_size_int)*sizeof(char));
            com_pid_str[pid_size_int] = '\0'; // null-terminate the string
            pid_t com_pid = atoi(com_pid_str);
            

            char *buffer_size = (char*)malloc((2)*sizeof(char));
            read(read_pd,buffer_size,sizeof(char));
            buffer_size[1] = '\0'; // null-terminate the string
            int buffer_size_int = atoi(buffer_size);

            char *buffer = (char*)malloc((buffer_size_int + 2)*sizeof(char));
            read(read_pd,buffer,(buffer_size_int)*sizeof(char));
            buffer[buffer_size_int] = '\0'; // null-terminate the string
            int command_size = atoi(buffer);
            

            char *command_line = (char*)malloc((command_size + 2)*sizeof(char));
            read(read_pd,command_line,(command_size + 1)*sizeof(char));
            command_line[command_size] = '\0'; // null-terminate the string
            

            char *token = strtok(command_line, " "); // tokenize the string using space as delimiter and get the command

            //issueJob ls-l
            
            char *command = strdup(token);
            char *job;
            token = strtok(NULL,"");
            if(token != NULL){
                job = strdup(token);
            }else{
                job = NULL;
            }

            char *message = NULL; // message for the commander;

            if(job != NULL){
                message = handleCommand(ex_serv,command,job);
            }
            
            if(strcmp(command,"exit") == 0){ // check if its the exit command
                char* temp_msg = "jobExecutorServer terminated.\0";
                message = (char*)malloc((strlen(temp_msg) + 2)*sizeof(char));
                strcpy(message,temp_msg);
                flag_exit = true;
            }

            if(message == NULL){
                message = (char*)malloc(sizeof(char));
                message[0] = '\0'; // null-terminate the string
            }

            int length = strlen(message);
            int indicator = snprintf(NULL, 0, "%d", length);
            char *pipe_msg = (char*)malloc((length + indicator + 3) * sizeof(char));
            snprintf(pipe_msg,(length + indicator + 2),"%d%d%s",indicator,length,message);
            pipe_msg[length + indicator + 1] = '\0'; // null-terminate the string

            write_pd = open(EX_TO_COM,O_WRONLY | O_NONBLOCK);
            write(write_pd,pipe_msg,(length + indicator + 1)*sizeof(char));

            if(kill(com_pid,SIGUSR1) == -1){
                perror("kill");
            }
                

            
            if(flag_exit == true){
                break;
            }

        }

        if(flag_child == true){
            flag_child = false;
            clean_queue(ex_serv,finished);
            available_jobs(ex_serv);
        }    
    }

    if (ex_serv->running != NULL) {
        jobQueue_destructor(ex_serv->running);
        free(ex_serv->running);
    }

    if (ex_serv->queued != NULL) {
        jobQueue_destructor(ex_serv->queued);
        free(ex_serv->queued);
    }

    jobQueue_destructor(finished);
    free(finished);
    free(ex_serv);

    close(fd);
    unlink(SERVER_FILE);
    unlink(EX_TO_COM);
    unlink(COM_TO_EX);

    return 0;
}