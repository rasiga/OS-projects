#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/fcntl.h>
#define BUFSIZE 1024
int pipes[2],pipes2[2];
	
int searchpipe(char line[])
{
	int i,k=0;
	for(i=0;i<strlen(line);i++)
	{
		if(line[i]=='|')
			k++;
	}
	return k;

}

int searchtee(char line[])
{
	int i,k=0;
	for(i=0;i<strlen(line);i++)
	{
		if(line[i]=='%')
			k++;
	}
	return k;

}

int searchredirect(char line[])
{
	int i,k=0;
	for(i=0;i<strlen(line);i++)
	{
		if(line[i]=='>')
			k++;
	}
	return k;

}

void nopipe(char line[])
{
				char pwd[BUFSIZE];
        char * cmd[10];
        char * temp;
        char * cmdtrunc[10];
        int i=0,j=0,k=0,clen=0,fd;
        int rc;
	
				i=0;
                        	temp = strtok (line," ");
                        	while (temp != NULL)
                        	{
                                	cmd[i]=(char *)malloc(sizeof(char)*(strlen(temp)+1));
	                                strcpy(cmd[i],temp);
	                                temp = strtok (NULL, " ");
        	                        i++;
        	                }
        	                cmd[i]=NULL;
        	                cmd[i-1][strlen(cmd[i-1])-1]='\0';
        	                if(strcmp(cmd[0],"exit")==0)
        	                {
					if(i==1)                                
						exit(0);
					else
						fprintf(stderr,"Error!\n");				
        	                }
        	                else if(strcmp(cmd[0],"pwd")==0)
        	                {
        	                        getcwd(pwd);
        	 			printf("%s\n",pwd);
        	                }
        	                else if(strcmp(cmd[0],"cd")==0)
        	                {
        	                        if(i==1)
        	                        {
        	                                strcpy(pwd,getenv("HOME"));
        	                                if (chdir (pwd) == -1) {
        	                                        fprintf(stderr,"Error!\n");
        	                                }
        	                        }
        	                        else
        	                        {
        	                                if (chdir (cmd[1]) == -1) {
        	                                        fprintf(stderr,"Error!\n");
        	                                }
        	                        }
        	                }
        	                else
        	                {
        	                        rc = fork();
        	                        if(rc==0)
        	                        {       
						for(j=0;cmd[j]!=NULL;j++)
						{
	                                        	if(strcmp(cmd[j],">")==0 || strcmp(cmd[j],">>")==0)
							{
	                                                        close(1); //close the STDOUT
 	         	                                        if (strcmp(cmd[j],">")==0)
         	                                                       fd = open(cmd[j+1],O_CREAT|O_TRUNC|O_RDWR,0644);
                	                                        else if(strcmp(cmd[j],">>")==0)
                                                                	fd = open(cmd[j+1],O_CREAT|O_APPEND|O_RDWR,0644);
                                                        	if(fd<0)
                                                        	{
                                                                	fprintf(stderr,"Error!\n");
                                                        	}	
                                                        	k=0;
                                                        	while(k!=j)
                                                        	{
                                                                	cmdtrunc[k] = (char*)malloc(sizeof(char)*(strlen(cmd[k])+1));
                                                                	strcpy(cmdtrunc[k],cmd[k]);
                                                                	k++;
                                                        	}
 								cmdtrunc[k] = NULL;
                                                        	//cmdtrunc[k-1][strlen(cmdtrunc[k-1])-1]='\0';

                                                                execvp(cmd[0],cmdtrunc);
                                                                fprintf(stderr,"Error!\n");
     	                                                }
             	                                    

                    	                  	}
                                          	{

                                                        execvp(cmd[0],cmd);
                                                        fprintf(stderr,"Error!\n");

                                        	}
                                	}
                                	else
					{
                                        	wait();
   		                        }

	                        }


}


