///////////////////////////////////////////////////////////////////
// File Name : proxy_cache.c                                     //
// Date      : 2022/05/03                                        //
// Os        : Ubuntu 16.04 LTS 64bits                           //
// Author    : Kim Sang Woo                                      //
// Student ID: 2018202074                                        //
// ------------------------------------------------------------- //
// Title: System Programming Assignment #2-3 (proxy server)      //
// Description:Code to take web browser url from html		 //
//	       and change URL to Hashed URL  	                 //
//             and save them in Cache directory                  //
//    	       according to the conditions                       // 
//    	       and make logfile about Hit&Miss			 //
//	       according to program input%outpupt                //
//	       they are executing by Parent process 	         //
//	       that make Child process 			 	 //	
//	       if miss execution happens, web browser will take  //
//	       (check for read message from web server)		 //
//	       if webserver don't receive HTTP response		 //
//	       during 10 seconds after receive HTTP request,     //
//	       stop alarm and print No Response			 //
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

#include <sys/socket.h> //header to use socket function
#include <netinet/in.h>	//header to use htonl, htons, etc(like inet_addr)
#include <stdlib.h>	//header to use more C functions
#include <signal.h>	//header to take signal
#include <sys/wait.h>	//header to wait sub process
#include <arpa/inet.h> //header to use inet_ntoa funtion

#define BUFFSIZE 1024	//const int BUFFSIZE
#define PORTNO 39999	//const int PORTNO

