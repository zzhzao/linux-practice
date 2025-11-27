#include <stdio.h>
#include <unistd.h>

int main()
{
    pid_t id = fork();
    if (id == 0)
    {
        while (1)
        {
            printf("a");
            fflush(stdout);
            printf("a");
            fflush(stdout);
            printf("\n");
            usleep(100);
        }
    }
    while (1)
    {
        printf("b");
        fflush(stdout);
        printf("b");
        fflush(stdout);
        printf("\n");
        usleep(100);
    }
}
