#include "syscall.h"

#define MAX_SHORT_FILE_NAME 32
#define MAX_LENGTH_OF_FILE 1024

int main() {
    // declaration area
    OpenFileId fd;
    char filename[MAX_SHORT_FILE_NAME];
    int isClosed;
    char buffer[MAX_LENGTH_OF_FILE];
    int nBytes;
    int i;

    ReadString(filename, MAX_SHORT_FILE_NAME);
    fd = Open(filename);
    PrintString("File descriptor is: ");
    PrintNum(fd);
    PrintChar('\n');

    nBytes = Read(buffer, MAX_LENGTH_OF_FILE, fd);
    for (i = 0; i < nBytes; i++) {
        PrintChar(buffer[i]);
    }
    PrintChar('\n');

    isClosed = Close(fd);
    if (isClosed == 1) {
        PrintString("File is closed successfully\n");
    }

    Halt();
    return 0;
}