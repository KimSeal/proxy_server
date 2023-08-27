///////////////////////////////////////////////////////////////////
// File Name : proxy_cache.c                                     //
// Date      : 2022/03/27                                        //
// Os        : Ubuntu 16.04 LTS 64bits                           //
// Author    : Kim Sang Woo                                      //
// Student ID: 2018202074                                        //
// ------------------------------------------------------------- //
// Title: System Programming Assignment #1-1 (proxy server)      //
// Description:Code to change input values to Hashed URL         //
//             and save them in Cache directory                  //
//    	       according to the conditions                       // 
///////////////////////////////////////////////////////////////////

#include <stdio.h>	//header to use functions like sprintf()
#include <string.h>	//header to use C string functions
#include <openssl/sha.h>//header to do Hashing

#include <sys/types.h>  //for getHomeDir function
#include <unistd.h>	//header to use getuid function
#include <pwd.h>        //header to use getpwuid function

#include <dirent.h>     //header to open Directory
#include <sys/stat.h>   //header to use function about diretory like mkdir, chdir..

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
// main                                                          //
// ------------------------------------------------------------- //
// Input     : int    ->Number of inputs                         //
//	       char * ->String that is inserted at start function//
// Output    : int    ->check program end                        //
// Purpose   : make hashed value according input(call function)  //
//             and make file(named hased value value left char)  //
//             and make directory(named hased 0,1,2 char)        //
//             their save place is cache, and directory have file//
///////////////////////////////////////////////////////////////////
int main(int argc, char *argv[]){
	char s1[1000];//user input
	char s2[41];//hashed value
	char s3[500];//Home's address
	char s4[3]; //directory name from hashed value
	char s5[38];//file name from hashed value
	char cacheaddr[500];//cache directory address
	char makedaddr[500];//hashed value's directory address
	struct dirent *pFile;//struct to check home
	DIR *pDir;	     //to open directory
	getHomeDir(s3);      //take home address
	strcpy(cacheaddr, s3);//make string about cache address
	strcat(cacheaddr, "/cache");

	FILE* fp;		//to make file
	chdir(s3);	//open home directory
	umask(000);	//use every permission
	mkdir("cache", S_IRWXU | S_IRWXG | S_IRWXO);//make cache directory

	while(1){				//infinity loop
		printf("input url> ");		//take user input
		scanf("%s", s1);
		if(strcmp(s1,"bye")==0){	//if input equals "bye", loop ends
			break;
		}
		chdir(cacheaddr);		//move to cache directory		
		sha1_hash(s1,s2);		//change input value to hashed value	
		strncpy(s4,s2,3);		//take directory name from hashed value
		s4[3]='\0';			//make strings end
		mkdir(s4, S_IRWXU | S_IRWXG | S_IRWXO);//make directory
		
		strcpy(makedaddr, cacheaddr);	//make address about new directory
		strcat(makedaddr, "/");
		strcat(makedaddr, s4);

		strcpy(s5, &s2[3]);		//take hashed value that was left 
		chdir(makedaddr);		//change current space to new directory
		fp=fopen(s5,"w");		//make file that named hashed value that was left
		fclose(fp);			//close File
	}
	return 0;
}
