#include "fs.h"
#include<stdio.h>
#include<string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

void idisplay(struct dinode *temp)
{
		printf("\ntype %d, nlink %d size %d",temp->type,temp->nlink,temp->size);
}
void dirdisplay(struct dirent *temp)
{
		printf("\ninum %d, name %s ",temp->inum,temp->name);
}
int count(unsigned int n)
{
	unsigned int count = 0;
	while(n)
	{
		count += n & 1;
		n >>= 1;
	}
	return count;
}
int main(int argc, char* arg[])
{
	int n;
	int inodesize=sizeof(struct dinode);
	// make sure format is right
	if(argc!=2)
        {
                printf("Error!\n");
                return -1; 
        }
	char img[strlen(arg[3])];
       	strcpy(img,arg[1]);
   	
	//strcpy(img,"example.img");
	int ffs=open(img,O_RDONLY);
	if(ffs==-1)
	{
                printf("Error!\n");
				return -1;
	}
	struct superblock* sb=malloc(sizeof(struct superblock));
	n=lseek(ffs,BSIZE,SEEK_SET);
	n=(read(ffs,sb,sizeof(struct superblock)));
	//printf("\n size %u block %u num_inodes %u",sb->size,sb->nblocks,sb->ninodes);
	if(sb->ninodes%IPB!=0)
	{
		printf("\nCorrupt file");
		return -1;
	}
	struct stat st;
	stat(img, &st);
	int size = st.st_size;
	if(size!=sb->size*BSIZE)
		printf("\nCorrupt file");
	int inodeblocks=sb->ninodes/8;
	//if(sb->ninodes%8!=0)
	//{
	//	printf("\nadded one");
		inodeblocks++;
	//}
	printf("\nnum of blocks used to store the inode list %d",inodeblocks);
	if(!(sb->size>inodeblocks+1))
	{
		printf("\nSniff test fail");
		return -1;
	}
	struct dinode *temp=malloc(sizeof(struct dinode));
	n=lseek(ffs,2*BSIZE,SEEK_SET);
	struct dinode *imap[sb->ninodes];
	int i;
	int totalsize=0,totalblocks=0;
	for(i=0;i<200;i++)
	{
		imap[i]=malloc(sizeof(struct dinode));
		n=(read(ffs,imap[i],sizeof(struct dinode)));
		
		totalsize+=imap[i]->size;
		totalblocks+=imap[i]->size/BSIZE;
		if(imap[i]->size%BSIZE!=0)
			totalblocks++;
	}
	printf("\n Num bytes %d block %d",totalsize,totalblocks);
	//struct dinode *root=imap[2];
	
	//printf("\nroot inode is at %d",n);
	//n=(read(ffs,root,sizeof(struct dinode)));
	/*idisplay(root);
	struct dirent *dir=malloc(sizeof(struct dirent));
	n=lseek(ffs,root->addrs[0]*BSIZE,SEEK_SET);
	
	for(i=0;i<3;i++)
	{
		n=(read(ffs,dir,sizeof(struct dirent)));
		if(n<=0)
		{
				printf("Not read properly\n");
		}
		dirdisplay(dir);
	}*/
	n=lseek(ffs,2*BSIZE+inodeblocks*BSIZE,SEEK_SET);
	int *bytes=malloc(BSIZE);
	n=read(ffs,bytes,BSIZE);
	printf("\n %d Byte is",n);
	int sum=0;
	for(i=0;i<n;i++)
	{
			unsigned int byte=bytes[i];
			sum+=count(byte);
			printf(" %d",byte);
	
	}
	printf("\nNumber of num bits set = %d",sum);
	return 1;



}

