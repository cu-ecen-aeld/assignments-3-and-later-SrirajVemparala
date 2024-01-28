/*******************************************************************************
 * writer.c
 * Date:        28-01-2024
 * Author:      Raghu Sai Phani Sriraj Vemparala, raghu.vemparala@colorado.edu
 * Description: This file has data related to writer script witten in C
 * References: https://stackoverflow.com/questions/7180293/how-to-extract-filename-from-path
 *
 *
 ******************************************************************************/
 
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <syslog.h>
#include <libgen.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
	openlog(NULL,0,LOG_USER);//Open Log
	if(argc != 3)//if argument count do not match log error
	{
		syslog(LOG_ERR,"Provide filepath and textstring");
		return 1;
	}
	char* filepath = argv[1];
	char* strtobeadded = argv[2];
	char* filename = basename(filepath);//Obtain the file name
	//Open the file
	int fd = open (filepath, O_CREAT|O_RDWR|O_TRUNC|O_NONBLOCK, S_IRWXU|S_IRWXG|S_IRWXO);
	//If the file is not opened then log error
	if(fd == -1)
	{
		syslog(LOG_ERR, "Failed to open file");
		return 1;
	}
	ssize_t nr;
	//Write data to the file
	nr = write (fd, strtobeadded, strlen (strtobeadded));
	if(nr == -1)
	{
		//Log error if print failed
		syslog(LOG_ERR, "write is not sucessful");
		close(fd);
		return 1;
	}
	else
	{
	 //check if the whole string is written to the  file
		if(nr == strlen(strtobeadded))
		{
		syslog(LOG_DEBUG, "Writing %s to %s\n” where %s is the text string written to file %s and %s is the file created by the script",strtobeadded,filename,strtobeadded,filename,filename);
		close(fd);
		}
		else
		{
			syslog(LOG_ERR, "Incorrect information written. Repeat the process");
			close(fd);
			return  1;
		}
	}
	closelog(); //close log
	return 0;
}
