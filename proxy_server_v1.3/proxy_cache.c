///////////////////////////////////////////////////////////////////
// File Name : proxy_cache.c                                     //
// Date      : 2022/04/10                                        //
// Os        : Ubuntu 16.04 LTS 64bits                           //
// Author    : Kim Sang Woo                                      //
// Student ID: 2018202074                                        //
// ------------------------------------------------------------- //
// Title: System Programming Assignment #1-3 (proxy server)      //
// Description:Code to control parent process to make		 //
//	       child process to change input values to Hashed URL//
//             and save them in Cache directory                  //
//    	       according to the conditions                       // 
//    	       and make logfile about Hit&Miss			 //
//	       according to program input%outpupt                //
///////////////////////////////////////////////////////////////////


#include <string.h>	//header to use C string functions
#include <openssl/sha.h>//header to do Hashing

#include <sys/types.h>  //header about directory
#include <unistd.h>	//header to use getuid function and use instruction about process
#include <pwd.h>        //header to use getpwuid function

#include <dirent.h>     //header to open Directory
#include <sys/stat.h>   //header to use function about diretory like mkdir, chdir..

#include <time.h>	//header to write hit&miss time and runtime
#include <stdio.h>	//header to use functions like sprintf()

#include <sys/wait.h>   //header to take value about process
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
//	       make logfile about hit&miss log file		 //
//	       (logfile needs time value)			 //
//	       they are executed in child process 		 //
//	       that is maked by main process			 //
///////////////////////////////////////////////////////////////////
int main(int argc, char * argv[]){
	char s1[500];			//user input
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
	
	char maininput[500]; 		// main process input
	pid_t pid;			//to control processes
	int status;			//to use waitpid instruction
	time_t mainstart, mainend;	//to check main process runtime
	mainstart=time(NULL);		//start time of main process
	int numsub=0;			//number of sub process
	int mainruntime;		//take main process runtime

	chdir(s3);			//open home directory
	umask(000);			//use every permission

	mkdir("cache", S_IRWXU | S_IRWXG | S_IRWXO);	//make cache directory
	mkdir("logfile", S_IRWXU | S_IRWXG | S_IRWXO);	//make cache directory

	chdir(logaddr);				//go to logfile directory
	fp2=fopen("logfile.txt","a+");		//make log file to write program information
	while(1){
		printf("[%d]input CMD> ",getpid());
		scanf("%s", maininput);
		if(strcmp(maininput, "connect")==0){
			numsub++;
			if((pid=fork()) < 0){
				fprintf(fp2,"fork error");
				break;
			}
			else if(pid==0){
			start=time(NULL);		//check start time
			while(1){				//infinity loop
				printf("[%d]input url> ",getpid());		//take user input
				scanf("%s", s1);
				if(strcmp(s1,"bye")==0){	//if input equals "bye", loop ends
					break;
				}
				chdir(cacheaddr);		//move to cache directory		
				sha1_hash(s1,s2);		//change input value to hashed value
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
								fprintf(fp2,"[Hit]%s/%s-[%02d/%02d/%02d, %02d:%02d:%02d]\n"
									,s4,s5,1900+ltp->tm_year,1+ltp->tm_mon,ltp->tm_mday,
									ltp->tm_hour,ltp->tm_min,ltp->tm_sec);
									//write hit data in logfile(hashdata & time)	
	
								fprintf(fp2,"[Hit]%s\n",s1);	//write hit data in logfile about input	
								hitnum+=1;			//add 1 to hit number
								hitcheck=1;			//check hit
								break;
							}
						}
						closedir(pDir2);			//close compared directory
					}
				}

				if(hitcheck==0){					//miss timing(no hit)
					fprintf(fp2,"[Miss]%s-[%02d/%02d/%02d, %02d:%02d:%02d]\n",s1,1900+ltp->tm_year,
					1+ltp->tm_mon,ltp->tm_mday,ltp->tm_hour,ltp->tm_min,ltp->tm_sec);				
					//write miss data in logfile about input and time
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
			end=time(NULL);							//check end time
			int runtime = end-start;					//make run time
			fprintf(fp2,"[Terminated] run time: %d sec. #request hit : %d, miss : %d\n",runtime,hitnum,missnum);
			//write data about run time, hit number, miss number in log file
			fclose(fp2);							//close logfile	
			closedir(pDir);							//close directory 				
			return 0;							//end of child process
			}
			if((pid=waitpid(pid, &status,0))<0){				//take end of chile process and return to main process
				printf("waitpid error\n");
			}
		}	
		else if(strcmp(maininput,"quit")==0){
			closedir(pDir);							//close directory 
			mainend=time(NULL);						//main process's end time
			mainruntime=mainend-mainstart;					//value of main process runtime
			fprintf(fp2,"**SERVER** [Terminated] run time: %d sec. #sub process: %d\n",mainruntime,numsub);	//write data in txt file according to format
			fclose(fp2);							//close logfile		
			break;								//end of main process loop
		}
	}
	
	return 0;									//program end
}
