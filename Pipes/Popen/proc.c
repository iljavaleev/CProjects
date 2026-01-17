#include <stdio.h>

#define MAX_LINE_LENGTH 100

int main() {
    char line[MAX_LINE_LENGTH];
    // Read at most MAX_LINE_LENGTH-1 characters from stdin
    if (fgets(line, sizeof(line), stdin) != NULL) {
        printf("The string is: %s", line);
    } else {
        perror("Error reading input");
    }

    return 0;
}