#!/bin/bash


#References:
#https://ryanstutorials.net/bash-scripting-tutorial/bash-if-statements.php
#https://ryanstutorials.net/bash-scripting-tutorial/bash-variables.php
#lecture content


# Check if the number of arguments is correct
if [ "$#" -ne 2 ]
then
    echo "Error: Provide the input arguments file path and string to search"
    exit 1
fi

# Assign arguments to variables
filesdir="$1"
searchstring="$2"

# Check if filesdir exists and is a directory
if [ ! -d "$filesdir" ] 
then
    echo "Error:Directory does not exist"
    exit 1
fi

# Find matching lines in files and count them
#The line num_files was fully generated using ChatGPT at https://chat.openai.com/ with prompts including How to find files present in directories and subdirectoies in bash provided a path and get the count
num_files=$(find "$filesdir" -type f | wc -l)
#The line num_matching_lines was fully generated using ChatGPT at https://chat.openai.com/ with prompts including Find the count of an input string in the file directory?
num_matching_lines=$(grep -r "$searchstring" "$filesdir" | wc -l)

# Print the result
echo "The number of files are $num_files and the number of matching lines are $num_matching_lines"

