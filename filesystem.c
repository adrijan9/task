#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#ifdef _WIN32
    #include <direct.h>
    #include <io.h>
    #define stat _stat
#else
    #include <unistd.h>
#endif

static int fs_mkdir(const char *dir_path) {
#ifdef _WIN32
    return _mkdir(dir_path);
#else
    return mkdir(dir_path, 0755);
#endif
}

static int fs_rmdir(const char *dir_path) {
#ifdef _WIN32
    return _rmdir(dir_path);
#else
    return rmdir(dir_path);
#endif
}

static int fs_unlink(const char *file_path) {
#ifdef _WIN32
    return _unlink(file_path);
#else
    return unlink(file_path);
#endif
}

static bool fs_is_dir(const char *path) {
    struct stat st;
    if (stat(path, &st) != 0) return false;
#ifdef _WIN32
    return (st.st_mode & _S_IFDIR) != 0;
#else
    return S_ISDIR(st.st_mode);
#endif
}

int fs_create_directory(const char *dir_path) {
    if (fs_mkdir(dir_path) == 0) return 0;

    if (errno == EEXIST && fs_is_dir(dir_path)) {
        return -2; // already exists (as directory)
    }

    return -1; // other error
}

bool fs_remove_directory(const char *dir_path) {
    return fs_rmdir(dir_path) == 0;
}

int64_t fs_get_file_size(const char *filename) {
    struct stat st;
    if (stat(filename, &st) != 0) return -1;
    return st.st_size;
}

bool fs_file_exists(const char *filename) {
    struct stat st;
    return stat(filename, &st) == 0;
}

bool fs_create_file(const char *file_path, const char *content) {
    FILE *file = fopen(file_path, "wb"); // exact bytes across platforms
    if (!file) return false;

    if (content) {
        if (fwrite(content, 1, strlen(content), file) != strlen(content)) {
            fclose(file);
            return false;
        }
    }

    return fclose(file) == 0;
}

bool fs_remove_file(const char *file_path) {
    return fs_unlink(file_path) == 0;
}
