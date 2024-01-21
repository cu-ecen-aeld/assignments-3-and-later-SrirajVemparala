#!/bin/bash


#References:
#https://ryanstutorials.net/bash-scripting-tutorial/bash-if-statements.php
#https://ryanstutorials.net/bash-scripting-tutorial/bash-variables.php
#lecture content


# Check if two arguments are added
if [ "$#" -ne 2 ]
then
    echo "Error: Please provide write file and data to write"
    exit 1
fi

# Store arguments
filedir="$1"
writestring="$2"

# Check if writefile and writestr are not empty
if [ -z "$filedir" ] || [ -z "$writestring" ] 
then
    echo "Error:writefile or writestr empty"
    exit 1
fi

#Obtain directory path
#The code block dirname was fully generated using ChatGPT at https://chat.openai.com/ with prompts including How to obtain directory name from a string input in bash?
path=$(dirname "$filedir")
echo "$path"
# Create the directory path if it doesn't exist
mkdir -p "$path"


# Write content to the file, overwriting any existing file
echo "$writestring" > "$filedir"

# Check if the folder was created successfully
if [ $? -ne 0 ] 
then
    echo "Error: Could not create or write to $writefile"
    exit 1
fi

echo "File $filedir created successfully with content: $writestring"

