#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/fcntl.h>
#define BUFSIZE 1024
int main(int argc, char **argv)
{
	//printf("DDDDD");
	char file[BUFSIZE];
	int n;
	int fd;	
	//read(stdin,file,strlen(file));
	//fprintf(stdout,"\n%s",file);
	//close(1); //close the STDOUT
 	fd = open("tee.txt",O_CREAT|O_TRUNC|O_RDWR,0644);
        while((n = read(0, file, strlen(file))) > 0)
	{
		write(fd,file,strlen(file));
		fprintf(stdout,"%s",file);
	
	}
	close(fd);
	return 1;
}


