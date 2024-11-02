#include <stdio.h>
#include <windows.h>

#define BUFFER_SIZE 1024

int main() {
    char buffer[BUFFER_SIZE];

    HANDLE readHandle = GetStdHandle(STD_INPUT_HANDLE);
    HANDLE writeHandle = GetStdHandle(STD_OUTPUT_HANDLE);

    DWORD readedBytes, writedBytes;

    ReadFile(readHandle, buffer, BUFFER_SIZE, &readedBytes, NULL);

    for (int i = 0; i < readedBytes; i++) {
        if (buffer[i] == ' ') {
            buffer[i] = '_';
        }
    }

    WriteFile(writeHandle, buffer, readedBytes, &writedBytes, NULL);

    return 0;
}