//References: https://stackoverflow.com/questions/7180293/how-to-extract-filename-from-path

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
	openlog(NULL,0,LOG_USER);
	//printf("Started logging\n");
	//printf("file loc:%s\n",argv[1]);
	//printf("file str:%s\n",argv[2]);
	if(argc != 3)
	{
		//printf("args invalid\n");
		syslog(LOG_ERR,"Provide filepath and textstring");
		return 1;
	}
	char* filepath = argv[1];
	char* strtobeadded = argv[2];
	char* filename = basename(filepath);
	int fd = open (filepath, O_CREAT|O_RDWR|O_TRUNC|O_NONBLOCK, S_IRWXU|S_IRWXG|S_IRWXO);
	if(fd == -1)
	{
		//perror("Open failed");
		//printf("Open failed\n");
		syslog(LOG_ERR, "Failed to open file");
		return 1;
	}
	ssize_t nr;
	//printf("Write started");
	nr = write (fd, strtobeadded, strlen (strtobeadded));
	if(nr == -1)
	{
		//printf("Write failed\n");
		syslog(LOG_ERR, "write is not sucessful");
		close(fd);
		return 1;
	}
	else
	{
	 //printf("In else verifying nr\n");
		if(nr == strlen(strtobeadded))
		{
		syslog(LOG_DEBUG, "Writing %s to %s\n” where %s is the text string written to file %s and %s is the file created by the script",strtobeadded,filename,strtobeadded,filename,filename);
		}
		else
		{
			
			syslog(LOG_ERR, "Incorrect information written. Reapt the process");
			close(fd);
			return  1;
		}
	}
	closelog();
	return 0;
}
