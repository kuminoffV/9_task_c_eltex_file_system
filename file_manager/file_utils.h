#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <dirent.h>

#define MAX_PATH_LEN 256

// Структура для представления файла или каталога
typedef struct {
    char name[MAX_PATH_LEN];
    int is_directory;
} FileEntry;

// Функция для перечисления файлов и каталогов в указанном каталоге
int list_files(const char *path, FileEntry **entries, int *num_entries);

// Функция для освобождения памяти, выделенной для записей файлов
void free_entries(FileEntry *entries, int num_entries);

// Функция для отрисовки файлов в панели
void draw_panel(int start_y, int start_x, int height, int width, FileEntry *entries, int num_entries, int current_index, int selected_index, int is_active);

#endif /* FILE_UTILS_H */