void onepipe(char line[],int number)
{
 char pwd[BUFSIZE];
        char * cmd[10];
        char * temp;
        char * cmdtrunc[10];
        int i=0,j=0,k=0;
	int rc;
        int fd,fd2;
	temp=NULL;
				i=0;
				//strcpy(line,cmd1);
				temp = strtok (line," ");
                        	while (temp != NULL)
                        	{
                                	cmd[i]=(char *)malloc(sizeof(char)*(strlen(temp)+1));
	                                strcpy(cmd[i],temp);
	                                temp = strtok (NULL, " ");
        	                        i++;
        	                }
        	                cmd[i]=NULL;
				if(number==1)
        	               	{	
					// cmd[i-1][strlen(cmd[i-1])-1]='\0';
				}
				else
				{
					cmd[i-1][strlen(cmd[i-1])-1]='\0';
				}
        	               if(strcmp(cmd[0],"exit")==0)
        	                {
					if(i==1)                                
						exit(0);
					else
						fprintf(stderr,"Error!\n");				
        	                }
        	                else if(strcmp(cmd[0],"pwd")==0)
        	                {
        	                        getcwd(pwd);
        	 			printf("%s\n",pwd);
        	                }
        	                else if(strcmp(cmd[0],"cd")==0)
        	                {
        	                        if(i==1)
        	                        {
        	                                strcpy(pwd,getenv("HOME"));
        	                                if (chdir (pwd) == -1) {
        	                                        fprintf(stderr,"Error!\n");
        	                                }
        	                        }
        	                        else
        	                        {
        	                                if (chdir (cmd[1]) == -1) {
        	                                        fprintf(stderr,"Error!\n");
        	                                }
        	                        }
        	                }
				else
        	                {
        	                        rc = fork();
        	                        if(rc==0)
        	                        {       
						for(j=0;cmd[j]!=NULL;j++)
						{
	                                        	if(strcmp(cmd[j],">")==0 || strcmp(cmd[j],">>")==0)
							{
	                                                        close(1); //close the STDOUT
 	         	                                        if (strcmp(cmd[j],">")==0)
         	                                                       fd = open(cmd[j+1],O_CREAT|O_TRUNC|O_RDWR,0644);
                	                                        else if(strcmp(cmd[j],">>")==0)
                                                                	fd = open(cmd[j+1],O_CREAT|O_APPEND|O_RDWR,0644);
                                                        	if(fd<0)
                                                        	{
                                                                	fprintf(stderr,"Error!\n");
                                                        	}	
                                                        	k=0;
                                                        	while(k!=j)
                                                        	{
                                                                	cmdtrunc[k] = (char*)malloc(sizeof(char)*(strlen(cmd[k])+1));
                                                                	strcpy(cmdtrunc[k],cmd[k]);
                                                                	k++;
                                                        	}
 								cmdtrunc[k] = NULL;
                                                        	//cmdtrunc[k-1][strlen(cmdtrunc[k-1])-1]='\0';
								if(number!=1)
								{
										dup2(pipes[0],0);
                                                       			close(pipes[1]);
									if (pipes[0] != 0)
										close(pipes[0]);

								}
                                                                execvp(cmd[0],cmdtrunc);
                                                                fprintf(stderr,"Error!\n");
     	                                                }

                    	                  	}
                                          	{
							if(number==1)
							{
					                dup2(pipes[1],1);
							close(pipes[0]);
                                                        }
							else if(number==2)
							{
										dup2(pipes[0],0);
                                                       close(pipes[1]);
							if (pipes[0] != 0)
								close(pipes[0]);
				
							}
                                                        execvp(cmd[0],cmd);
                                                        fprintf(stderr,"Error!\n");

                                        	}
                                	}
                                	else
					{
						if(number!=2)
						{
							wait();
						}
						else
						{
									close(pipes[0]);
						close(pipes[1]);                                      	
						wait();
						}
   		                        }
	                        }
}


