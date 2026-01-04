#ifndef TASKS_FILESYSTEM_H
#define TASKS_FILESYSTEM_H

#include <stdbool.h>

int create_directory(const char *dir_path);
bool remove_directory(const char *dir_path);
long file_get_size(const char *filename);
bool check_file_exists(const char *filename);
bool create_file(const char *file_path, const char *content);
bool remove_file(const char *file_path);

#endif // TASKS_FILESYSTEM_H
