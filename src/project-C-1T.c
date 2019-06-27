
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX 1000
#define RED "\x1B[31m"
#define BLU "\x1B[34m"
#define GRN "\x1B[32m"
#define RESET "\x1B[0m"

int main(int argc, char *argv[]) {

    int nbytes, fd[2];

    /* Buffer used to read bytes from the pipe in the parent. */
    char readbuffer[MAX];

    char *args[argc - 1];
    for (int j = 0; j < argc - 1; j++) {
        args[j] = argv[j + 1];
    }
    /* Null is required at the end of agrs by execvp.*/
    args[argc - 1] = (char *)NULL;

    if (pipe(fd) < 0)
        fprintf(stderr, "Call to the pipe function failed\n"), exit(EXIT_FAILURE);

    switch (fork()) {
    case -1:
        /* Fork function failed. */
        fprintf(stderr, "Call to fork function failed\n");
        exit(EXIT_FAILURE);
    case 0: /* child */
        printf("I'm the child " RED "%d" RESET ", my parent is" BLU " %d " RESET " \n", getpid(), getppid());
        printf("I'm the child " RED "%d" RESET ", and i'm executing the following command : " GRN "%s" RESET "\n",
               getpid(), args[0]);

        if (close(fd[0]) == -1)
            fprintf(stderr, "Couldn’t close read end of pipe in child\n"), exit(EXIT_FAILURE);

        if (dup2(fd[1], STDOUT_FILENO) == -1)
            fprintf(stderr, "dup2 in child failed.\n"), exit(EXIT_FAILURE);

        if (close(fd[1]) == -1)
            fprintf(stderr, "Couldn’t close write end of pipe in child\n"), exit(EXIT_FAILURE);

        execvp(args[0], args);
        /* If the following statement is reached, execvp must have failed. */
        fprintf(stderr, "Call to execvp function in child failed.\n");
        exit(EXIT_FAILURE);
    default: /* parent */
        if (close(fd[1]) == -1)
            fprintf(stderr, "Couldn’t close write end of pipe in parent\n"), exit(EXIT_FAILURE);

        printf("I'm the parent " BLU "%d" RESET ", and i'm printing the result of the command \n", getpid());

        while ((nbytes = read(fd[0], readbuffer, sizeof(readbuffer))) != 0) {
            (nbytes < 0) ? fprintf(stderr, "Call to read function in the parent failed\n"),
                exit(EXIT_FAILURE)
                         : write(1, readbuffer, nbytes) == -1
                ? fprintf(stderr, "Call to write function in parent failed\n"),
                exit(EXIT_FAILURE) : fprintf(stdout, "End of the program ! \n");
        }
    }
    return 0;
} /* End of main. */
