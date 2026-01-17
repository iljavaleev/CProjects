#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    FILE *fp;
    const char *command = "/Users/iljavaleev/Dev/CProjects/Pipes/Popen/proc"; // Simple command that reads from stdin and writes to stdout
    const char *mode = "w";
    const char *data_to_send = "Hello from the parent process!\n";
    int pclose_status;

    /* Open the command for writing */
    fp = popen(command, mode);
    if (fp == NULL) {
        perror("popen failed");
        exit(EXIT_FAILURE);
    }

    printf("Pipe opened successfully. Writing data to the child process (%s)...\n", command);

    /* Write data to the child process's standard input */
    if (fputs(data_to_send, fp) == EOF) {
        perror("fputs error");
        pclose_status = pclose(fp); // Attempt to close even on error
        if (pclose_status == -1) {
            perror("pclose error");
        }
        exit(EXIT_FAILURE);
    }

    printf("Data written successfully. Closing pipe...\n");

    /* Close the pipe and wait for the child process to terminate */
    pclose_status = pclose(fp);
    if (pclose_status == -1) {
        perror("pclose error");
        exit(EXIT_FAILURE);
    }

    printf("Pipe closed. Child process returned status: %d\n", pclose_status);

    return EXIT_SUCCESS;
}
