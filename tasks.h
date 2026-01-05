#ifndef TASKS_TASKS_H
#define TASKS_TASKS_H

#define TASKS_CAPACITY 30
#define ID_SIZE 15
#define TITLE_SIZE 256
#define FILE_PATH_SIZE 256

typedef struct {
    char id[ID_SIZE];
    char title[TITLE_SIZE];
    char file_path[FILE_PATH_SIZE];
} Task;

void task_load(void);
void task_show(void);
int task_create(char *title);
int task_delete(const char *task_id);
int task_preview(const char *task_id);

#endif // TASKS_TASKS_H
