/* @author : Athanasios Giavridis -- sdi2100022 */

#include "jobCommands.h"

char* handleCommand(server *s,char *command,char *job){
    char* message = NULL;

    if (strcmp(command, "issueJob") == 0) { // check if its the issueJob command
        message = issueJob(job,s);
    } else if (strcmp(command, "setConcurrency") == 0) { // check if its the setConcurrency command
        int N = atoi(job);
        setConcurrency(N,s);
    } else if (strcmp(command, "stop") == 0) { // check if its the stop command
        message = stop(job,s);
    } else if (strcmp(command, "poll") == 0) { // check if its the poll command
        message = poll(job,s);
    }
    
    return message;
}

void clean_queue(server *s,jobQueue *f){
    while(f->size > 0){

        jobtuple *tuple = (jobtuple*)dequeue(f);
        if(tuple == NULL){
            break;
        }

        pid_t search_pid = tuple->pid;

        jobtuple *terminated = (jobtuple*)removeNode(s->running,&search_pid,compareByPID);
        if(terminated != NULL){
            jobtuple_destructor(terminated);
        }
        jobtuple_destructor(tuple);
    }
}