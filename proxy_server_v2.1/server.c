///////////////////////////////////////////////////////////////////
// File Name : server.c                                          //
// Date      : 2022/04/27                                        //
// Os        : Ubuntu 16.04 LTS 64bits                           //
// Author    : Kim Sang Woo                                      //
// Student ID: 2018202074                                        //
// ------------------------------------------------------------- //
// Title: System Programming Assignment #2-1 (proxy server)      //
// Description:Code to change input values to Hashed URL         //
//             and save them in Cache directory                  //
//    	       according to the conditions                       // 
//    	       and make logfile about Hit&Miss			 //
//	       according to program input%outpupt                //
//	       they are executing by main server process         //
//	       that make sub server process 			 //
///////////////////////////////////////////////////////////////////

#include <stdio.h>	//header to use functions like sprintf()
#include <string.h>	//header to use C string functions
#include <openssl/sha.h>//header to do Hashing

#include <sys/types.h>  //for getHomeDir function
#include <unistd.h>	//header to use functions like getuid function
#include <pwd.h>        //header to use getpwuid function

#include <dirent.h>     //header to open Directory
#include <sys/stat.h>   //header to use function about diretory like mkdir, chdir..

#include <time.h>	//header to write hit&miss time and runtime

//////////////2-1 add header
#include <sys/socket.h> //header to use socket function
#include <netinet/in.h>	//header to use htonl, htons, etc(like inet_addr)
#include <stdlib.h>	//header to use more C functions
#include <signal.h>	//header to take signal
#include <sys/wait.h>	//header to wait sub process

#define BUFFSIZE 1024	//const int BUFFSIZE
#define PORTNO 40000	//const int PORTNO

///////////////////////////////////////////////////////////////////
// getHomeDir                                                    //
// ------------------------------------------------------------- //
// Input     : char * ->Insert String to save home address       //
// Output    : char * ->address about home                       //
// Purpose   : Find address about home(using pwd.h, unistd.h)    //
//             and return address to Inserted string             //
///////////////////////////////////////////////////////////////////

