#include "syscall.h"
#define MAX_SHORT_FILE_NAME 32

int main() {
    char filename[MAX_SHORT_FILE_NAME];
    ReadString(filename, MAX_SHORT_FILE_NAME);

    if (Create(filename) < 0) {
        PrintString("Cannot create file\n");
    }

    Halt();
}