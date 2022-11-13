#include "syscall.h"
#define MAX_SHORT_FILE_NAME 32

int main() {
    char filename[MAX_SHORT_FILE_NAME];
    PrintString("Enter your filename: ");
    ReadString(filename, MAX_SHORT_FILE_NAME);

    if (Remove(filename) < 0) {
        PrintString("Cannot remove file\n");
    }

    Halt();
}
