#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/file.h>

#define FILE_NAME "test.bin"
#define STR_LEN 100

double get_elapsed_time(struct timeval start, struct timeval end) {
    return (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_usec - start.tv_usec) / 1000000.0;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s num_iterations\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int num_iterations = atoi(argv[1]);
    if (num_iterations <= 0) {
        printf("Error: num_iterations must be a positive integer\n");
        exit(EXIT_FAILURE);
    }

    FILE *fp;
    int fd;
    struct timeval start_time, end_time;

    gettimeofday(&start_time, NULL);

    fp = fopen(FILE_NAME, "a");
    if (fp == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    fd = fileno(fp);
    if (fd == -1) {
        perror("Error getting file descriptor");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < num_iterations; i++) {
        if (flock(fd, LOCK_EX) == -1) {
            perror("Error locking file");
            exit(EXIT_FAILURE);
        }

        if (fprintf(fp, "%*.*s\n", STR_LEN, STR_LEN, "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890") < 0) {
            perror("Error writing to file");
            exit(EXIT_FAILURE);
        }

        if (fsync(fd) == -1) {
            perror("Error syncing file");
            exit(EXIT_FAILURE);
        }

        if (flock(fd, LOCK_UN) == -1) {
            perror("Error unlocking file");
            exit(EXIT_FAILURE);
        }
    }

    if (fclose(fp) != 0) {
        perror("Error closing file");
        exit(EXIT_FAILURE);
    }

    gettimeofday(&end_time, NULL);

    printf("Elapsed time: %f seconds\n", get_elapsed_time(start_time, end_time));

    return 0;
}

