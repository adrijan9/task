#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <dirent.h>
#include <limits.h>
#include <unistd.h>

#include "app.h"
#include "filesystem.h"

DIR *dir = {0};
static char task_directory[PATH_MAX] = {0};

bool setup(void) {
    char cwd[PATH_MAX] = {0};
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("Error getting current directory");
        return false;
    }

    const char *base = strrchr(cwd, '/');
    base = (base != NULL) ? base + 1 : cwd;

    if (strcmp(base, TASK_DIRECTORY_NAME) == 0) {
        snprintf(task_directory, sizeof(task_directory), "%s", cwd);
    } else if (strcmp(cwd, "/") == 0) {
        snprintf(task_directory, sizeof(task_directory), "/%s", TASK_DIRECTORY_NAME);
    } else {
        snprintf(task_directory, sizeof(task_directory), "%s/%s", cwd, TASK_DIRECTORY_NAME);
    }

    const int res = create_directory(task_directory);
    if (res == -1) {
        fprintf(stderr, "Error tasks directory.\n");
        return false;
    }

    dir = opendir(task_directory);

    if (dir == NULL) {
        perror("Error open directory");
        return false;
    }

    return true;
}

const char *get_task_directory(void) {
    return task_directory;
}

char *get_meta_file_path(void) {
    char buffer[PATH_MAX];
    snprintf(buffer, sizeof(buffer), "%s%s", get_task_directory(), META_FILE_PATH);
    return strdup(buffer);
}

char *get_current_time(const char *format) {
    time_t raw_time;
    char buffer[80];

    time(&raw_time);

    const struct tm *time_info = gmtime(&raw_time);

    if (time_info == NULL) {
        perror("Error creating time");
        return NULL;
    }

    strftime(buffer, sizeof(buffer), format, time_info);

    return strdup(buffer);
}

size_t get_longest_line_size(char *content) {
    char *line_ptr = NULL;
    char *line = strtok_r(content, "\n", &line_ptr);

    size_t size = 0;
    while (line != NULL) {
        size_t len = strlen(line);
        if (len > size) size = len;
        line = strtok_r(NULL, "\n", &line_ptr);
    }

    return size;
}

void create_hor_line(size_t len) {
    printf("\n");

    for (size_t i = 0; i < len; i++) {
        printf("%s", "-");
    }

    printf("\n");
}
