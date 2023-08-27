///////////////////////////////////////////////////////////////////
// File Name : client.c                                          //
// Date      : 2022/04/27                                        //
// Os        : Ubuntu 16.04 LTS 64bits                           //
// Author    : Kim Sang Woo                                      //
// Student ID: 2018202074                                        //
// ------------------------------------------------------------- //
// Title: System Programming Assignment #2-1 (proxy server)      //
// Description:Code to send data sub to sub server process       //
//	       this code take inputs from user			 //
///////////////////////////////////////////////////////////////////
#include <stdio.h>	//header to use C funtion
#include <string.h>	//header to use C string functions
#include <stdlib.h>	//header to use more C functions
#include <unistd.h>	//header to use functions like close
#include <sys/types.h>	//header to use types like pid_t
#include <sys/socket.h>	//header to use functino about socket
#include <netinet/in.h> //header to use function like htons,etc

#define BUFFSIZE 1024	//const int BUFFSIZE
#define PORTNO 40000	//const int PORTNUMBER
///////////////////////////////////////////////////////////////////
// main                                                          //
// ------------------------------------------------------------- //
// Output    : int    ->check program end                        //
// Purpose   : take input data from user 			 //
//	       and send them to sub server		         //
///////////////////////////////////////////////////////////////////
int main(){
	int socket_fd, len;			//variable for socket
	struct sockaddr_in server_addr;		//variable to link server
	char haddr[] = "127.0.0.1";		//own ip address
	char buf[BUFFSIZE];			//take or send data
	
	if((socket_fd = socket(PF_INET, SOCK_STREAM, 0))<0){//set socket to connection
		printf("can't create socket\n");	    //error check
		return -1;
	}
	bzero(buf,sizeof(buf));				    //initialize buf to zero
	bzero((char*)&server_addr, sizeof(server_addr));    //initialize server addr to zero
	server_addr.sin_family=AF_INET;			    //use ipv4
	server_addr.sin_addr.s_addr = inet_addr(haddr);	    //use own ip address
	server_addr.sin_port = htons(PORTNO);		    //host to network short type(portnumber)
	if(connect(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr))<0){//connect to server
		printf("can't connect. \n");					      //error check
		return -1;
	}
	write(STDOUT_FILENO, "input url > ",12);		    //print format
	while((len=read(STDIN_FILENO, buf, sizeof(buf)))>0){	    //take input from user by console
		buf[len]=0;					    //last char is null
		if(write(socket_fd, buf, strlen(buf))>0){	    //send to server
			if((len=read(socket_fd, buf, BUFFSIZE))>0){ //read data that send from server
				if(!strncmp(buf,"END",3)){	    //take End signal from server
					bzero(buf, sizeof(buf));    //break for end program
					break;
				}
				write(STDOUT_FILENO,buf,len);	    //print hit or miss signal from server
				bzero(buf, sizeof(buf));	    //buf initialize
			}
		}
		write(STDOUT_FILENO, "\ninput url > ",13);	    //print format
	}
	close(socket_fd);					    //socket close
	exit(0);						    //end of client
	return 0;
}
