#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#define MAX_CHARS 200
int main()
{
    printf("welcome to my own shell");
    int two=2;
    while (two--)
    {
        char *input[MAX_CHARS];
        printf("custom $ prompt");
        // gets(input);
        int status;
        int value = fork();
        if (value != 0)
        {
            /*this means this is block from parent process*/
            waitpid(-1, &status, 0); // system call
        }
        else
        {
            /*we are in the child forked process*/
            execlp("echo", "README.md"); // system call
        }
    }
}
