# tasks

> A lightweight, file-based CLI tool for managing personal tasks using folders and Markdown.

`tasks` is a simple productivity utility that stores each task as its own directory containing a `TASK.md` file.  
The structure is intentionally designed to support future expansion such as attachments, images, or additional metadata alongside the Markdown file.

> ⚠️ Disclaimer  
> This is a personal project provided as-is.  
Use at your own risk. No guarantees are provided regarding data safety or stability.

## 🖥️ OS Support

- macOS: supported
- Linux: not verified
- Windows: not supported (for now)
- 
## ✨ Features

- File-based task storage (no database, no lock-in)
- Human-readable Markdown task format
- Stable task identifiers based on UTC timestamps
- Designed for extensibility (attachments, assets, metadata)
- Single static binary — fast, portable, and dependency-free

## 📁 Task Storage Model

Each task is stored in its own directory, named using the current UTC timestamp:

```
20260104201212/
└── TASK.md
```

This structure allows future enhancements (images, notes, related files) to be placed directly next to the task description.

## 📍 Task Directory Location

When executed, `tasks` creates (or uses) a `tasks/` directory in the **current working directory**.

- If the program is run **outside** a `tasks/` directory, it creates one.
- If run **inside** an existing `tasks/` directory, it uses that directory directly and avoids nesting.

This makes the tool fully portable and project-local.

## 🧰 Requirements

- C compiler with **C11** support
- **CMake 4.1+**

## 🔨 Build

```sh
cmake -S . -B build
cmake --build build
```

## 🚀 Run

```sh
./build/tasks
```

## 📦 Install (system-wide)

```sh
cmake -S . -B build
cmake --build build
make install # use (sudo make install) if permission required
```

## 🧭 Usage

```
tasks — manage entries stored in ./tasks

Usage:
  tasks [-h] [-a] [-c "title"] [-d [TASK_ID]] [-p [TASK_ID]]

Options:
  -h                        Show help and exit
  -a                        List all tasks
  -c "some title"           Create a new task
  -d [TASK_ID]              Delete a task (or list tasks if no ID provided)
  -p [TASK_ID]              Preview a task (or list tasks if no ID provided)

Examples:
  tasks
  tasks -a
  tasks -c "Write documentation"
  tasks -p 20260104192618
  tasks -d 20260104192618
```

## 🧾 Task File Format

```text
# ID: 20260104192618
# CREATED_AT: 2026-01-04 19:26:18

    STATUS: OPEN
    PRIORITY: ?
    TASK:
        Task title
        // Description
```

## 🤝 Collaboration & Version Control

Once tasks are created inside the `tasks/` directory, you can choose how they integrate with your workflow:

- **Commit tasks to Git** if you want all contributors to share and update the same task list.
- **Ignore the `tasks/` directory** in `.gitignore` if tasks are personal or should not be modified by every contributor.

This flexibility allows the tool to be used both for individual productivity and for team coordination.

## 🗺 Roadmap

- [ ] Edit task content directly from CLI
- [ ] Mark tasks as completed / archived
- [ ] Tagging and filtering
- [ ] Task search
- [ ] Import / export support
- [ ] Optional JSON metadata file
- [ ] Optional sync support (git-friendly)
- [ ] Cross-platform packaging
