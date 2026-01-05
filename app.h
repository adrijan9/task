#ifndef TASKS_APP_H
#define TASKS_APP_H

#define TASK_DIRECTORY_NAME "tasks"
#define META_FILE_PATH "/meta.bin"

#include <stdbool.h>

bool aop_setup(void);
const char *app_get_task_directory(void);
char *app_get_meta_file_path(void);
char *app_get_current_time(const char *format);
size_t app_get_longest_line_size(char *content);
void app_create_hor_line(size_t len);

#endif // TASKS_APP_H
