#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "app.h"
#include "tasks.h"

void display_help(void);

int main(const int argc, char *argv[]) {
    if (!setup()) {
        perror("Error creating directory");
        return EXIT_FAILURE;
    }

    load_tasks();

    if (argc == 1) {
        display_help();

        return EXIT_SUCCESS;
    }

    int opt;

    while ((opt = getopt(argc, argv, "hacdp::")) != -1) {
        switch (opt) {
        case 'h':
            display_help();
            return EXIT_SUCCESS;
        case 'a':
            printf("\nTasks:\n");
            show_tasks();
            return EXIT_SUCCESS;
        case 'c':
            if (argv[optind] == NULL) {
                printf("\nUsage: tasks -c \"My task title\"\n");
                return EXIT_FAILURE;
            }

            return create_task(argv[optind]);
        case 'd':
            if (argv[optind] == NULL) {
                printf("\nUsage: tasks -d [TIMESTAMPT]\n");
                return EXIT_FAILURE;
            }
            return delete_task(argv[optind]);
        case 'p':
            if (argv[optind] == NULL) {
                printf("\nUsage: tasks -p [TIMESTAMPT]\n");
                return EXIT_FAILURE;
            }
            return preview_task(argv[optind]);
        default:
            printf("Unknown option: -%c\n", optopt ? optopt : '?');
            return EXIT_FAILURE;
        }
    }

    return 0;
}

void display_help(void) {
    printf("tasks - manage entries stored in %s\n\n", get_task_directory());
    printf("Usage:\n");
    printf("  tasks [-h] [-c] [-d [TASK_ID]]\n\n");
    printf("Options:\n");
    printf("  -h                         Show this help message and exit\n");
    printf("  -a                         Show all tasks\n");
    printf("  -c \"some title\"          Create new task file (name assigned automatically)\n");
    printf("  -d [TASK_ID]               Delete task by id; lists tasks if no id is provided\n\n");
    printf("  -p [TASK_ID]               Preview task by id; lists tasks if no id is provided\n\n");
    printf("Examples:\n");
    printf("  tasks                      Print this help text\n");
    printf("  tasks -h                   Show help explicitly\n");
    printf("  tasks -a                   Show all tasks\n");
    printf("  tasks -c \"some title\"    Create a new task with title\n");
    printf("  tasks -d 12345             Delete task with id 12345\n");
    printf("  tasks -d                   List task ids to choose from\n");
    printf("  tasks -p 12345             Preview task with id 12345\n");
    printf("  tasks -p                   List task ids to choose from\n");
}