void twopipe(char line[],int number)
{
 char pwd[BUFSIZE];
        char * cmd[10];
        char * temp;
        char * cmdtrunc[10];
        int i=0,j=0,k=0;
	int rc;
        int fd,fd2;
	temp=NULL;
				i=0;
				temp = strtok (line," ");
                        	while (temp != NULL)
                        	{
                                	cmd[i]=(char *)malloc(sizeof(char)*(strlen(temp)+1));
	                                strcpy(cmd[i],temp);
	                                temp = strtok (NULL, " ");
        	                        i++;
        	                }
        	                cmd[i]=NULL;
				if(number==1||number==2)
        	               	{	
					// cmd[i-1][strlen(cmd[i-1])-1]='\0';
				}
				else
				{
					cmd[i-1][strlen(cmd[i-1])-1]='\0';
				}
        	               if(strcmp(cmd[0],"exit")==0)
        	                {
					if(i==1)                                
						exit(0);
					else
						fprintf(stderr,"Error!\n");				
        	                }
        	                else if(strcmp(cmd[0],"pwd")==0)
        	                {
        	                        getcwd(pwd);
        	 			printf("%s\n",pwd);
        	                }
        	                else if(strcmp(cmd[0],"cd")==0)
        	                {
        	                        if(i==1)
        	                        {
        	                                strcpy(pwd,getenv("HOME"));
        	                                if (chdir (pwd) == -1) {
        	                                        fprintf(stderr,"Error!\n");
        	                                }
        	                        }
        	                        else
        	                        {
        	                                if (chdir (cmd[1]) == -1) {
        	                                        fprintf(stderr,"Error!\n");
        	                                }
        	                        }
        	                }
				else
        	                {
        	                        rc = fork();
        	                        if(rc==0)
        	                        {       
						for(j=0;cmd[j]!=NULL;j++)
						{
	                                        	if(strcmp(cmd[j],">")==0 || strcmp(cmd[j],">>")==0)
							{
	                                                        close(1); //close the STDOUT
 	         	                                        if (strcmp(cmd[j],">")==0)
         	                                                       fd = open(cmd[j+1],O_CREAT|O_TRUNC|O_RDWR,0644);
                	                                        else if(strcmp(cmd[j],">>")==0)
                                                                	fd = open(cmd[j+1],O_CREAT|O_APPEND|O_RDWR,0644);
                                                        	if(fd<0)
                                                        	{
                                                                	fprintf(stderr,"Error!\n");
                                                        	}	
                                                        	k=0;
                                                        	while(k!=j)
                                                        	{
                                                                	cmdtrunc[k] = (char*)malloc(sizeof(char)*(strlen(cmd[k])+1));
                                                                	strcpy(cmdtrunc[k],cmd[k]);
                                                                	k++;
                                                        	}
 								cmdtrunc[k] = NULL;
                                                        	//cmdtrunc[k-1][strlen(cmdtrunc[k-1])-1]='\0';
								if(number==3)
								{
							dup2(pipes2[0],0);
                                                       close(pipes2[1]);
							if (pipes2[0] != 0)
								close(pipes2[0]);

							}
                                                        
                                                                execvp(cmd[0],cmdtrunc);
                                                                fprintf(stderr,"Error!\n");
     	                                                }

                    	                  	}
                                          	{
							if(number==1)
							{
					                 dup2(pipes[1],1);
							close(pipes[0]);
        						}
							else if(number==2)
							{
							dup2(pipes[0],0);
							dup2(pipes2[1], 1);
                                                        close(pipes[1]);
							close(pipes2[0]);
							if (pipes[0] != 0)
								close(pipes[0]);
			
							}
							else
							{
							dup2(pipes2[0],0);
                                                       close(pipes2[1]);
							if (pipes2[0] != 0)
								close(pipes2[0]);

							}
                                                        execvp(cmd[0],cmd);
                                                        fprintf(stderr,"Error!\n");

                                        	}
                                	}
                                	else
					{
						if(number==1)
						{
							wait();
						}
						else if(number==2)
						{
								close(pipes[0]);
						close(pipes[1]);                                      	
						wait();
						}
						else
						{
						close(pipes2[0]);
						close(pipes2[1]);                                      	
						wait();
						}
   		                        }

	                        }
}



