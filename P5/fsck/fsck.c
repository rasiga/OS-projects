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
//			bitmap[bitindex++]= (n & 1);
		count += n & 1;
		n >>= 1;
	}
	return count;
}
int main(int argc, char* arg[])
{
	int n,j,i,k;
	//int links_changed[sb->inodes];
	//int linkchanges=0;
	int inodesize=sizeof(struct dinode);
	// make sure format is right
	if(argc!=2)
        {
                printf("Error!\n");
                return -1; 
        }
	char img[strlen(arg[1])];
       	strcpy(img,arg[1]);
   	
	//strcpy(img,"example.img");
	int ffs=open(img,O_RDWR);
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
		printf("\nError\n");
		return -1;
	}
	//printf("\n sb->nblocks is %d",sb->nblocks);
	struct stat st;
	stat(img, &st);
	int size = st.st_blocks;
	if(size!=sb->size)
	{
		//printf("Error!\n");
		//printf("\nInvalid size is %d should be %d",sb->size,size);
		sb->size=size;
		//sb->nblocks=st.st_blocks;      // Number of data blocks

		
		n=lseek(ffs,BSIZE,SEEK_SET);
        	n=(write(ffs,sb,sizeof(struct superblock)));
	
		n=lseek(ffs,BSIZE,SEEK_SET);
                n=(read(ffs,sb,sizeof(struct superblock)));
		printf("\nNew size is %d",sb->size);
                

		//printf("\n1");
	}
	if(sb->ninodes > size || sb->nblocks > size)
	{
		printf("Error!\n");
		return -1;
	}	
	int inodeblocks=sb->ninodes/8;
	inodeblocks++; // because ther is one empty block
	//printf("\nnum of blocks used to store the inode list %d",inodeblocks);
	if(!(sb->size>inodeblocks+1))
	{
		printf("\nSniff test fail");
		return -1;
	}


	n=lseek(ffs,2*BSIZE+inodeblocks*BSIZE,SEEK_SET);
        //printf("\n is at %x",n);
        int *bytes=malloc(BSIZE);
        n=read(ffs,bytes,BSIZE);
        //printf("\n %d Byte is sizeof(int) is %d\n",n,sizeof(int));
        int sum=0;
        n=n/sizeof(int);
	int tblocks=0;
	int bitmap[10000];
	int bitindex=0;

        for(i=0;i<n;i++)
        {
                        unsigned int byte=bytes[i];
                        sum+=count(byte);
			
                  //      printf(" %x",byte);
			bitmap[bitindex++]= (byte & 1);
			bitmap[bitindex++]= (byte >> 1) & 1;
                        bitmap[bitindex++]= (byte >> 2) & 1;
                        bitmap[bitindex++]= (byte >> 3) & 1;
			bitmap[bitindex++]= (byte >> 4) & 1;
                        bitmap[bitindex++]= (byte >> 5) & 1;
                        bitmap[bitindex++]= (byte >> 6) & 1;
                        bitmap[bitindex++]= (byte >> 7) & 1;
			
			bitmap[bitindex++]= (byte >> 8) & 1;
                        bitmap[bitindex++]= (byte >> 9) & 1;
                        bitmap[bitindex++]= (byte >> 10) & 1;
                        bitmap[bitindex++]= (byte >> 11) & 1;
                        bitmap[bitindex++]= (byte >> 12) & 1;
                        bitmap[bitindex++]= (byte >> 13) & 1;
                        bitmap[bitindex++]= (byte >> 14) & 1;
                        bitmap[bitindex++]= (byte >> 15) & 1;


			bitmap[bitindex++]= (byte >> 16) & 1;
                        bitmap[bitindex++]= (byte >> 17) & 1;
                        bitmap[bitindex++]= (byte >> 18) & 1;
                        bitmap[bitindex++]= (byte >> 19) & 1;
                        bitmap[bitindex++]= (byte >> 20) & 1;
                        bitmap[bitindex++]= (byte >> 21) & 1;
                        bitmap[bitindex++]= (byte >> 22) & 1;
                        bitmap[bitindex++]= (byte >> 23) & 1;

	
			bitmap[bitindex++]= (byte >> 24)& 1;
                        bitmap[bitindex++]= (byte >> 25) & 1;
                        bitmap[bitindex++]= (byte >> 26) & 1;
                        bitmap[bitindex++]= (byte >> 27) & 1;
                        bitmap[bitindex++]= (byte >> 28) & 1;
                        bitmap[bitindex++]= (byte >> 29) & 1;
                        bitmap[bitindex++]= (byte >> 30) & 1;
                        bitmap[bitindex++]= (byte >> 31) & 1;


                       
		//	printf(" %d\n",bitindex);

        }
        printf("\nNumber of num bits set = %d",sum);
	/*for(i=0;i<n;i++)
        {
                        unsigned int byte=bytes[i];
			bitmap[bitindex++]=(byte & 0x1) >> 0;                        
			bitmap[bitindex++]=(byte & 0x2) >> 1;
                        bitmap[bitindex++]=(byte & 0x4) >> 2;
                        bitmap[bitindex++]=(byte & 0x8) >> 3;
			
        }
//	for(i=0;i<bitindex;i++)
//	{
//		printf("\n i %d bit %d",i,bitmap[i]);
//	} 
	*/

	int maxsize = 1 + inodeblocks + (sb->ninodes - sum);  // RASIGA WILL NOT FORGET 421=totalblocks changed 421 to sum
        //printf("\n SUM IS %d",sum);



	int blocks[sb->nblocks];
	for(i=0;i<sb->nblocks;i++)
		blocks[i] = 0; 

	struct dinode *temp=malloc(sizeof(struct dinode));
	n=lseek(ffs,2*BSIZE,SEEK_SET);
	struct dinode *imap[sb->ninodes];
	//int i;
	int totalblocks=0;
	uint indirect[sb->ninodes];
	for(i=0;i<sb->ninodes;i++)
		indirect[i]=0;
	for(i=0; i<sb->ninodes; i++)
	{
		imap[i]=malloc(sizeof(struct dinode));
		n=(read(ffs,imap[i],sizeof(struct dinode)));
	
		/********************After calculating totalblocks****************************/
		
		//checking valid inode type	
		if( imap[i]->type!=0 && imap[i]->type!=1 && imap[i]->type!=2 && imap[i]->type!=3 )		
		{
			printf("\n Invalid inode");
			imap[i]->type = 0; //clear the inode
		}

		//checking the size field in inode
		if(imap[i]->size > maxsize)
		{
			printf("\n Invalid size in inode");
			imap[i]->type = 0;
		}

		//if no file is pointing to this inode, invalidate it
		if(imap[i]->nlink == 0)
			imap[i]->type=0;


	//	if(i==0)
	//		continue;
		//printf("\nInode %d:",i);
		
		if(imap[i]->type==0)
			continue;
		
		for(j=0;j<13;j++)		
		{
		//	printf(" %d",imap[i]->addrs[j]);
			if(imap[i]->addrs[j]==0)
				continue;
			if(blocks[imap[i]->addrs[j]] == 1)
			{
				printf("\nrepeated");
			}
			else
			{
				 blocks[imap[i]->addrs[j]]=1;
			}
			totalblocks++;
		}
		indirect[i]=imap[i]->addrs[12];
		//printf("\n Indirect pointer of %d is: %d",i,indirect[i]);
	}
	//printf("\n Number of direct addresses %d",totalblocks);
	uint ind_addr;
	uint *data=malloc(sizeof(BSIZE));
	for(i=0; i<sb->ninodes; i++)
        {
		
		if(indirect[i]!=0)
		{
			n=lseek(ffs,indirect[i]*BSIZE,SEEK_SET);
			n=read(ffs,data,BSIZE);
			for(j=0;j<BSIZE/sizeof(uint);j++)
			{
			//	printf("\n Indirect %d",data[j]);
				if(data[j]!=0)
				{
					totalblocks++;
					blocks[data[j]]=1;
				}
			}

                /*	n=lseek(ffs,indirect[i]*BSIZE,SEEK_SET);
			for(k=0;k<BSIZE/sizeof(uint);k++)
			{
                		n=read(ffs,ind_addr,sizeof(uint));
				//printf("\n size is %d",n);
                		//for(j=0;j<BSIZE/sizeof(uint);j++)
                //		printf(" %d",ind_addr);
				{
                        		if(ind_addr == 0)
						continue;
                                	totalblocks++;
					if(blocks[ind_addr] == 1)
                        		{
                                		printf("\nrepeated");
                        		}
                        		else
                        		{
                                		 blocks[ind_addr]=1;
                        		}
				}

                	}*/
		}

        }

	printf("\nNum blocks traversing the inode %d",totalblocks);
	for(i=0;i<sb->nblocks;i++)
	{
	//	printf("\n i %d index %d",i,blocks[i]);
		tblocks+=blocks[i];
	}
	printf("\n sum is %d",tblocks);
	n=lseek(ffs,2*BSIZE,SEEK_SET);
	for(i=0;i<sb->ninodes;i++) // imap[i]s updated in place
        {
                n=(write(ffs,imap[i],sizeof(struct dinode)));
        }
	struct dirent *dir=malloc(sizeof(struct dirent));
	int *addr=malloc(sizeof(BSIZE));
	//printf("\n%d DIR",sizeof(struct dirent));
	int links[sb->ninodes];
	int parent[sb->ninodes];
	int level[sb->ninodes];
	int exists[sb->ninodes];
	int inodes_seen[sb->ninodes];
	//initializing the links and parent variables
	for(i=0;i<sb->ninodes;i++)
	{
		links[i]=0;
		parent[i]=0;
		exists[i]=0;
		inodes_seen[i]=0;
	}
	int lseekpos;
	int lost_found=0;
	for(i=0;i<sb->ninodes;i++)
	{
		//set the exixsts to 2 for valid inodes
		if(imap[i]->type!=0)
		{
	//		printf("\n Inum is %d",i);
			exists[i]=2;
		}
		if(imap[i]->type==1)
		{
	//		printf("\nAt dir %d",i);
			for(j=0;j<12;j++)
			{
				if(imap[i]->addrs[j]==0)
					continue;
        			lseekpos=lseek(ffs,imap[i]->addrs[j]*BSIZE,SEEK_SET);
				for(k=0;k<BSIZE/sizeof(struct dirent);k++)
				{
					//parent[dir->inum]=i;
					n=(read(ffs,dir,sizeof(struct dirent)));
					/**********************Counting the links for each inum**************/
					//now that you have read the dir entry, increment the nlinks for that dir->inum
					if(strcmp(dir->name,".")!=0 && strcmp(dir->name,"..")!=0 && dir->name!=NULL) //other than the . and .. entries
					{
						links[dir->inum]++;
						//printf("\n%s");


					}
					if(! ((j==0 && k==0) ||( j==0 && k==1) ) ) // if it is not . and ..
						parent[dir->inum]=i;
					if(j ==0 && k==0)
					{
						// must be . pointing to itself
						if(dir->name==NULL || strcmp(dir->name,".")!=0 || dir->inum!=i) // if 1st entry is not pointing to itself
						{
						//	printf("\nmust be . pointing to itself");
							dir->inum=i;
							strcpy(dir->name,".");
							
							lseek(ffs,lseekpos + k * (sizeof(struct dirent)),SEEK_SET);
							n=(write(ffs,dir,sizeof(struct dirent)));
						}
					}
					if(j ==0 && k==1)
                                        {
                                                // must be .. pointing to parent
                                                if(i!=1) // if it is not root
						{
                                                	if(dir->name==NULL || strcmp(dir->name,"..")!=0 ||(dir->inum!=parent[i] )) // second entry point to parent of i 
                                                	{
                                                  //      	printf("\nmust be .. pointing to parent");
								dir->inum=parent[i];
  	                                                        strcpy(dir->name,"..");
                                                        	lseek(ffs,lseekpos + k * (sizeof(struct dirent)),SEEK_SET);
                                                        	n=(write(ffs,dir,sizeof(struct dirent)));

                                                	}
						}
						else
						{

							if(dir->name==NULL || strcmp(dir->name,"..")!=0 || dir->inum!=1) // second entry should point to itself
                                                	{
                                            	        //    	printf("\nmust be .. pointing to parent");
								dir->inum=1;
                                                                strcpy(dir->name,"..");
                                                                lseek(ffs,lseekpos + k * (sizeof(struct dirent)),SEEK_SET);
                                                                n=(write(ffs,dir,sizeof(struct dirent)));

                                                	}
						}

                                        }
					if(dir->inum==0)
						continue;
				//	printf("\n INUM %d NAME %s ",dir->inum,dir->name);

					if(strcmp(dir->name,"lost+found")==0)
					{
						lost_found=dir->inum;
					}		
					//parent[dir->inum]=i; //set the parent of all children
							
				}

        		}
			/********************Go to the place pointed by indirect pointer*******************************************/
			n=lseek(ffs,imap[i]->addrs[12]*BSIZE,SEEK_SET);
			if(imap[i]->addrs[12]==0)
				continue;
			n=(read(ffs,addr,sizeof(BSIZE)));
			for(j=0;j<BSIZE/sizeof(uint);j++)
                        {
				if(addr[j]==0)
					continue;
				n=lseek(ffs,addr[j]*BSIZE,SEEK_SET);
				for(k=0;k<BSIZE/sizeof(struct dirent);k++)
                                {
                                        n=(read(ffs,dir,sizeof(struct dirent)));
                                        if(dir->inum==0)
                                                continue;
                                  //      printf("\n INUM %d NAME %s ",dir->inum,dir->name);
					links[dir->inum]++;
                                }

			}

		}
	}
	
	/***************Go through the inodes to see if the links count matches with the calculated one; if not change it**************/
	for(i=0;i<sb->ninodes;i++)
	{
		//printf("\n Type %d Calculated nlink for inode %d is: %d, the one found in the inode is %d",imap[i]->type,i,links[i],imap[i]->nlink);
		//link count for a dir is 1
		if((imap[i]->type==1 || imap[i]->type==3) && imap[i]->nlink!=1)
			imap[i]->nlink = 1;
		//change the nlink for a file it doesnt match the one calculated
		if(imap[i]->type==2)
		{	if(imap[i]->nlink != links[i])
			{
				printf("\n the linkcount doesn't match for %d, changing it %d %d",i,imap[i]->nlink,links[i]);
				imap[i]->nlink = links[i];		
			}
		}
	}
	/************rewrite inode with correct link count**************/
	n=lseek(ffs,2*BSIZE,SEEK_SET);
        for(i=0;i<sb->ninodes;i++) // imap[i]s updated in place
        {
		//if(imap[i]->type==2)
                	n=(write(ffs,imap[i],sizeof(struct dinode)));
        }
	/**********************Checking the no referenced inode and 
	
	 for(i=0;i<sb->ninodes;i++)
        {
		 //if(i==0)
                 //       continue;
		//printf(" %d",indirect[i]);

		//printf("size of uint %d ", sizeof(uint));
		n=lseek(ffs,indirect[i]*BSIZE,SEEK_SET);
		//for(j=0;j<BSIZE/sizeof(uint);j++)
		//{
			uint *ind_addr=malloc(sizeof(BSIZE));
			//uint *ind_addr;
			n=(read(ffs,ind_addr,sizeof(BSIZE)));
		for(j=0;j<BSIZE/sizeof(uint);j++)
		{
			//printf("Size of ind_addr[j] is %d",sizeof(ind_addr[j]));
			if(ind_addr[j]!=0)
				totalblocks++;
		}

        }
	
//	printf("\n Num block %d",totalblocks);


	/*	
	struct dinode *root=imap[1];
	
	//printf("\nroot inode is at %d",n);
	n=lseek(ffs,2*BSIZE+sizeof(struct dinode),SEEK_SET);
	n=(read(ffs,root,sizeof(struct dinode)));
	idisplay(root);
	//struct dirent *dir=malloc(sizeof(struct dirent));
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

	//printf("Num is %d",(2+inodeblocks));

/********** BITMAP     ***********/
/*	n=lseek(ffs,2*BSIZE+inodeblocks*BSIZE,SEEK_SET);
	printf("\n is at %x",n);
	int *bytes=malloc(BSIZE);
	n=read(ffs,bytes,BSIZE);
	printf("\n %d Byte is sizeof(int) is %di\n",n,sizeof(int));
	int sum=0;
	n=n/sizeof(int);
	for(i=0;i<n;i++)
	{
			unsigned int byte=bytes[i];
			sum+=count(byte);
			printf(" %x",byte);
	
	}
	printf("\nNumber of num bits set = %d",sum);
*/

	/****************Checking the nodes that are not referenced (ie., not reachable from the root)*************/
	//struct dinode *currinode = imap[1];
	int traversed=0,index=1;
	exists[1]=1;
	inodes_seen[0]=1;
	// lost & found inum is in lost_found
	//printf("\n Lost and found dir inum is %d",lost_found);
