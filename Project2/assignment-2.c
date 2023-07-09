/*
Program Name: Large Integer Multiplication Program (4 digit)
Program Purpose: Perform large integer multiplication by decomposing the two factors and performing multiplication and addition on the simplified terms
Author: Laurence Lu
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char* argv[]) {
    int port[2];
    pid_t pid;

    if (argc != 3) {    // check for correct number of command line parameters (2 four-digit numbers)
        printf("Enter: %s <number1> <number2>\n", argv[0]);
        exit(1);
    }

    int a = atoi(argv[1]);  // atoi() converts any string that looks like an integer into an integer
    int b = atoi(argv[2]);

    int a1 = a / 100; // partition the two four-digit numbers into four two-digit numbers.
    int a2 = a % 100;
    int b1 = b / 100;
    int b2 = b % 100;

    printf("Your integers are %d %d\n", a, b); // print out numbers imputed 

    // create and check pipe
    if (pipe(port) < 0) {
        perror("pipe error");
        exit(1);
    }

    // create and check fork
    pid = fork();
    if (pid < 0) {
        perror("fork error");
        exit(1);
    }

    // child process from fork() system call
    else if (pid == 0) { 
        while (1) { //infinite loop
            int a1, a2, b1, b2;
            close(port[1]);
            if (read(port[0], &a1, sizeof(a1)) <= 0) { //reads a1,a2,b1,b2 from file descriptor port[0]. If any reads fails, the loop breaks and the child process exits using the exit() system call. 
                break;
            }
            if (read(port[0], &a2, sizeof(a2)) <= 0) {
                break;
            }
            if (read(port[0], &b1, sizeof(b1)) <= 0) {
                break;
            }
            if (read(port[0], &b2, sizeof(b2)) <= 0) {
                break;
            }
            int A = a1 * b1;  //calculations for A,B,C,D to be sent to parent
            int B = a2 * b1;
            int C = a1 * b2;
            int D = a2 * b2;

            if (write(port[1], &A, sizeof(A)) == -1) { 
                perror("write error");
                exit(1);
            }
            /*
            //checks the return value of the write() system call of A,B,C,D to detect any errors. 
            If write() returns -1, print error message. The child process then exits using the exit() system call.
            If all reads succeed, the child process computes the values of A, B, C, and D, and writes them to port[1] using the write() system call
            */
            if (write(port[1], &B, sizeof(B)) == -1) {
                perror("write error");
                exit(1);
            }
            if (write(port[1], &C, sizeof(C)) == -1) {
                perror("write error");
                exit(1);
            }
            if (write(port[1], &D, sizeof(D)) == -1) {
                perror("write error");
                exit(1);
            }
        }
        exit(0);
    } 
    else {  // parent process
        printf("Parent (PID %d): created child (PID %d)\n", getpid(), pid);
       
        int A = a1 * b1;
        printf("\n###\n# Calculating X\n###\n");
        printf("Parent (PID %d): Sending %d to child\n", getpid(), a1);
        printf("Parent (PID %d): Sending %d to child\n", getpid(), b1);
        close(port[1]);
        write(port[1], &a1, sizeof(a1)); //send a1 and b1 to the child process through a pipe
        write(port[1], &b1, sizeof(b1));
        read(port[0], &A, sizeof(A)); //return calculations for A from child process through a pipe
        printf("        Child (PID %d): Received %d from parent\n", pid, a1);
        printf("        Child (PID %d): Received %d from parent\n", pid, b1);
        int X = A * 10000; //calculate X
        printf("        Child (PID %d): Sending %d to parent\n", pid, A);
        printf("Parent (PID %d): Received %d from child\n", getpid(), A);
        
        int B = a2 * b1;
        printf("\n###\n# Calculating Y\n###\n");
        printf("Parent (PID %d): Sending %d to child\n", getpid(), a2);
        printf("Parent (PID %d): Sending %d to child\n", getpid(), b1);
        close(port[1]);
        write(port[1], &a2, sizeof(int)); //send a2 and b1 to the child process through a pipe
        write(port[1], &b1, sizeof(int));
        read(port[0], &B, sizeof(int)); //return calculations for B from child process through a pipe
        printf("        Child (PID %d): Received %d from parent\n", pid, a2);
        printf("        Child (PID %d): Received %d from parent\n", pid, b1);
        printf("        Child (PID %d): Sending %d to parent\n", pid, B);
        printf("Parent (PID %d): Received %d from child\n", getpid(), B);

        int C = a1 * b2;
        printf("Parent (PID %d): Sending %d to child\n", getpid(), a1);
        printf("Parent (PID %d): Sending %d to child\n", getpid(), b2);
        close(port[1]);
        write(port[1], &a1, sizeof(int)); //send a1 and b2 to the child process through a pipe
        write(port[1], &b2, sizeof(int));
        read(port[0], &C, sizeof(int)); //return calculations for C from child process through a pipe
        printf("        Child (PID %d): Received %d from parent\n", pid, a1);
        printf("        Child (PID %d): Received %d from parent\n", pid, b2);
        printf("        Child (PID %d): Sending %d to parent\n", pid, C);
        printf("Parent (PID %d): Received %d from child\n", getpid(), C);
        int Y = (B + C) * 100; //calculate Y

        int D = a2 * b2;
        printf("\n###\n# Calculating Z\n###\n");
        printf("Parent (PID %d): Sending %d to child\n", getpid(), a2);
        printf("Parent (PID %d): Sending %d to child\n", getpid(), b2);
        close(port[1]);
        write(port[1], &a2, sizeof(int)); //send a2 and b2 to the child process through a pipe
        write(port[1], &b2, sizeof(int));
        read(port[0], &D, sizeof(int)); //return calculations for D from child process through a pipe
        printf("        Child (PID %d): Received %d from parent\n", pid, a2);
        printf("        Child (PID %d): Received %d from parent\n", pid, b2);
        printf("        Child (PID %d): Sending %d to parent\n", pid, D);
        printf("Parent (PID %d): Received %d from child\n", getpid(), D);
        int Z = D; //calculate Z

        int answer = X + Y + Z; // Calculate X+Y+Z which should be same as inputed integer multiplication

        printf("\n%d*%d == %d + %d + %d == %d\n",a,b,X,Y,Z,answer); //print inputed four-digit integers,X,Y,Z, and sum to screen
    }
}