void tee(char line[],int number)
{
 char pwd[BUFSIZE];
        char * cmd[10];
        char * temp;
        char * cmdtrunc[10];
        int i=0,j=0,k=0;
	int rc;
        int fd,fd2;
	char *name[2];	
	temp=NULL;
				i=0;
				if(number!=2)
				{
					temp = strtok (line," ");
                        		while (temp != NULL)
                        		{
                                		cmd[i]=(char *)malloc(sizeof(char)*(strlen(temp)+1));
	                                	strcpy(cmd[i],temp);
	                                	temp = strtok (NULL, " ");
        	                        	i++;
        	                	}
        	                	cmd[i]=NULL;
					if(number==1)
        	               		{	
						// cmd[i-1][strlen(cmd[i-1])-1]='\0';
					}
					else
					{
						cmd[i-1][strlen(cmd[i-1])-1]='\0';
					}
				}
        	               if(strcmp(cmd[0],"exit")==0)
        	                {
					if(i==1)                                
						exit(0);
					else
						fprintf(stderr,"Error!\n");				
        	                }
        	                else if(strcmp(cmd[0],"pwd")==0)
        	                {
        	                        getcwd(pwd);
        	 			printf("%s\n",pwd);
        	                }
        	                else if(strcmp(cmd[0],"cd")==0)
        	                {
        	                        if(i==1)
        	                        {
        	                                strcpy(pwd,getenv("HOME"));
        	                                if (chdir (pwd) == -1) {
        	                                        fprintf(stderr,"Error!\n");
        	                                }
        	                        }
        	                        else
        	                        {
        	                                if (chdir (cmd[1]) == -1) {
        	                                        fprintf(stderr,"Error!\n");
        	                                }
        	                        }
        	                }
				else
        	                {
        	                        rc = fork();
        	                        if(rc==0)
        	                        {       
						for(j=0;cmd[j]!=NULL;j++)
						{
	                                        	if(strcmp(cmd[j],">")==0 || strcmp(cmd[j],">>")==0)
							{
	                                                        close(1); //close the STDOUT
 	         	                                        if (strcmp(cmd[j],">")==0)
         	                                                       fd = open(cmd[j+1],O_CREAT|O_TRUNC|O_RDWR,0644);
                	                                        else if(strcmp(cmd[j],">>")==0)
                                                                	fd = open(cmd[j+1],O_CREAT|O_APPEND|O_RDWR,0644);
                                                        	if(fd<0)
                                                        	{
                                                                	fprintf(stderr,"Error!\n");
                                                        	}	
                                                        	k=0;
                                                        	while(k!=j)
                                                        	{
                                                                	cmdtrunc[k] = (char*)malloc(sizeof(char)*(strlen(cmd[k])+1));
                                                                	strcpy(cmdtrunc[k],cmd[k]);
                                                                	k++;
                                                        	}
 								cmdtrunc[k] = NULL;
                                                        	//cmdtrunc[k-1][strlen(cmdtrunc[k-1])-1]='\0';
							/*	if(number!=1)
								{
										dup2(pipes[0],0);
                                                       			close(pipes[1]);
									if (pipes[0] != 0)
										close(pipes[0]);

								}*/
                                                                execvp(cmd[0],cmdtrunc);
                                                                fprintf(stderr,"Error!\n");
     	                                                }

                    	                  	}
                                          	{
							if(number==1)
							{
					                dup2(pipes[1],1);
							close(pipes[0]);
                                                        
                                                        execvp(cmd[0],cmd);
                                                        fprintf(stderr,"Error!\n");
        						}
							else if(number==2)
							{

                        				dup2(pipes[0],0);
                                                       close(pipes[1]);
							if (pipes[0] != 0)
								close(pipes[0]);
							name[0]=malloc(sizeof(char)*(strlen("mytee")+1));
							name[1]=NULL;
						    	strcpy(name[0],"mytee");
							dup2(pipes2[1], 1);
							close(pipes2[0]);
	
							execvp(name[0],name);
                                                        fprintf(stderr,"Error!\n");

							}
							else
							{
                        				dup2(pipes2[0],0);
                                                       close(pipes2[1]);
							if (pipes2[0] != 0)
								close(pipes2[0]);
							execvp(cmd[0],cmd);
                                                        fprintf(stderr,"Error!\n");
							}
                                        	}
                                	}
                                	else
					{
						if(number==1)
						{
							wait();
						}
						else if(number==2)
						{
						close(pipes[0]);
						close(pipes[1]);                                      	
						wait();
						}
						else
						{

						close(pipes2[0]);
						close(pipes2[1]);                                      	
						wait();

						}
   		                        }

	                        }
}