//	printf("\n---------------------------------------------------------------------------------");
	while(inodes_seen[traversed]!=0)
	{
		i=inodes_seen[traversed];		
		traversed++;
		if(imap[i]->type==1)
		{
	//		printf("\nAt dir %d",i);
			for(j=0;j<12;j++)
			{
				if(imap[i]->addrs[j]==0)
					continue;
        			lseekpos=lseek(ffs,imap[i]->addrs[j]*BSIZE,SEEK_SET);
				for(k=0;k<BSIZE/sizeof(struct dirent);k++)
				{
					n=(read(ffs,dir,sizeof(struct dirent)));
					if(dir->inum==0)
						continue;
	//				printf("\n INUM %d NAME %s ",dir->inum,dir->name);
					exists[dir->inum]=1;
					if(imap[dir->inum]->type==1 && !( (j==0 && k==0)|| (j==0 && k==1) ) )
					{
					//	printf("\nadding %d",dir->inum);					
						inodes_seen[index]=dir->inum;
						index++;
					}
								
				}

        		}
			/********************Go to the place pointed by indirect pointer*******************************************/
			n=lseek(ffs,imap[i]->addrs[12]*BSIZE,SEEK_SET);
			if(imap[i]->addrs[12]==0)
				continue;
			n=(read(ffs,addr,sizeof(BSIZE)));
			for(j=0;j<BSIZE/sizeof(uint);j++)
                        {
				if(addr[j]==0)
					continue;
				n=lseek(ffs,addr[j]*BSIZE,SEEK_SET);
				for(k=0;k<BSIZE/sizeof(struct dirent);k++)
                                {
                                        n=(read(ffs,dir,sizeof(struct dirent)));
                                        if(dir->inum==0)
                                                continue;
          //                              printf("\n INUM %d NAME %s ",dir->inum,dir->name);
					exists[dir->inum]=1;
					if(imap[dir->inum]->type==1)
					{
	//					printf("\nadding %d",dir->inum);					
						inodes_seen[index]=dir->inum;
						index++;
					}
			       }

			}

		}
	}
	//n=lseek(ffs,lost_found*BSIZE,SEEK_SET);
	i=lost_found;
	//printf("\nlost+found has");
	int lfentry;
	for(j=0;j<12;j++)
        {
                                if(imap[i]->addrs[j]==0)
                                        continue;
                                lseekpos=lseek(ffs,imap[i]->addrs[j]*BSIZE,SEEK_SET);
				//printf("\n LSEEK at %d",lseekpos);
				int pos=lseekpos;
                                for(k=0;k<BSIZE/sizeof(struct dirent);k++)
                                {
                                        n=(read(ffs,dir,sizeof(struct dirent)));
					pos+=n;
                                        if(dir->inum==0)
                                        {
						//printf("\t%d ",pos);
						lfentry=lseek(ffs,pos-sizeof(struct dirent),SEEK_SET);
						continue;	
					}
				//	printf("\n INUM %d NAME %s ",dir->inum,dir->name);
                                }

        }
	//printf("\n Lfentry at %d",lfentry);
	lseekpos=lseek(ffs,lfentry,SEEK_SET);
	//printf("\nsize : %d",imap[lost_found]->size);
	for(i=0;i<sb->ninodes;i++) // imap[i]s updated in place
        {
		if(exists[i]==2)
		{
   //             	printf("\n Imap %d can't be traversed from root",i);
			dir->inum=i;
			strcpy(dir->name,"lost");
			write(ffs,dir,sizeof(struct dirent));
			imap[lost_found]->size+=sizeof(struct dirent);

		}
        }
	
	/*Checking if written in lost+found
	i=lost_found;
        printf("\nlost+found has");
        for(j=0;j<12;j++)
        {
                                if(imap[i]->addrs[j]==0)
                                        continue;
                                lseekpos=lseek(ffs,imap[i]->addrs[j]*BSIZE,SEEK_SET);
                                for(k=0;k<BSIZE/sizeof(struct dirent);k++)
                                {
                                        n=(read(ffs,dir,sizeof(struct dirent)));
                                        if(dir->inum==0)
                                        {
                                                continue;
                                        }
                                        printf("\n INUM %d NAME %s ",dir->inum,dir->name);
                                }

        }
	*/

	/************rewrite inode with correct changed imap size**************/
        n=lseek(ffs,2*BSIZE,SEEK_SET);
       
	 for(i=0;i<sb->ninodes;i++) // imap[i]s updated in place
        {
 
                n=(write(ffs,imap[i],sizeof(struct dinode)));
        }
	/*n=lseek(ffs,2*BSIZE,SEEK_SET);
	for(i=0;i<sb->ninodes;i++) // imap[i]s updated in place
        {

                n=(read(ffs,imap[i],sizeof(struct dinode)));
        }*/
//	printf("\nsize : %d",imap[lost_found]->size);
	
	//printf("\n sb->ninodes %d ",sb->ninodes);
	 
	return 1;



}