char *getHomeDir(char *home){	//return home directory adderss
	struct passwd *usr_info = getpwuid(getuid()); //use getuid to find home address
	strcpy(home, usr_info->pw_dir);               //input home address

	return home;
}
///////////////////////////////////////////////////////////////////
// sha1_hash                                                     //
// ------------------------------------------------------------- //
// Input     : char * ->Insert String that will change to hashed //
//                      value                                    //
//	       char * ->Insert String to save hashed value       //
// Output    : char * ->hashed value that was Input              //
// Purpose   : Change input value to hashed value                //
//             to check file's name and directory's name.        //
//	       (they will made according hashed value            //
///////////////////////////////////////////////////////////////////
char *sha1_hash(char *input_url, char * hashed_url){                 //function to hashing
	unsigned char hashed_160bits[20];		             //160bit hashed value
	char hashed_hex[41];					     //function output value
	int i;							     //for loop

	SHA1(input_url, strlen(input_url), hashed_160bits);	     //hashing function

	for(i=0;i<sizeof(hashed_160bits);i++)			     //change format about hashed value
		sprintf(hashed_hex + i*2, "%02x", hashed_160bits[i]);

	strcpy(hashed_url, hashed_hex);				     //copy value to return

	return hashed_url;					     //return
}
///////////////////////////////////////////////////////////////////
// handler                                                       //
// ------------------------------------------------------------- //
// Purpose   : Function to handle multi process system           //
//             wait next pid return                              //
//	       until no more process return                      //
///////////////////////////////////////////////////////////////////
static void handler(){			
	pid_t pid; //pid check
	int status;//child process's end status
	while((pid = waitpid(-1, &status, WNOHANG))>0);	
	//take retun 0 until process return 
}
///////////////////////////////////////////////////////////////////
// main                                                          //
// ------------------------------------------------------------- //
// Input     : int    ->Number of inputs                         //
//	       char * ->String that is inserted at start function//
// Output    : int    ->check program end                        //
// Purpose   : make hashed value according input(call function)  //
//             and make file(named hased value value left char)  //
//             and make directory(named hased 0,1,2 char)        //
//             their save place is cache, and directory have file//
//	       make logfile about hit&miss log file		 //
//	       (logfile needs time value)			 //
//	       they will execute in sub server process		 //
//	       mainserver process will make Sub server process   //
//	       to make sub process				 //
///////////////////////////////////////////////////////////////////
int main(int argc, char * argv[]){
	char s2[41];			//hashed value
	char s3[500];			//Home's address
	char s4[3]; 			//directory name from hashed value
	char s5[38];			//file name from hashed value
	char cacheaddr[500];		//cache directory address
	char makedaddr[500];		//hashed value's directory address

	char logaddr[500];		//logfile directory address
	char comaddr[500];		//compared directory address to check hit&miss

	struct dirent *pFile;		//struct to check home
	struct dirent *pFile2;		//struct to write logfile
	DIR *pDir;	     		//to open directory
	DIR *pDir2;	   		//to read file

	int hitcheck;			//value to check hit
	int hitnum=0;			//number of hit
	int missnum=0;			//number of miss
	int filecheck=0;		//value to check same filename

	time_t now;			//time_t value to check current time
	struct tm *ltp;			//struct to check current time
	time(&now);			//take calender time
	ltp = localtime(&now);		//use localtime to convert time_t to struct tm

	getHomeDir(s3);   		//take home address
	strcpy(cacheaddr, s3);		//make string about cache address
	strcat(cacheaddr, "/cache");	

	strcpy(logaddr, s3);		//make string about logfile address
	strcat(logaddr, "/logfile");

	FILE* fp;			//to make file
	FILE* fp2;			//to make and write logfile
	
	time_t start, end;		//value to check start&end timing
	start=time(NULL);		//check start time

	chdir(s3);			//open home directory
	umask(000);			//use every permission

	mkdir("cache", S_IRWXU | S_IRWXG | S_IRWXO);	//make cache directory
	mkdir("logfile", S_IRWXU | S_IRWXG | S_IRWXO);	//make cache directory

	chdir(logaddr);				//go to logfile directory
	fp2=fopen("logfile.txt","a+");		//make file that named hashed value that was left
///////////2-1 add
	struct sockaddr_in server_addr, client_addr;//variable of sockcet
	int socket_fd, client_fd;		    //taked socket funtion to connection
	int len, len_out;			    //size of client addr			    
	char buf[BUFFSIZE];			    //buf about read&write data
	pid_t pid;				    //take pid

	if((socket_fd=socket(PF_INET, SOCK_STREAM, 0))<0){//make socket and error check
		printf("Server:Can't open stram socket.");
		return 0;
	}
	bzero((char *)&server_addr, sizeof(server_addr));//initialize sockaddr_in about server
	server_addr.sin_family = AF_INET;		 //ipv4
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY); //host to network long type
	server_addr.sin_port = htons(PORTNO);		 //host to network short type(portnumber)
	if(bind(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr))<0){//bind socket& sockaddr_in information
		printf("Server : Can't bind local address\n");//error check
		close(socket_fd);
		return 0;
	}
	listen(socket_fd, 30);				//wait process accept
	signal(SIGCHLD,(void *)handler);		//signal about child process end
	while(1){
	bzero((char*)&client_addr, sizeof(client_addr));//initialize sockaddr_in about server
	len = sizeof(client_addr);			//take size of client_addr
	client_fd = accept(socket_fd,(struct sockaddr*)&client_addr, &len);//accept from sub server
	if(client_fd < 0){						   //error check
		printf("Server : accept failed %d\n",getpid());
		close(socket_fd);
		return 0;
	}
	printf("[%d : %d] client was connected\n", client_addr.sin_addr.s_addr, client_addr.sin_port);
	//print client connect in console
	pid = fork();//make sub process

	if(pid == -1){		 //if pid=-1
		close(client_fd);//close them
		close(socket_fd);
		continue;	 //and take again
	}
		
	if(pid == 0){//in sub process
	while((len_out=read(client_fd, buf, BUFFSIZE))>0){				//infinity loop
		char * s1=strtok(buf,"\n");
		if(!strcmp(buf,"bye")){	//if input equals "bye", loop ends
			write(client_fd,"END",3);
			break;
		}
		chdir(cacheaddr);		//move to cache directory		
		sha1_hash(buf,s2);		//change input value to hashed value
		strncpy(s4,s2,3);		//take directory name from hashed value
		s4[3]='\0';			//make strings end
		
		strcpy(s5, &s2[3]);		//take hashed value that was left 

		pDir=opendir(cacheaddr);	//open cache
		if(pDir == NULL){		//error check about cache
			printf("Dir read error\n");
			return 1;
		}
		hitcheck=0;			//reset hit check
		filecheck=0;			//reset filecheck
		for(pFile=readdir(pDir); pFile; pFile=readdir(pDir)){	//read every directory name in cache
			if(strcmp(s4,pFile->d_name)==0){		//compare input's front 3 letters and files
				filecheck=1;
				strcpy(comaddr, cacheaddr);		//make address about compared directory
				strcat(comaddr, "/");
				strcat(comaddr, s4);
				pDir2=opendir(comaddr);			//open compared address
				for(pFile2=readdir(pDir2);pFile2;pFile2=readdir(pDir2)){		//read file name in compared directory
					if(0==strcmp(s5,pFile2->d_name)){			//compare input's data(from front 3 letters) and files
						write(client_fd,"HIT",3);			//send HIT message to client
						fprintf(fp2,"[Hit] ServerPID : %d | %s/%s - [%02d/%02d/%02d, %02d:%02d:%02d]\n"
							,getpid(),s4,s5,1900+ltp->tm_year,1+ltp->tm_mon,ltp->tm_mday,
							ltp->tm_hour,ltp->tm_min,ltp->tm_sec);
							//write hit data in logfile(hashdata & time)	
	
						fprintf(fp2,"[Hit]%s\n",buf);	//write hit data in logfile about input
						fflush(fp2);			//fflush for write logfile real time
						hitnum+=1;			//add 1 to hit number
						hitcheck=1;			//check hit
						break;
					}
				}
				closedir(pDir2);			//close compared directory
			}
		}

		if(hitcheck==0){					//miss timing(no hit)			
			write(client_fd,"MISS",4);			//send MISS message to client
			fprintf(fp2,"[Miss] ServerPID : %d | %s - [%02d/%02d/%02d, %02d:%02d:%02d]\n",getpid(),buf,1900+ltp->tm_year,
			1+ltp->tm_mon,ltp->tm_mday,ltp->tm_hour,ltp->tm_min,ltp->tm_sec);				
			//write miss data in logfile about input and time
			fflush(fp2);					//fflush for write logfile real time
			missnum+=1;					//add 1 to miss number
			if(filecheck==0){				//input is miss but cache have directory that named input's front 3letters
				mkdir(s4, S_IRWXU | S_IRWXG | S_IRWXO);		//make directory
			}

			strcpy(makedaddr, cacheaddr);			//make address about new directory
			strcat(makedaddr, "/");
			strcat(makedaddr, s4);
	
			chdir(makedaddr);				//change current space to new directory
			fp=fopen(s5,"w");				//make file that named hashed value that was left
			fclose(fp);					//close File
		}
	}
	printf("[%d : %d] client was disconnected\n", client_addr.sin_addr.s_addr, client_addr.sin_port);
	
	end=time(NULL);							//check end time
	int runtime = end-start;					//make run time
	fprintf(fp2,"[Terminated] ServerPID : %d | run time: %d sec. #request hit : %d, miss : %d\n",getpid(),runtime,hitnum,missnum);
	//write data about run time, hit number, miss number in log file
	fflush(fp2);							//fflush for write logfile real time
	fclose(fp2);							//close logfile
	closedir(pDir);							//close directory 
	close(client_fd);
	exit(0);
	}
	close(client_fd);
	}
	close(socket_fd);	

	return 0;
}
