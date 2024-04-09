#include <ncurses.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PATH_LEN 256

// Структура для представления файла или каталога
typedef struct {
    char name[MAX_PATH_LEN];
    int is_directory;
} FileEntry;

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

int main() {
    char path1[MAX_PATH_LEN] = "/"; // Путь для панели 1
    char path2[MAX_PATH_LEN] = "/"; // Путь для панели 2
    FileEntry *entries1, *entries2;
    int num_entries1, num_entries2;
    int current_index1 = 0, current_index2 = 0;
    int selected_index1 = -1, selected_index2 = -1;
    int active_panel = 1; // 1 для панели 1, 2 для панели 2

    // Инициализируем ncurses
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    while (1) {
        // Перечисляем файлы для обеих панелей
        list_files(path1, &entries1, &num_entries1);
        list_files(path2, &entries2, &num_entries2);

        // Очищаем экран
        clear();

        // Рисуем обе панели
        draw_panel(0, 0, LINES, COLS / 2, entries1, num_entries1, current_index1, selected_index1, active_panel == 1);
        draw_panel(0, COLS / 2, LINES, COLS / 2, entries2, num_entries2, current_index2, selected_index2, active_panel == 2);

        // Обновляем экран
        refresh();

        // Получаем ввод пользователя
        int ch = getch();
        switch (ch) {
            case '\t': // Клавиша Tab для переключения панелей
                active_panel = (active_panel == 1) ? 2 : 1;
                break;
            case KEY_UP:
                if (active_panel == 1)
                    current_index1 = (current_index1 > 0) ? current_index1 - 1 : 0;
                else
                    current_index2 = (current_index2 > 0) ? current_index2 - 1 : 0;
                break;
            case KEY_DOWN:
                if (active_panel == 1)
                    current_index1 = (current_index1 < num_entries1 - 1) ? current_index1 + 1 : num_entries1 - 1;
                else
                    current_index2 = (current_index2 < num_entries2 - 1) ? current_index2 + 1 : num_entries2 - 1;
                break;
            case KEY_BACKSPACE: // Клавиша Backspace для возврата в родительский каталог
                if (active_panel == 1 && strcmp(path1, "/") != 0) {
                    char *last_slash = strrchr(path1, '/');
                    if (last_slash != path1)
                        *last_slash = '\0';
                    else
                        *(last_slash + 1) = '\0';
                } else if (active_panel == 2 && strcmp(path2, "/") != 0) {
                    char *last_slash = strrchr(path2, '/');
                    if (last_slash != path2)
                        *last_slash = '\0';
                    else
                        *(last_slash + 1) =

 '\0';
                }
                break;
            case 10: // Клавиша Enter для входа в каталог
                if (active_panel == 1 && entries1[current_index1].is_directory) {
                    strcat(path1, entries1[current_index1].name);
                    strcat(path1, "/");
                }
                else if (active_panel == 2 && entries2[current_index2].is_directory) {
                    strcat(path2, entries2[current_index2].name);
                    strcat(path2, "/");
                }
                break;
            case 'q':
            case 'Q':
                // Освобождаем память и выходим
                free_entries(entries1, num_entries1);
                free_entries(entries2, num_entries2);
                endwin();
                return 0;
        }
    }

    // Этот код никогда не должен сюда дойти
    return 0;
}
