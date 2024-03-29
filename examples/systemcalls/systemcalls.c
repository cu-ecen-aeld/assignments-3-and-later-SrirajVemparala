/*******************************************************************************
 * systemcalls.c
 * Date:        04-02-2024
 * Author:      Raghu Sai Phani Sriraj Vemparala, raghu.vemparala@colorado.edu
 * Description: This file has data related to writer script witten in C
 * References: Linux System Programming Textbook
 *
 *
 ******************************************************************************/
#include "systemcalls.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <syslog.h>
#include <libgen.h>
#include <stdio.h>
/**
 * @param cmd the command to execute with system()
 * @return true if the command in @param cmd was executed
 *   successfully using the system() call, false if an error occurred,
 *   either in invocation of the system() call, or if a non-zero return
 *   value was returned by the command issued in @param cmd.
*/
bool do_system(const char *cmd)
{

/*
 * TODO  add your code here
 *  Call the system() function with the command set in the cmd
 *   and return a boolean true if the system() call completed with success
 *   or false() if it returned a failure
*/
    int ret_val;
    ret_val = system(cmd);
	if(ret_val == -1)
	{
	  return false;
	}
    return true;
}

/**
* @param count -The numbers of variables passed to the function. The variables are command to execute.
*   followed by arguments to pass to the command
*   Since exec() does not perform path expansion, the command to execute needs
*   to be an absolute path.
* @param ... - A list of 1 or more arguments after the @param count argument.
*   The first is always the full path to the command to execute with execv()
*   The remaining arguments are a list of arguments to pass to the command in execv()
* @return true if the command @param ... with arguments @param arguments were executed successfully
*   using the execv() call, false if an error occurred, either in invocation of the
*   fork, waitpid, or execv() command, or if a non-zero return value was returned
*   by the command issued in @param arguments with the specified arguments.
*/

bool do_exec(int count, ...)
{
    va_list args;
    va_start(args, count);
    char * command[count+1];//count+1 because the last value should be NULL
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;
    // this line is to avoid a compile warning before your implementation is complete
    // and may be removed
	command[count] = command[count];
/*
 * TODO:
 *   Execute a system command by calling fork, execv(),
 *   and wait instead of system (see LSP page 161).
 *   Use the command[0] as the full path to the command to execute
 *   (first argument to execv), and use the remaining arguments
 *   as second argument to the execv() command.
 *
*/
// Reference taken from Linux System Programming Textbook
	pid_t pid = fork();//Create child process
	//printf("ForkPID:%d\n",pid);
	int wait_status;
	int ret_exec = 0;
	if(pid == -1)
	{
	 printf("Child process is not created sucessfully");
	 return false;
	}
	else if(!pid)
	{
	 //Ran by child process
	 ret_exec = execv(command[0],command);
	 if(ret_exec == -1)
	 {
	  perror("Execv failed");
	  //printf("Execv failed\n");
     	  exit(EXIT_FAILURE);
	 } 
	}
	else
	{
	 //printf("BEPIDis: %d\n",pid);
	 pid = wait(&wait_status);
	 //printf("AFPIDis: %d\n",pid);
        if (WIFEXITED(wait_status) == 1)//Check if exit call is called by the child process
        {
         	if(WEXITSTATUS(wait_status) == 0)//Check the value of the exit status. 
         	{
            		printf("Command executed successfully\n");
            	}
         	else {
         		perror("Command_exec_failed");
            		//printf("Command execution failed\n");
            		return false;
        	}
        }
	 
	}
    va_end(args);

    return true;
}

/**
* @param outputfile - The full path to the file to write with command output.
*   This file will be closed at completion of the function call.
* All other parameters, see do_exec above
*/
bool do_exec_redirect(const char *outputfile, int count, ...)
{
    va_list args;
    va_start(args, count);
    char * command[count+1];//count+1 because the last value should be NULL
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;
    // this line is to avoid a compile warning before your implementation is complete
    // and may be removed
    command[count] = command[count];


/*
 * TODO
 *   Call execv, but first using https://stackoverflow.com/a/13784315/1446624 as a refernce,
 *   redirect standard out to a file specified by outputfile.
 *   The rest of the behaviour is same as do_exec()
 *
*/
pid_t pid;
int ret_exec;
int wait_status;
int fd = open(outputfile, O_WRONLY|O_TRUNC|O_CREAT, 0644);
if (fd < 0) { perror("open"); abort(); }
switch (pid = fork()) {
  case -1: perror("fork"); abort();
  case 0:
    if (dup2(fd, 1) < 0) { perror("dup2"); abort(); }
    close(fd);
    ret_exec = execv(command[0],command);
    if(ret_exec == -1)
    {
    	  perror("Execv failed");
	  //printf("Execv failed\n");
     	  exit(EXIT_FAILURE);
    } 
  default:
    close(fd);
    pid = wait(&wait_status);
	 //printf("AFPIDis: %d\n",pid);
        if (WIFEXITED(wait_status) == 1)
         {
         	if(WEXITSTATUS(wait_status) == 0)
         	{
         		
            		printf("Command executed successfully\n");
            	}
         	else {
         		perror("Command_exec_failed");
            		//printf("Command execution failed\n");
            		return false;
        	}
        }
   }
    /* do whatever the parent wants to do. */
    
    va_end(args);

    return true;
}
