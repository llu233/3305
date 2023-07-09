/*
Program Name: Password Checking Program
Program Purpose: Check characters of password from hackme program with/without forking and execl
Author: Laurence Lu 
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include "checkPassword.h"
#include <string.h>

int main( int argc, char *argv[] )  {
    pid_t pid;
	pid_t parent;
    printf("Program name %s\n", argv[0]);
    
    /* First fork */
    pid = fork();

    if (strcmp(argv[1],"-f") == 0) {
	    if (pid < 0){ //unsuccessful
            printf("fork unsuccessful");
		    exit(1);
        }
        
        if (pid > 0){ //parent process
            /* Print out, what the PID for child 1 is */
            printf("PID 1.0 is %d. Child 1.2 is %d\n", getpid(), pid);
            printf("PID 1.2 is %d. PPID 1.0 is %d\n", pid, getpid());
            parent = getpid();
            wait(NULL);
            
            /* Create child 2 */
            pid = fork();
            if(pid == 0){
                /* Print out, what the PID for child 2 is */
                printf("PID 1.1 is %d. PPID 1.0 is %d\n", getpid(), getppid());
                printf("PID 1.0 is %d. Child 1.1 is %d\n", getppid(),getpid());
                wait(NULL);

                /* Create child 3 */
                pid = fork();
                if(pid == 0){
                    /* Print out, what the PID for child 3 is */
                    printf("PID 1.1 is %d. Child 1.1.1 is %d\n",getppid(), getpid());
                    printf("PID 1.1.1 is %d. PPID 1.1 is %d\n", getpid(), getppid());
			}
		}   
        wait(NULL); 
    }
}
    else{ //test passwords
        int number1,number2,number3;
        char password[4];
        password[3] = '\0';
        for(number1 = 33;number1 <= 126;number1++){
            for(number2 = 33;number2 <= 126;number2++){
                for(number3 = 33;number3 <= 126;number3++){
                    password[0]=number1;
                    password[1]=number2;
                    password[2]=number3;
                    checkPassword(password,0);
                    checkPassword(password,3);
                    checkPassword(password,6);
                    checkPassword(password,9);
                }
            }
        }
        return 0;
    }
    if (strcmp(argv[2],"-p") == 0) {
        wait(NULL);
        execl("./hackme","./hackme", (char*) NULL);
        return -1; //if theres problem error
    }
    else {
    /* hackme not requested, return 0 */
        return 0;
    }
}