#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "app.h"
#include "filesystem.h"
#include "tasks.h"

Task *tasks;
size_t tasks_count = 0;
size_t task_capacity = 0;

void realloc_tasks(void) {
    if (tasks == NULL) {
        tasks = malloc(sizeof(Task) * TASKS_CAPACITY);
    }

    if (tasks_count >= task_capacity) {
        task_capacity = task_capacity * 2;
        tasks = realloc(tasks, sizeof(Task) * task_capacity);
    }
}

void destroy_tasks(void) {
    if (tasks == NULL) return;

    free(tasks);
}

void load_tasks(void) {
    task_capacity = TASKS_CAPACITY;

    char *meta_file_path = get_meta_file_path();

    if (!check_file_exists(meta_file_path)) {
        tasks = malloc(sizeof(Task) * TASKS_CAPACITY);
        if (tasks == NULL) {
            perror("Error creating tasks");
            return;
        }

        return;
    }

    FILE *meta_file = fopen(meta_file_path, "rb");

    if (meta_file == NULL) {
        perror("Error opening meta file");
        exit(1);
    }

    if (fread(&tasks_count, sizeof(size_t), 1, meta_file) != 1) {
        printf("Did not read the tasks count\n");
        perror("Error reading tasks");
        destroy_tasks();
        fclose(meta_file);
        exit(1);
    }

    realloc_tasks();

    if (fread(tasks, sizeof(Task), tasks_count, meta_file) != tasks_count) {
        printf("Did not read exact amount of tasks\n");
        destroy_tasks();
        fclose(meta_file);
        exit(1);
    }

    fclose(meta_file);
    free(meta_file_path);
}

void save_tasks(void) {
    if (tasks == NULL) return;

    char *meta_file_path = get_meta_file_path();

    FILE *meta_file = fopen(meta_file_path, "wb");
    if (meta_file == NULL) {
        printf("-> %s\n", meta_file_path);
        perror("Error creating file");
        destroy_tasks();
        return;
    }

    fwrite(&tasks_count, sizeof(size_t), 1, meta_file);
    fwrite(tasks, sizeof(Task), tasks_count, meta_file);

    fclose(meta_file);
}

void append_task(const Task task) {
    if (tasks == NULL) {
        printf("Tasks array is empty.\n");
        return;
    }

    // append id
    strncpy(tasks[tasks_count].id, task.id, ID_SIZE - 1);
    tasks[tasks_count].id[ID_SIZE - 1] = '\0';
    // append title
    strncpy(tasks[tasks_count].title, task.title, TITLE_SIZE - 1);
    tasks[tasks_count].title[TITLE_SIZE - 1] = '\0';
    // append file_path
    strncpy(tasks[tasks_count].file_path, task.file_path, FILE_PATH_SIZE - 1);
    tasks[tasks_count].file_path[FILE_PATH_SIZE - 1] = '\0';

    tasks_count++;
}

void remove_task(const Task *task) {
    if (tasks == NULL) {
        printf("Tasks array is empty.\n");
        return;
    }

    if (task == NULL || tasks_count == 0) {
        return;
    }

    size_t index = (size_t)(task - tasks);
    if (index >= tasks_count) {
        return;
    }

    for (size_t i = index + 1; i < tasks_count; i++) {
        tasks[i - 1] = tasks[i];
    }

    tasks_count--;
    tasks[tasks_count] = (Task){0};
}

Task *find_task_by_id(const char *id) {
    if (tasks == NULL || id == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < tasks_count; i++) {
        if (strcmp(tasks[i].id, id) == 0) {
            return &tasks[i];
        }
    }

    return NULL;
}

char *create_task_path(const char *task_id, const bool is_file_path) {
    const char *suffix = is_file_path ? "/TASK.md" : "";
    const char *task_directory = get_task_directory();
    size_t len =
        strlen(task_directory) + 1 /* slash */ + strlen(task_id) + strlen(suffix) + 1; // +1 for '\0'
    char *path = malloc(len);
    if (path == NULL) {
        return NULL;
    }

    snprintf(path, len, "%s/%s%s", task_directory, task_id, suffix);

    return path;
}

char *create_id(void) {
    return get_current_time("%Y%m%d%H%M%S");
}

char *create_task_content(const char *task_id, const char *title) {
    char *string_format = "# ID: %s\n";
    char content[1024];
    size_t len = strlen(string_format) + strlen(task_id);
    snprintf(content, len, string_format, task_id);

    char *curr_time = get_current_time("%Y-%m-%d %H:%M:%S");
    char created_at[256];
    string_format = "# CREATED_AT: %s\n\n";
    len = strlen(string_format) + strlen(curr_time);
    snprintf(created_at, len, string_format, get_current_time("%Y-%m-%d %H:%M:%S"));

    strcat(content, created_at);
    strcat(content, "    STATUS: OPEN\n");
    strcat(content, "    PRIORITY: ?\n");
    strcat(content, "    TASK: \n");
    strcat(content, "        ");
    strcat(content, title);
    strcat(content, "\n");
    strcat(content, "        // Description\n");

    free(curr_time);

    return strdup(content);
}

