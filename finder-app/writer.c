#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <string.h>

int main(int argc, char *argv[]) {
    // Check for correct number of arguments
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <file path> <string>\n", argv[0]);
        syslog(LOG_ERR, "Invalid number of arguments: %d", argc - 1);
        return 1;
    }

    // Extract arguments
    const char *writefile = argv[1];
    const char *writestr = argv[2];

    // Open syslog
    openlog("writer", LOG_PID | LOG_CONS, LOG_USER);

    // Open the file for writing
    FILE *file = fopen(writefile, "w");
    if (file == NULL) {
        fprintf(stderr, "Could not open file %s for writing\n", writefile);
        syslog(LOG_ERR, "Could not open file %s for writing", writefile);
        closelog();
        return 1;
    }

    // Write the string to the file
    if (fputs(writestr, file) == EOF) {
        fprintf(stderr, "Failed to write to file %s\n", writefile);
        syslog(LOG_ERR, "Failed to write to file %s", writefile);
        fclose(file);
        closelog();
        return 1;
    }

    // Log the successful write operation
    syslog(LOG_DEBUG, "Writing %s to %s", writestr, writefile);

    // Close the file
    fclose(file);

    // Close syslog
    closelog();

    return 0;
}
