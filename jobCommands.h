/* @author : Athanasios Giavridis -- sdi2100022 */

/* The Header file of the JobCommands */
/* This file contains the definitions of the commands used by the application*/

#include "jobUtil.h"

#define ERROR_CODE -1

#define SERVER_FILE "jobExecutorServer.txt"
#define COM_TO_EX "jobCommander_to_jobExecutorServer_pipe.fifo"
#define EX_TO_COM "jobExecutorServer_to_Commander_pipe.fifo"

/* This is the struct of the server */
typedef struct server{
    jobQueue *running;
    jobQueue *queued;
    int concurrency;
}server;


/* Insert a new job in the queued jobQueue of the server and check if there is available space so it can be executed */
char *issueJob(char *job,server *s);

/* Change the concurrency of the server */
void setConcurrency(int N,server *s);

/* Remove/Terminate the job with job_XX jobID from the server*/
char *stop(char *jobID,server *s);

/* Get all the tuples that are either in the running jobQueue or the queued jobQueue of the server */
char *poll(char *state,server *s);

/* Check if there is space in the running jobQueue of the server and insert new jobs from the queued jobQueue*/
void available_jobs(server *s);

