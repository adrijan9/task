#ifndef TASKS_FILESYSTEM_H
#define TASKS_FILESYSTEM_H

#include <stdbool.h>

int fs_create_directory(const char *dir_path);
bool fs_remove_directory(const char *dir_path);
long fs_get_file_size(const char *filename);
bool fs_check_file_exists(const char *filename);
bool fs_create_file(const char *file_path, const char *content);
bool fs_remove_file(const char *file_path);

#endif // TASKS_FILESYSTEM_H
