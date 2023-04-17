#include <unistd.h>
#include <getopt.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#define ERROR_ARG_MSG "Error: Enter file to lock"
#define UNLOCK_ERR "Error: Can not unlock file"

static int LOCKS = 0;
static pid_t PID;

void kill_handler(int sig) {
    int stat_str_len = snprintf(NULL, 0, "PID %d Succseed locks: %d\n", PID, LOCKS);
    char* stat_str = malloc(stat_str_len+1);
    snprintf(stat_str, stat_str_len+1, "PID %d Succseed locks: %d\n", PID, LOCKS);
    int stat_fd = open("stat", O_WRONLY | O_APPEND | O_CREAT, 0640);
    write(stat_fd, stat_str, stat_str_len);
    free(stat_str);
    exit(0);
}

void lock(char* lck_name, char write_buf[5], int w_buf_size) {
    int lck_fd = -1;
    while (lck_fd == -1) {
        lck_fd = open(lck_name, O_CREAT | O_EXCL | O_RDWR, 0640);
    }
    write(lck_fd, write_buf, w_buf_size);
    close(lck_fd);
}

int unlock(char* lck_name, int w_buf_size) {
    char lck_r_buf[w_buf_size];
    int lck_fd = open(lck_name, O_RDONLY, 0640);
    lseek(lck_fd, 0, SEEK_SET);
    int readed = read(lck_fd, lck_r_buf, w_buf_size);
    int readed_pid = atoi(lck_r_buf);
    if (readed_pid != PID) {
        return -1;
    }
    close(lck_fd);
    int rm_ex = remove(lck_name);
    if (rm_ex != 0) {
        return -1;
    }
    return 0;
}

void mock_read() {
    sleep(1);
}

int main(int argc, char *argv[]) {
    signal(SIGINT, kill_handler);
    char lck_suf[] = ".lck";
    if (argv[1] == NULL) {
        printf(ERROR_ARG_MSG);
        return 1;
    }

    char* to_lock = argv[1];
    char* lck_name = strcat(to_lock, lck_suf);
    PID = getpid();
    while (1) {
        char write_buf[10];
        sprintf(write_buf, "%d", PID);
        int w_buf_size = strlen(write_buf);
        lock(lck_name, write_buf, w_buf_size);
        mock_read();
        int ok = unlock(lck_name, w_buf_size);
        if (ok != 0) {
            printf(UNLOCK_ERR);
            return 1;
        }
        LOCKS++;
    }
    return 0;
}