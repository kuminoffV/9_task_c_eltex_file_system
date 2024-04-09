#include "file_utils.h"
#include <ncurses.h> // Включаем заголовочный файл ncurses для использования символьных атрибутов
#include <stdlib.h>
#include <string.h>

// Функция для перечисления файлов и каталогов в указанном каталоге
int list_files(const char *path, FileEntry **entries, int *num_entries) {
    DIR *dir;
    struct dirent *entry;
    int count = 0;

    // Открываем каталог
    if ((dir = opendir(path)) == NULL)
        return -1;

    // Выделяем память для записей
    *entries = (FileEntry *)malloc(sizeof(FileEntry) * 100);

    // Считываем записи каталога
    while ((entry = readdir(dir)) != NULL) {
        // Игнорируем скрытые файлы и каталоги
        if (entry->d_name[0] == '.')
            continue;

        // Заполняем информацию о файле
        strcpy((*entries)[count].name, entry->d_name);
        (*entries)[count].is_directory = (entry->d_type == DT_DIR);
        count++;
    }

    closedir(dir);
    *num_entries = count;
    return 0;
}

// Функция для освобождения памяти, выделенной для записей файлов
void free_entries(FileEntry *entries, int num_entries) {
    if (entries != NULL)
        free(entries);
}

// Функция для отрисовки файлов в панели
void draw_panel(int start_y, int start_x, int height, int width, FileEntry *entries, int num_entries, int current_index, int selected_index, int is_active) {
    int i;
    // Рисуем рамку панели
    if (is_active)
        attron(A_BOLD);
    for (i = 0; i < height; i++) {
        mvprintw(start_y + i, start_x, "|");
        mvprintw(start_y + i, start_x + width, "|");
    }
    for (i = 0; i < width + 1; i++) {
        mvprintw(start_y, start_x + i, "-");
        mvprintw(start_y + height, start_x + i, "-");
    }
    attroff(A_BOLD);

    // Рисуем файлы
    attron(A_BOLD);
    mvprintw(start_y, start_x + 2, "Name");
    attroff(A_BOLD);

    for (i = 0; i < height - 2 && i < num_entries; i++) {
        mvprintw(start_y + i + 1, start_x + 2, entries[i].name);

        // Добавляем метку --DIR-- для каталогов
        if (entries[i].is_directory)
            mvprintw(start_y + i + 1, start_x + width - 9, "--DIR--");

        // Подсвечиваем выбранный элемент
        if (i == selected_index && is_active) {
            attron(A_REVERSE);
            mvprintw(start_y + i + 1, start_x + 2, entries[i].name);
            attroff(A_REVERSE);
        }

        // Показываем '>' для текущего каталога
        if (i == current_index && is_active)
            mvprintw(start_y + i + 1, start_x, ">");
    }
}
