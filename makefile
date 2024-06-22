OBJS 	= jobCommander.o jobExecutorServer.o jobCommands.o jobUtil.o
SOURCE	= jobCommander.c jobExecutorServer.c jobCommands.c jobUtil.c progDelay.c multijob.sh allJobsStop.sh
HEADER  = jobCommander.h jobExecutorServer.h jobCommands.h jobUtil.h
OUT  	= jobCommander jobExecutorServer progDelay multijob allJobsStop
CC	= gcc
FLAGS   = -Wall -lrt -g -c 

all: $(OUT)

jobCommander: jobCommander.o jobCommands.o jobUtil.o
	$(CC) -g jobCommander.o jobCommands.o jobUtil.o -o jobCommander

jobCommander.o: jobCommander.c 
	$(CC) $(FLAGS) -c jobCommander.c

jobExecutorServer: jobExecutorServer.o jobCommands.o jobUtil.o
	$(CC) -g jobExecutorServer.o jobCommands.o jobUtil.o -o jobExecutorServer

jobExecutorServer.o: jobExecutorServer.c 
	$(CC) $(FLAGS) -c jobExecutorServer.c

jobCommands.o: jobCommands.c jobUtil.o
	$(CC) $(FLAGS) -c jobCommands.c 

jobUtil.o: jobUtil.c
	$(CC) $(FLAGS) -c jobUtil.c

progDelay: progDelay.c
	gcc progDelay.c -o progDelay

multijob: multijob.sh
	chmod +x multijob.sh

allJobsStop: allJobsStop.sh
	chmod +x allJobsStop.sh

clean:
	rm -f $(OBJS) $(OUT)

count: 
	wc $(SOURCE) $(HEADER)