int main()
{
        char line[BUFSIZE];
        int i=0,j=0,k=0;
	int numpipe;
        int len1=0,len2=0,len3=0,index;
	char cmd1[BUFSIZE],cmd2[BUFSIZE],cmd3[BUFSIZE];
	int perr;
	 char *name[3];	
        while(1)
        {
                printf("mysh> ");
                i=0;
                if (fgets(line, sizeof(line), stdin) != NULL)
                {
			/*if(strcmp(line,"\n")==0||line[0]==' ')
			{
				continue;
			}*/
			if(searchtee(line)==1)
			{
				perr = pipe(pipes);
			        if(perr == -1)
			        {
			                fprintf(stderr,"Error!\n");
			                return 1;
			        }
				perr = pipe(pipes2);
       				if(perr == -1)
      				{
                			fprintf(stderr,"Error!\n");
                			return 1;
        			}

				len1=0;
				len2=0;
				for(index=0;line[index]!='%';index++)
					cmd1[len1++]=line[index];
				cmd1[len1]='\0';
				for(index=index+1;line[index]!='%'&&index<strlen(line);index++)
					cmd2[len2++]=line[index];
				cmd2[len2]='\0';
				tee(cmd1,1);
				tee(cmd2,2);
				tee(cmd2,3);
				continue;				
			}
			numpipe=searchpipe(line);
			if(numpipe==0)
			{
				onepipe(line,3);
			}                
			else if(numpipe==1)
			{
				perr = pipe(pipes);
			        if(perr == -1)
			        {
			                fprintf(stderr,"Error!\n");
			                return 1;
			        }
				
				len1=0;
				len2=0;
				for(index=0;line[index]!='|';index++)
					cmd1[len1++]=line[index];
				cmd1[len1]='\0';
				for(index=index+1;line[index]!='|';index++)
					cmd2[len2++]=line[index];
				cmd2[len2]='\0';
				onepipe(cmd1,1);				
				onepipe(cmd2,2);
			}
			else if(numpipe==2)
			{
				perr = pipe(pipes);
        			if(perr == -1)
        			{
                			fprintf(stderr,"Error!\n");
                			return 1;
        			}
				perr = pipe(pipes2);
       				if(perr == -1)
      				{
                			fprintf(stderr,"Error!\n");
                			return 1;
        			}
				len1=0;
				len2=0;
				len3=0;
				for(index=0;line[index]!='|';index++)
					cmd1[len1++]=line[index];
				cmd1[len1]='\0';
				for(index=index+1;line[index]!='|';index++)
					cmd2[len2++]=line[index];
				cmd2[len2]='\0';
				for(index=index+1;line[index]!='|';index++)
					cmd3[len3++]=line[index];
				cmd3[len3]='\0';
				twopipe(cmd1,1);				
				twopipe(cmd2,2);				
				twopipe(cmd3,3);				
			}
		}
        }
        return 1;
}

