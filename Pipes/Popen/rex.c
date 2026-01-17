#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 1024

int main() {
    FILE *fp;
    char path[MAX_LINE_LENGTH];
    
    const char* command = "ls -l"; 

    /* Open the command for reading. */
    fp = popen(command, "r");
    if (fp == NULL) {
        perror("popen failed");
        exit(EXIT_FAILURE);
    }

    /* Read the output line by line and print it */
    while (fgets(path, sizeof(path), fp) != NULL) {
        printf("%s", path);
    }

    int status = pclose(fp);
    if (status == -1) {
        perror("pclose failed");
        exit(EXIT_FAILURE);
    }

    printf("\nCommand exited with status %d\n", status);

    return 0;
}
