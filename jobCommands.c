/* @author : Athanasios Giavridis -- sdi2100022 */

#include "jobCommands.h"

char *issueJob(char *job, server *s){

    static int job_count = 1; // count of JobIDs

    char *result = NULL;
    int buffer_size = snprintf(NULL,0,"job_%d",job_count); //get the buffer size of the JobID string
    char* jobID = (char*)malloc((buffer_size + 1)*sizeof(char)); // allocating JobID string
    if(jobID == NULL){
        printf("Memory Error!\n");
        printf("Allocation Failed!\n");
        return result;
    }
    snprintf(jobID,buffer_size + 2,"job_%d",job_count); // get the JobID string

    jobtuple *tuple = (jobtuple*)malloc(sizeof(jobtuple)); // allocating jobtuple
    if(tuple == NULL){
        free(jobID);
        printf("Memory Error!\n");
        printf("Allocation Failed!\n");
        return result;
    }

    jobtuple_constructor(tuple,jobID,job); // allocating and initializing the tuple

    enqueue(s->queued,tuple); // queue the job in the "queued" queue of the server;

    buffer_size = snprintf(NULL,0,"<%s,%s,%d>",tuple->jobID, tuple->job, tuple->queuePosition);  //get the buffer size of the tuple's values
    result = (char*)malloc((buffer_size + 1)*sizeof(char)); // allocating the string
    if(result == NULL){
        free(jobID);
        free(tuple);
        printf("Memory Error!\n");
        printf("Allocation Failed!\n");
        return result;
    }

    snprintf(result,buffer_size + 1,"<%s,%s,%d>",tuple->jobID, tuple->job, tuple->queuePosition);// get the string

    job_count++;

    available_jobs(s); // check if there is available space for the new issued job
    
    return result;
}

void setConcurrency(int N, server *s){
    
    s->concurrency = N;
    available_jobs(s); // check if there is available space for a job
    return;
}

char *stop(char *jobID, server *s){
    
    int size = 0; // size of the result string
    char *result = NULL;
    jobtuple *tuple = NULL; // stored tuple that matches the jobID

    tuple = removeNode(s->queued,jobID,compareByJobID); // search the queued jobQueue
    if(tuple != NULL){ // if found in queued 

        size = snprintf(NULL,0,"%s removed",tuple->jobID);
        result = (char*)malloc((size + 1)*sizeof(char));
        snprintf(result,size + 1,"%s removed",tuple->jobID);

    }else{ // if not found in queued

        tuple = removeNode(s->running,jobID,compareByJobID); // search the running jobQueue
        if(tuple != NULL){ // if found in running 

            kill(tuple->pid,SIGKILL); // terminate the process

            size = snprintf(NULL,0,"%s terminated",tuple->jobID);
            result = (char*)malloc((size + 1)*sizeof(char));
            snprintf(result,size + 1,"%s terminated",tuple->jobID);

        }else{ // if not found in running either

            size = snprintf(NULL,0,"%s not in server",jobID);
            result = (char*)malloc((size + 1)*sizeof(char));
            snprintf(result,size + 1,"%s not in server",jobID);
        }
    }

    if(tuple != NULL){
        jobtuple_destructor(tuple);
    }

    return result;
}

char *poll(char *state, server *s){

    char *result = NULL;
    if(strcmp(state,"running") == 0){
        result = (char*)traverse(s->running,concatenated_string); // get all jobs in running state
    }else if(strcmp(state,"queued") == 0){
        result = (char*)traverse(s->queued,concatenated_string); // get all jobs in queued state
    }

    if(result == NULL || result[0] == '\0'){
        int buffer_size = snprintf(NULL,0,"<>");
        result = (char*)malloc((buffer_size + 1)*sizeof(char));
        snprintf(result,buffer_size + 1,"<>");
    }

    return result;

}

void available_jobs(server *s){

    while(s->running->size < s->concurrency){
        jobtuple *tuple = (jobtuple*)dequeue(s->queued);
        if(tuple == NULL){ // queued jobQueue is empty
            break;
        }

        char *token;
        char *command_copy = strdup(tuple->job);
        if(command_copy == NULL){
            continue;
        }
        int size = 0;

        token = strtok(command_copy," "); // calculate size of the argv list
        while (token != NULL) {
            size++;
            token = strtok(NULL," ");
        }
        free(command_copy);

        char **argv = (char**)malloc((size + 1)*sizeof(char*)); // allocate memory for the argv list
        int argc = 0;

        command_copy = strdup(tuple->job);
        if(command_copy == NULL){
            free(argv);
            continue;
        }

        token = strtok(command_copy," ");
        while (token != NULL) {
            argv[argc] = strdup(token); // create the argv list
            if(argv[argc] == NULL){
                for (int j = 0; j < argc; j++) {
                    free(argv[j]);
                }
                free(argv);
                free(command_copy);
                continue;
            }
            argc++;
            token = strtok(NULL," ");
        }
        argv[argc] = NULL;
        free(command_copy);

        pid_t pid = fork();
        if(pid == -1){
            perror("fork");
            for (int j = 0; j < argc; j++) {
                free(argv[j]);
            }
            free(argv);
        }else if(pid == 0){ // if it is the child process
            
            execvp(argv[0], argv); // execute the command

            perror("execvp");
            exit(EXIT_FAILURE);

        }else{

            tuple->pid = pid; // get pid from child process
            enqueue(s->running,tuple); // insert the job in the running jobQueue
            
            for (int j = 0; j < argc; j++) {
                free(argv[j]);
            }
            free(argv);
        }
    }
    
    return ;
}
