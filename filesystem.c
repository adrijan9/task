#include <stdbool.h>
#include <stdio.h>
#include <sys/errno.h>
#include <sys/stat.h>
#include <unistd.h>

int fs_create_directory(const char *dir_path) {
    const int status = mkdir(dir_path, 0755);

    if (status == -1) {
        // Check errno for the specific error
        if (errno == EEXIST) {
            return -2;
        }
        return -1;
    }

    return 0;
}

bool fs_remove_directory(const char *dir_path) {
    if (rmdir(dir_path) == -1) {
        perror("Error removing directory");
        return false;
    }

    return true;
}

long fs_get_file_size(const char *filename) {
    struct stat st;
    if (stat(filename, &st) != 0) {
        return -1;
    }
    return st.st_size;
}

bool fs_check_file_exists(const char *filename) {
    return fs_get_file_size(filename) >= 0;
}

bool fs_create_file(const char *file_path, const char *content) {
    FILE *file = fopen(file_path, "w");

    if (file == NULL) {
        printf("-> %s\n", file_path);
        perror("Error creating file");
        return false;
    }

    if (content != NULL) fprintf(file, "%s", content);

    fclose(file);

    return true;
}

bool fs_remove_file(const char *file_path) {
    if (unlink(file_path) == -1) {
        perror("Error removing file");
        return false;
    }

    return true;
}