int create_task(char *title) {
    char *id = create_id();

    if (id == NULL) {
        perror("Error creating task id");
        return EXIT_FAILURE;
    }

    char *dir_path = create_task_path(id, false);

    if (dir_path == NULL) {
        perror("create_task: Error creating directory path");
        return EXIT_FAILURE;
    }

    if (create_directory(dir_path) != 0) {
        fprintf(stderr, "Error creating task %s directory\n", id);
        free(id);
        free(dir_path);
        return EXIT_FAILURE;
    }

    char *file_path = create_task_path(id, true);

    if (file_path == NULL) {
        perror("create_task: Error creating file path");
        free(id);
        free(dir_path);
        return EXIT_FAILURE;
    }

    char *content = create_task_content(id, title);

    if (!create_file(file_path, content)) {
        perror("Error creating file2");
        free(id);
        free(dir_path);
        free(content);
        free(file_path);
        return EXIT_FAILURE;
    }

    Task task = {0};
    strncpy(task.id, id, ID_SIZE - 1);
    task.id[ID_SIZE - 1] = '\0';
    strncpy(task.title, title, TITLE_SIZE - 1);
    task.title[TITLE_SIZE - 1] = '\0';
    strncpy(task.file_path, file_path, FILE_PATH_SIZE - 1);
    task.file_path[FILE_PATH_SIZE - 1] = '\0';

    append_task(task);

    save_tasks();

    printf("Task created... %s/%s/TASK.md\n", get_task_directory(), id);

    free(id);
    free(dir_path);
    free(content);
    free(file_path);

    return EXIT_SUCCESS;
}

char *get_task_title(const char *task_path) {
    // Open the file
    FILE *file = fopen(task_path, "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening task file %s\n", task_path);
        return NULL;
    }

    // Read into the buffer
    char buffer[1024];
    fread(buffer, sizeof(char), 1024, file);

    // Found the description
    char *line_ptr = NULL;
    char *line = strtok_r(buffer, "\n", &line_ptr);

    while (line) {
        if (strstr(line, "TASK:") != NULL) {
            line = strtok_r(NULL, "\n", &line_ptr);
            break;
        }

        line = strtok_r(NULL, "\n", &line_ptr);
    }

    if (line != NULL) line += 8;

    return strdup(line);
}

void show_tasks(void) {
    const int id_width = ID_SIZE;
    const int title_width = TITLE_SIZE / 4;

    // -------------
    // Table header
    printf("+");
    for (int i = 0; i < id_width + 1; i++) printf("-");
    printf("+");
    for (int i = 0; i < title_width + 1; i++) printf("-");
    printf("+\n");

    // Table
    printf("| %-*s | %-*s |\n", id_width - 1, "Task ID", title_width - 1, "Title");

    printf("+");
    for (int i = 0; i < id_width + 1; i++) printf("-");
    printf("+");
    for (int i = 0; i < title_width + 1; i++) printf("-");
    printf("+\n");

    // if no tasks show not found
    if (tasks_count == 0) {
        printf("| %-*s   %-*s|\n", id_width - 1, "No tasks found.", title_width - 1, "");
    }
    // print the tasks
    else {
        for (size_t i = 0; i < tasks_count; i++) {
            Task task = tasks[i];

            const size_t len = strlen(task.title);
            const int cut = (len > title_width) ? title_width - 4 : title_width;

            // cut title properly
            char title_cpy[64] = {0};
            strncpy(title_cpy, task.title, cut);
            if (len > title_width) {
                strcat(title_cpy, "...");
            }
            title_cpy[title_width - 1] = '\0';

            printf("| %-*s | %-*s |\n", id_width - 1, task.id, 64 - 1, title_cpy);
        }
    }

    // table footer
    printf("+");
    for (int i = 0; i < id_width + 1; i++) printf("-");
    printf("+");
    for (int i = 0; i < title_width + 1; i++) printf("-");
    printf("+\n");
}

int delete_task(const char *task_id) {
    if (task_id == NULL) {
        printf("\nProvide task id (20251010233212)\n");
        show_tasks();

        return EXIT_SUCCESS;
    }

    Task *task = find_task_by_id(task_id);

    if (task == NULL) {
        printf("Task %s does not exists.\n", task_id);
        return EXIT_FAILURE;
    }

    const char *task_file = create_task_path(task_id, true);
    if (task_file == NULL) {
        perror("Error creating task file path");
        return EXIT_FAILURE;
    }

    if (!check_file_exists(task_file)) {
        printf("Task %s file does not exists.\n", task_id);
        return EXIT_FAILURE;
    }

    if (!remove_file(task_file)) {
        return EXIT_FAILURE;
    }

    const char* task_dir = create_task_path(task_id, false);
    if (!remove_directory(task_dir)) {
        return EXIT_FAILURE;
    }

    remove_task(task);
    save_tasks();

    printf("Task %s is removed successfully.\n", task_id);

    return EXIT_SUCCESS;
}

int preview_task(const char *task_id) {
    if (task_id == NULL) {
        printf("\nProvide task id (20251010233212)\n");
        show_tasks();
        return EXIT_SUCCESS;
    }

    char *task_file = create_task_path(task_id, true);

    if (task_file == NULL) {
        printf("preview_task: Error create task file path.\n");
        return EXIT_FAILURE;
    }

    FILE *file = fopen(task_file, "r");

    if (file == NULL) {
        perror("preview_task: Error opening task file");
        show_tasks();
        free(task_file);
        return EXIT_FAILURE;
    }

    char buffer[1024];
    fread(buffer, sizeof(char), 1024, file);

    char buffer2[1024];
    strcpy(buffer2, buffer);

    const size_t line_len = get_longest_line_size(buffer2);

    create_hor_line(line_len);
    printf("%s", buffer);
    create_hor_line(line_len);

    free(task_file);

    return EXIT_SUCCESS;
}
