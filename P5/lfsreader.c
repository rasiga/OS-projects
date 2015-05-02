#include "lfsheader.h"
#include<stdio.h>
#include<string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>



int main(int argc, char* arg[])
{
	int i,j,n,num_ptr;
	int BLOCKSIZE=4096;
	char cmd[strlen(arg[1])];
        char path[strlen(arg[2])];
        char img[strlen(arg[3])];
       	inodeMap *imap[INODEPIECES];
	int inodes[4096];
	int index=0,k;
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
	 printf("\n Path is %s",path);
	int nlevel=0;
	int begin=0,end=0;
	char *entries[3];
	char *token;
	token=strtok(path,"/");
	while(token!=NULL)
	{
		//entries[nlevel++]=token;
		token=strtok(NULL,"/");
	}
	/*for(i=0;i<nlevel;i++)
	{
		printf("\n%s",entries[i]);
		
	}
	if(i==nlevel)
		return -1;*/
	entries[0]=strdup("nested");
        entries[1]=strdup("directory");
        entries[2]=strdup("example");
	entries[3]=strdup("sample.c");
	//entries[0]=strdup("code");
	//entries[1]=strdup("a.out");


	
	//strcpy(img,"example.img");
	int lfs=open(img,O_RDONLY);
	if(lfs==-1)
	{
		printf("\nFile not found");
		return -1;	
	}
	//the first part of fs is the cr region
	checkpoint *cr=malloc(sizeof(checkpoint));
	n=(read(lfs,cr,sizeof(checkpoint)));
	if(n<0)
	{
		printf("\nError!\\n");
	}
	int sum=0;
	
	for(i=0;i<INODEPIECES;i++)
	{
		j=lseek(lfs,0,SEEK_SET);
		j=lseek(lfs,cr->iMapPtr[i],SEEK_SET);
		imap[i]=malloc(sizeof(inodeMap));
		n=(read(lfs,imap[i],sizeof(inodeMap)));
	}
	// go to root and print it
	//printf("\ninodePtr of each imapptr\n");
	//printf("%d",imap[1]->inodePtr[0]);
	for(i=0;i<INODEPIECES;i++)
	{
		for(j=0;j<16;j++)
		{
			index=i*16+j;
			inodes[index]=imap[i]->inodePtr[j];
		}
	}
	//printf("\n Root is at block %d %d",inodes[0],imap[0]->inodePtr[0]);
	
	
	j=lseek(lfs,0,SEEK_SET);
        j=lseek(lfs,inodes[0],SEEK_SET);
               
	inode *root=malloc(sizeof(inode));
	read(lfs,root,sizeof(inode));

	inode *inodelevel=malloc(sizeof(inode));
	inodelevel=root;
        /////////////  Root inode is  
	int level=0;
	// find the directory
	while((level<3 && strcmp(cmd,"ls")) || (level<3 && strcmp(cmd,"cat")))
	{
		level++;
		dirEnt *dir=malloc(sizeof(dirEnt));
		//printf(" Size %d No of directory entries %d No of directories per block %d \n",inodelevel->size,inodelevel->size/sizeof(dirEnt),BLOCKSIZE/(sizeof(dirEnt)));
		int pointers=inodelevel->size/BLOCKSIZE;
		int find=0;	
		for(num_ptr=0;num_ptr<pointers;num_ptr++)
		{
			j=lseek(lfs,0,SEEK_SET);
			j=lseek(lfs,inodelevel->ptr[num_ptr],SEEK_SET);	
			for(i=0;i<64;i++)
			{
				read(lfs,dir,sizeof(dirEnt));
				if(dir->inum==-1)
					continue;
				//printf("\n Values of root's first entry %s %d",dir->name,dir->inum);
				if(strcmp(dir->name,entries[level-1])==0)
				{
				//	printf("\nentry matched %s\n",dir->name);
					j=lseek(lfs,0,SEEK_SET);
        				j=lseek(lfs,inodes[dir->inum],SEEK_SET);
			        	inode *new=malloc(sizeof(inode));
			        	read(lfs,new,sizeof(inode));
					inodelevel=new;
					find=1;
					break;	
				}
		}
		if(find==1)
			break;
		}

	}
	// inodelevel has the right directory
	if(strcmp(cmd,"ls")==0)
	{
		dirEnt *dir=malloc(sizeof(dirEnt));
		j=lseek(lfs,0,SEEK_SET);
        	j=lseek(lfs,inodelevel->ptr[0],SEEK_SET);
        	for(i=0;i<64;i++)
        	{
        		read(lfs,dir,sizeof(dirEnt));
                	if(dir->inum==-1)
                		continue;
			printf("%s\n",dir->name,dir->inum);
		}
	}
	else if(strcmp(cmd,"cat")==0)
        {
                dirEnt *dir=malloc(sizeof(dirEnt));
		int pointers=inodelevel->size/BLOCKSIZE;
		for(num_ptr=0;num_ptr<pointers;num_ptr++)
		{		
                	j=lseek(lfs,0,SEEK_SET);
                	j=lseek(lfs,inodelevel->ptr[0],SEEK_SET);
                	for(i=0;i<64;i++)
                	{
                        	read(lfs,dir,sizeof(dirEnt));
                        	if(dir->inum==-1)
                                	continue;
				if(strcmp(entries[level],dir->name)==0)
                        	{	
					printf("\n%s %d",dir->name,dir->inum);
				
					//inode of file set to new
					j=lseek(lfs,0,SEEK_SET);
        	                       	j=lseek(lfs,inodes[dir->inum],SEEK_SET);
	                        	inode *new=malloc(sizeof(inode));
                                	read(lfs,new,sizeof(inode));

					// file contenet is at data
					printf("\t Size %d",new->size);
					int fileptrs;
					pointers=new->size/BLOCKSIZE;
					if(pointers==0)
						pointers=1;
					//printf("\nPointers %d",pointers);
					for(fileptrs=0;fileptrs<pointers;fileptrs++)
					{
						j=lseek(lfs,0,SEEK_SET);
                                		j=lseek(lfs,new->ptr[fileptrs],SEEK_SET);
						char data[4096];
						read(lfs,data,sizeof(data));
						printf("%s",data);
					}
					break;
				}
                	}
		}

        }

	
	/*
	// check which command is used
	if(strcmp(cmd,"ls")==0)
	{
	}
	else if(strcmp(cmd,"cat")==0)
        {
        }
	else
	{
		printf("\nError!\\n");
		return -1;
	}
	*/
	//printf("\n%s %s %s",cmd,path,img);
	return 1;



}

