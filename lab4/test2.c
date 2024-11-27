#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>

typedef float (*E_func_t)(int);

void load_library(const char *lib_name, void **handle, E_func_t *E_func) {
    char *error;

    if (*handle) {
        dlclose(*handle);
    }
    *handle = dlopen(lib_name, RTLD_LAZY);
    if (!*handle) {
        fprintf(stderr, "Ошибка загрузки библиотеки: %s\n", dlerror());
        exit(EXIT_FAILURE);
    }
    dlerror();
    *E_func = (E_func_t) dlsym(*handle, "E");
    if ((error = dlerror()) != NULL) {
        fprintf(stderr, "Ошибка загрузки функции: %s\n", error);
        exit(EXIT_FAILURE);
    }
    printf("Загружена библиотека %s\n", lib_name);
}

float calculate(E_func_t E_func, int x) {
    return E_func(x);
}

int main() {
    void *handle = NULL;
    E_func_t E_func = NULL;
    char command[256];

    char current_lib[50] = "";

    while (1) {
        printf("Введите команду: ");
        fgets(command, sizeof(command), stdin);
        command[strcspn(command, "\n")] = '\0';

        if (strcmp(command, "0") == 0) {
            if (strcmp(current_lib, "./lib_impl1.so") == 0 || strcmp(current_lib, "") == 0) {
                strcpy(current_lib, "./lib_impl2.so");
            } else {
                strcpy(current_lib, "./lib_impl1.so");
            }
            load_library(current_lib, &handle, &E_func);
        } else if (command[0] == '1') {
            if (strcmp(current_lib, "./lib_impl1.so") != 0) {
                strcpy(current_lib, "./lib_impl1.so");
                load_library(current_lib, &handle, &E_func);
            }
            int x;
            if (sscanf(command + 2, "%d", &x) != 1) {
                printf("Неверный аргумент. Используйте: 1 x\n");
                continue;
            }
            float result = calculate(E_func, x);     
            printf("E(%d) = %f\n", x, result);
        } else if (command[0] == '2') {
            if (strcmp(current_lib, "./lib_impl2.so") != 0) {
                strcpy(current_lib, "./lib_impl2.so");
                load_library(current_lib, &handle, &E_func);
            }
            int x;
            if (sscanf(command + 2, "%d", &x) != 1) {
                printf("Неверный аргумент. Используйте: 2 x\n");
                continue;
            }
            float result = calculate(E_func, x);     
            printf("E(%d) = %f\n", x, result);
        } else if (strcmp(command, "exit") == 0 || strcmp(command, "q") == 0) {
            break;
        } else {
            printf("Неизвестная команда.\n");
        }
    }

    if (handle) {
        dlclose(handle);
    }
    return 0;
}
