#include "lfsheader.h"
#include<stdio.h>
#include<string.h>

void ls(char dir[],char path[])
{


}
void cat(char file[],char path[])
{

}

int main(int argc, char* arg[])
{
	if(argc!=4)
	{
		printf("\nError!\\n");
		printf("\nCommand should be");
		printf("\ncat    \'absolute pathname for a file\'   \'lfs image\'");
		printf("\nls    \'absolute pathname for a directory\'   \'lfs image\'");
      		return -1; 
	}
	char cmd[strlen(arg[1])];
	char path[strlen(arg[2])];
	char img[strlen(arg[3])];
	strcpy(cmd,arg[1]);
   	strcpy(path,arg[2]);
   	strcpy(img,arg[3]);
	if(strcmp(cmd,"ls")==0)
	{
		ls(img,path);
	}
	else if(strcmp(cmd,"cat")==0)
        {
                cat(img,path);
        }
	else
	{
		printf("\nError!\\n");
		return -1;
	}

	//printf("\n%s %s %s",cmd,path,img);
	return 1;



}

