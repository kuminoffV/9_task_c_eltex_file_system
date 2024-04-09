// Написать программу, которая создает файл с именем output.txt,
// записывает в него строку “String from file”, затем считывает ее из файла
// с конца и выводит на экран.
#include <stdio.h>
#include <string.h>

int main() {
    FILE *file;
    char outputText[] = "String from file";
    char readText[100];
    int length, i;

    file = fopen("output.txt", "w");
    if (file == NULL) {
        printf("Ошибка открытия файла\n");
        return 1;
    }

    fprintf(file, "%s", outputText);
    fclose(file);

    file = fopen("output.txt", "r");
    if (file == NULL) {
        printf("Ошибка открытия файла\n");
        return 1;
    }

    fseek(file, 0, SEEK_END);
    length = ftell(file);

    for (i = 1; i <= length; i++) {
        fseek(file, -i, SEEK_END);
        printf("%c", fgetc(file));
    }
    printf("\n");

    fclose(file);
    return 0;
}
