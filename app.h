#ifndef TASKS_APP_H
#define TASKS_APP_H

#define TASK_DIRECTORY_NAME "tasks"
#define META_FILE_PATH "/meta.bin"

#include <stdbool.h>

bool setup(void);
const char *get_task_directory(void);
char *get_meta_file_path(void);
char *get_current_time(const char *format);
size_t get_longest_line_size(char *content);
void create_hor_line(size_t len);

#endif // TASKS_APP_H
