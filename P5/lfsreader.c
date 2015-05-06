#include "lfsheader.h"
#include<stdio.h>
#include<string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

int numoflevels(char num[])
{
	int i,nums=0;
	for(i=0;i<strlen(num);i++)
	{
		if(num[i]=='/')
			nums++;
	}
	return nums;

}

int main(int argc, char* arg[])
{
	int i,j,n,num_ptr;
	int BLOCKSIZE=4096;
	// make sure format is right
	if(argc!=4)
        {
                printf("Error!\n");
                return -1; 
        }
	char cmd[strlen(arg[1])];
        char path[strlen(arg[2])];
        char img[strlen(arg[3])];
       	inodeMap *imap[INODEPIECES];
	int inodes[4096];
	int index=0,k;
	// Assign variables values from cmd args
	strcpy(cmd,arg[1]);
   	strcpy(path,arg[2]);
   	strcpy(img,arg[3]);
	if((strcmp(cmd,"ls")&&strcmp(cmd,"cat"))!=0)
        {
                printf("Error!\n");
                return -1;
        }

	//open file and create basic lfs structure
	int nlevel=0;
	int begin=0,end=0;
	int nums=numoflevels(path);
	char *entries[nums];
	char *token;
	token=strtok(path,"/");
	while(token!=NULL)
	{
		entries[nlevel]=token;
		token=strtok(NULL,"/");
		nlevel++;
	}
	
	//strcpy(img,"example.img");
	int lfs=open(img,O_RDONLY);
	if(lfs==-1)
	{
                printf("Error!\n");
        	return -1;
	}
	//the first part of fs is the cr region
	if(strcmp(cmd,"ls")==0)
	{
		//printf("\nls ");
		nlevel++;
	}
	checkpoint *cr=malloc(sizeof(checkpoint));
	n=(read(lfs,cr,sizeof(checkpoint)));
	if(n<0)
	{
		printf("Error!\n");
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
	while((level<nlevel-1 && strcmp(cmd,"ls")) || (level<nlevel-1 && strcmp(cmd,"cat")))
	{
		level++;
		dirEnt *dir=malloc(sizeof(dirEnt));
		//printf(" Size %d No of directory entries %d No of directories per block %d \n",inodelevel->size,inodelevel->size/sizeof(dirEnt),BLOCKSIZE/(sizeof(dirEnt)));
		int pointers=inodelevel->size/BLOCKSIZE;
		if(inodelevel->size%BLOCKSIZE!=0)
			pointers++;					
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
		if(find==0)
                {
                        printf("Error!\n");
                        return -1;
                }
	

	}
	// inodelevel has the right directory
	if(strcmp(cmd,"ls")==0)
	{
		if(inodelevel->type==1)
		{
			printf("Error!\n");
			return -1;
		}
		int pointers=inodelevel->size/BLOCKSIZE;
                if(inodelevel->size%BLOCKSIZE!=0)
			pointers++;					
		for(num_ptr=0;num_ptr<pointers;num_ptr++)
                {
			dirEnt *dir=malloc(sizeof(dirEnt));
			j=lseek(lfs,0,SEEK_SET);
        		j=lseek(lfs,inodelevel->ptr[num_ptr],SEEK_SET);
			dirEnt *entries[64];
			for(i=0;i<64;i++)
                        {
				entries[i]=malloc(sizeof(dirEnt));
                                read(lfs,entries[i],sizeof(dirEnt));
                                //if(dir->inum==-1)
                                 //       continue;

                                //printf("%s\n",dir->name,dir->inum);
                        }
			//j=lseek(lfs,0,SEEK_SET);
                        //j=lseek(lfs,inodelevel->ptr[num_ptr],SEEK_SET);
                       
        		for(i=0;i<64;i++)
        		{
        			//read(lfs,dir,sizeof(dirEnt));
        			dir=entries[i];
                		if(dir->inum==-1)
                			continue;
                       		inode *x=malloc(sizeof(inode));
				j=lseek(lfs,inodes[dir->inum],SEEK_SET);
				read(lfs,x,sizeof(inode));
				
				printf("%s",dir->name);
				if(x->type==0)
					printf("/");
				printf("\n");
			}
		}
	}
	else if(strcmp(cmd,"cat")==0)
        {
                dirEnt *dir=malloc(sizeof(dirEnt));
		int pointers1=inodelevel->size/BLOCKSIZE;
		if(inodelevel->size%BLOCKSIZE!=0)
			pointers1++;					
		int find=0;
		for(num_ptr=0;num_ptr<pointers1;num_ptr++)
		{		
                	j=lseek(lfs,0,SEEK_SET);
                	j=lseek(lfs,inodelevel->ptr[num_ptr],SEEK_SET);
                	for(i=0;i<64;i++)
                	{
                        	read(lfs,dir,sizeof(dirEnt));
                        	if(dir->inum==-1)
                                	continue;
				if(strcmp(entries[level],dir->name)==0)
                        	{	

					//printf("\n%s %d",dir->name,dir->inum);
				
					//inode of file set to new
					j=lseek(lfs,0,SEEK_SET);
        	                       	j=lseek(lfs,inodes[dir->inum],SEEK_SET);
	                        	inode *new=malloc(sizeof(inode));
                                	read(lfs,new,sizeof(inode));
					if(new->type==0)
                                        {
	                                        printf("Error!\n");
        	                                return -1;
                                        }
					find=1;
					// file contenet is at data
				//	printf("\t Size %d",new->size);
					int fileptrs;
					int pointers=new->size/BLOCKSIZE;
					if(new->size%BLOCKSIZE!=0)
						pointers++;
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
		if(find==0)
                {
                        printf("Error!\n");
                        return -1;
                }


        }

	return 1;



}