#include <netdb.h>	//header to use gethostbyname function
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
// alarmHandler                                                  //
// ------------------------------------------------------------- //
// Purpose   : Function to handle alarm signal system            //
//             about response from web during 10 seconds over    //
//	       to kill child process                             //
///////////////////////////////////////////////////////////////////
static void alarmHandler(){			//alarm function
	printf("========No Response========\n");//print about no response			
	exit(0);//kill current process(about child process)
}
///////////////////////////////////////////////////////////////////
// main                                                          //
// ------------------------------------------------------------- //
// Input     : int    ->Number of inputs                         //
//	       char * ->String that is inserted at start function//
// Output    : int    ->check program end                        //
// Purpose   : make hashed value according input(browser URL)    //
//             and make file(named hased value value left char)  //
//             and make directory(named hased 0,1,2 char)        //
//             their save place is cache, and directory have file//
//	       make logfile about hit&miss log file		 //
//	       (logfile needs time value)			 //
//	       web browser take URL,school ID and hit&miss stauts//
//	       they will execute in child process		 //
//	       Parent process will make Sub server process  	 //
//	       to make child process				 //
//	       if web responce time is over 10seconds,		 //
//	       kill child process about that url		 //
//	       and put message to web browser after execution    //
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

	struct sockaddr_in server_addr, client_addr;//variable of sockcet
	int socket_fd, client_fd;		    //taked socket funtion to connection
	int len, len_out;			    //size of client addr			    
	char buf[BUFFSIZE];			    //buf about read&write data
	pid_t pid;				    //take pid

	if((socket_fd=socket(PF_INET, SOCK_STREAM, 0))<0){//make socket and error check
		printf("Server:Can't open stream socket.");
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
		signal(SIGALRM, alarmHandler);					//signal about check web responce time
	while(1){					//wait Executing main program by accept
	struct in_addr inet_client_address;		//variable to take data about web browser
	char buf[BUFFSIZE];				//string about web brower information
	char response_header[BUFFSIZE]={0, };		//string about web browser imformation that send to web browser
	char response_message[BUFFSIZE]={0, };		//string about web browser data that send to web browser
	char tmp[BUFFSIZE] ={0, };			//String to copy buf
	char method[20] = {0, };			//check GET
	char url[BUFFSIZE] = {0, };			//string about URL
	char * tok = NULL;				//string to cut string
	
	len = sizeof(client_addr);			//take size of client_addr
	client_fd = accept(socket_fd,(struct sockaddr*)&client_addr, &len);//accept from sub server
	if(client_fd < 0){						   //error check
		printf("Server : accept failed %d\n",getpid());
		close(socket_fd);
		return 0;
	}
	
	pid = fork();//make sub process

	if(pid == -1){		 //if pid=-1
		close(client_fd);//close them
		close(socket_fd);
		return -1;//error
	}

	if(pid == 0){//in sub process			
		inet_client_address.s_addr= client_addr.sin_addr.s_addr;	//set client
		len_out=read(client_fd, buf, BUFFSIZE);				//and take data that web browser sent
		if(len_out==-1){
			printf("========browser Response error========\n");//print about no response
			exit(0);
		}
		strcpy(tmp, buf);								      //take method from tmp(buf)
		tok = strtok(tmp, " ");								      
		strcpy(method, tok);
		if(strcmp(method, "GET")==0){								//method check
			printf("[%s : %d] client was connected\n", inet_ntoa(inet_client_address), client_addr.sin_port);
			//string about connect
			puts("======================================================");
			printf("Request from [%s:%d]\n",inet_ntoa(inet_client_address), client_addr.sin_port);//string about request
			puts(buf);									    //data that web browser sent
			puts("======================================================");
			tok = strtok(NULL, " ");							      //take URL
			strcpy(url, tok);

			char httpchk[8];			//variable to check http://
			char * domainurl;			//variable to save url that  deleted http://
			strncpy(httpchk, tok, 7);		
			httpchk[7]='\0';			//put null at end of string
			if(strcmp(httpchk, "http://")==0){	//check http://
				strcpy(url, tok);		//delete http:// by using shift
				int i;
				for (i = 7; tok[i]; i++)
    				{
        				url[i - 7] = tok[i]; //
    				}
				url[i-1]='\0';
				domainurl=strtok(url, "/");
			}
			else{exit(0);}				//if address is no http, stop process
			struct hostent *hent;			//struct to take domain and change domain to ip
			char * haddr;
			//int lenex = strlen(domainurl);
			hent = (struct hostent*)gethostbyname(domainurl);
			//take ip from domain
			if(hent!=NULL){
				haddr = inet_ntoa(*((struct in_addr*)hent->h_addr_list[0]));
				//take ip from domain
			}
			else{
				exit(0);			//if domain dont have ip, stop process
			}
		chdir(cacheaddr);		//move to cache directory		
		sha1_hash(tok,s2);		//change input value to hashed value
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
					if(0==strcmp(s5,pFile2->d_name)){		//compare input's data(from front 3 letters) and files
						write(client_fd,"HIT",3);			//send HIT message to client
						fprintf(fp2,"[Hit] ServerPID : %d | %s/%s - [%02d/%02d/%02d, %02d:%02d:%02d]\n"
							,getpid(),s4,s5,1900+ltp->tm_year,1+ltp->tm_mon,ltp->tm_mday,
							ltp->tm_hour,ltp->tm_min,ltp->tm_sec);
							//write hit data in logfile(hashdata & time)	
	
						fprintf(fp2,"[Hit]%s\n",url);	//write hit data in logfile about input
						fflush(fp2);			//fflush for write logfile real time
						hitnum+=1;			//add 1 to hit number
						sprintf(response_message, "<h1>HIT</h1><br>""%s:%d<br>""%s<br>""kw2018202074",
						inet_ntoa(inet_client_address), client_addr.sin_port, url);//write message to send browser
						sprintf(response_header,"HTTP/1.0 200 OK\r\n""Server:2018 simple web server\r\n"
						"Content-length:%lu\r\n""Content-type:text/html\r\n\r\n",strlen(response_message));
						//write format to send browser
						write(client_fd, response_header, strlen(response_header));//send message to browser
						write(client_fd, response_message, strlen(response_message));//send header to browser
						hitcheck=1;			//check hit
						break;
					}
				}
				closedir(pDir2);			//close compared directory
			}
		}

		if(hitcheck==0){					//miss timing(no hit)		
			int socket_fd2, len2;					//variable for socket about web server
			struct sockaddr_in server_addr2;		        //variable to link server
			char subbuf[BUFFSIZE];					//take data from web server
		
			if((socket_fd2 = socket(PF_INET, SOCK_STREAM, 0))<0){//set socket to connection
				printf("can't create socket\n");	    //error check
				exit(0);
			}
			bzero(subbuf,sizeof(subbuf));			      //initialize subbuf to zero
			bzero((char*)&server_addr2, sizeof(server_addr2));    //initialize server addr2 to zero
						
			server_addr2.sin_family=AF_INET;			    //use ipv4
			//server_addr2.sin_addr.s_addr = inet_addr(haddr);	    //use own ip address about ipv4
			inet_pton(AF_INET,haddr,&server_addr2.sin_addr);	    //addr input to connect web-server
			server_addr2.sin_port = htons(80);		    //put portnumberto connect web server

			if(connect(socket_fd2, (struct sockaddr*)&server_addr2, sizeof(server_addr2) ) < 0){  //connect to server
				printf("can't connect. \n");					              //error check
				exit(0);
			}
			write(socket_fd2, buf, strlen(buf));
			alarm(10);					//start alarm after send request
			if(len2=read(socket_fd2, subbuf, BUFFSIZE)<0){				//and take data from web-server
				printf("========Read Error========\n");//print about no response
				exit(0);
			}
			subbuf[BUFFSIZE]='\0';							//put null to subbuf
			alarm(0);								//stop alarm
			write(client_fd,subbuf,strlen(subbuf));			//send message to web browser about web server
			fprintf(fp2,"[Miss] ServerPID : %d | %s - [%02d/%02d/%02d, %02d:%02d:%02d]\n",getpid(),url,1900+ltp->tm_year,
			1+ltp->tm_mon,ltp->tm_mday,ltp->tm_hour,ltp->tm_min,ltp->tm_sec);				
			//write miss data in logfile about input and time
			fflush(fp2);					//fflush for write logfile real time
			missnum+=1;					//add 1 to miss number
			/*			
			sprintf(response_message,"<h1>MISS</h1><br>""%s:%d<br>""%s<br>""kw2018202074",
			inet_ntoa(inet_client_address), client_addr.sin_port, url);
			//make message to send browser
			sprintf(response_header,"HTTP/1.0 200 OK\r\n""Server:2018 simple web server\r\n"
			"Content-length:%lu\r\n""Content-type:text/html\r\n\r\n",strlen(response_message));
			//write format to send browser
			write(client_fd, response_header, strlen(response_header));	//send message to browser
			write(client_fd, response_message, strlen(response_message));	//send header to browser
			*/
			if(filecheck==0){				//input is miss but cache have directory 
									//that named input's front 3letters
				mkdir(s4, S_IRWXU | S_IRWXG | S_IRWXO);		//make directory
			}

			strcpy(makedaddr, cacheaddr);			//make address about new directory
			strcat(makedaddr, "/");
			strcat(makedaddr, s4);
	
			chdir(makedaddr);				//change current space to new directory
			fp=fopen(s5,"w");				//make file that named hashed value that was left
			fclose(fp);					//close File
		}
							//if process take response data from web, end alarm signal.
	printf("[%s : %d] client was disconnected\n", inet_ntoa(inet_client_address), client_addr.sin_port);
	
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
	}
	close(client_fd);
	}
	close(socket_fd);	

	return 0;
}
