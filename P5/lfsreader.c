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
	int i;
	char cmd[strlen(arg[1])];
        char path[strlen(arg[2])];
        char img[strlen(arg[3])];
       	inodemap *imap[INODEPIECES];
	// make sure format is right
	if(argc!=4)
	{
		printf("\nError!\\n");
		printf("\nCommand should be");
		printf("\ncat    \'absolute pathname for a file\'   \'lfs image\'");
		printf("\nls    \'absolute pathname for a directory\'   \'lfs image\'");
      		return -1; 
	}
	// Assign variables values from cmd args
	strcpy(cmd,arg[1]);
   	strcpy(path,arg[2]);
   	strcpy(img,arg[3]);
	//open file and create basic lfs structure
	
	int lfs=open(img,O_RDONLY);
	//the first part of fs is the cr region
	checkpoint *cr=malloc(sizeof(checkpoint));
	if((read(lfs,cr,sizeof(checkpoint)))<0)
	{
		printf("\nError!\\n");
	}
	for(i=0;i<INODEPIECES;i++)
	{
		
	}


	// check which command is used
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

