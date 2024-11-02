#include <stdio.h>
#include <windows.h>
#include <string.h>

#define BUFFER_SIZE 1024

HANDLE g_hChild1_IN_Rd = NULL;
HANDLE g_hChild1_OUT_Wr = NULL;

HANDLE g_hChild2_IN_Rd = NULL;
HANDLE g_hChild2_OUT_Wr = NULL;

HANDLE g_hParent_IN_Rd = NULL;
HANDLE g_hParent_OUT_Wr = NULL;

int CreateChildProcess();

int main() {
    SECURITY_ATTRIBUTES saAttr;
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;

    CreatePipe(&g_hChild1_IN_Rd, &g_hParent_OUT_Wr, &saAttr, 0);

    CreatePipe(&g_hChild2_IN_Rd, &g_hChild1_OUT_Wr, &saAttr, 0);

    CreatePipe(&g_hParent_IN_Rd, &g_hChild2_OUT_Wr, &saAttr, 0);

    CreateChildProcess();

    char input[BUFFER_SIZE];
    printf("Enter a string: ");
    fgets(input, BUFFER_SIZE, stdin);

    DWORD dwWritten, dwRead;

    WriteFile(g_hParent_OUT_Wr, input, strlen(input), &dwWritten, NULL);

    CloseHandle(g_hParent_OUT_Wr);

    char result[BUFFER_SIZE];

    ReadFile(g_hParent_IN_Rd, result, BUFFER_SIZE, &dwRead, NULL);

    printf("Result: %s", result);

    CloseHandle(g_hParent_IN_Rd);

    return 0;
}

int CreateChildProcess() {
    PROCESS_INFORMATION pi1, pi2;
    STARTUPINFO si1, si2;

    ZeroMemory(&si1, sizeof(si1));
    si1.cb = sizeof(si1);
    si1.hStdInput = g_hChild1_IN_Rd;
    si1.hStdOutput = g_hChild1_OUT_Wr;
    si1.dwFlags |= STARTF_USESTDHANDLES;

    ZeroMemory(&si2, sizeof(si2));
    si2.cb = sizeof(si2);
    si2.hStdInput = g_hChild2_IN_Rd;
    si2.hStdOutput = g_hChild2_OUT_Wr;
    si2.dwFlags |= STARTF_USESTDHANDLES;

    CreateProcess(NULL, "child1.exe", NULL, NULL, TRUE, 0, NULL, NULL, &si1, &pi1);

    CreateProcess(NULL, "child2.exe", NULL, NULL, TRUE, 0, NULL, NULL, &si2, &pi2);
}