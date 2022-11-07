#include "syscall.h"
#define MAX_SHORT_FILE_NAME 32

int main() {
    char filename[MAX_SHORT_FILE_NAME + 1];
    ReadString(filename, MAX_SHORT_FILE_NAME);

    const int sucess_code = 1;

    if (Create(filename) != sucess_code) {
        PrintString("Cannot create file\n");
    }
    else {
        PrintString("File created successfully\n");
    }
}