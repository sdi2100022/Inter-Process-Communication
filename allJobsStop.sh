#!/bin/bash


./jobCommander poll queued > queued.txt # run poll command with queued and save it inside a file
./jobCommander poll running > running.txt # run poll command with running and save it inside a file


if [ -f "queued.txt" ] && [ -f "running.txt" ]; then #if both files are valid
    
    while IFS= read -r line || [ -n "$line" ]; do # for all lines of file 1

        job=$(echo "$line" | sed -n 's/^<\([^,]*\),.*>$/\1/p') # get jobID from tuple in line
        if [ -n "$job" ]; then # check if jobID is not empty
            ./jobCommander stop "$job" # stop job
        fi

    done < "queued.txt" # done for queued jobs 
    rm "queued.txt"

    while IFS= read -r line || [ -n "$line" ]; do # for all lines of file 2
    
        job=$(echo "$line" | sed -n 's/^<\([^,]*\),.*>$/\1/p') # get jobID from tuple in line
        if [ -n "$job" ]; then # check if jobID is not empty
            ./jobCommander stop "$job" # stop job
        fi

    done < "running.txt" # done for running jobs 
    rm "running.txt"

else
    echo "File(s) not found"
fi
./jobCommander exit #terminate jobExecutorServer