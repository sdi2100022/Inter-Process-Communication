#!/bin/bash

if [ $# -eq 0 ]; then # if no files given, error
    echo "Please provide files"
    exit 1
fi

for file in "$@" #for every file
do
    if [ -f "$file" ]; then # if file is valid
        while IFS= read -r line || [ -n "$line" ]; do # read every line
            ./jobCommander issueJob $line # run issueJob with line
        done < "$file"
    else 
        echo "File '$file' not found"
    fi
done
./jobCommander exit # terminate jobExecutorServer