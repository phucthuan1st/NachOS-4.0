#include "syscall.h"

#define MAX_SHORT_FILE_NAME 32
#define MAX_LENGTH_OF_FILE 1024

int main() {
    int fileId_1;
    int fileId_2;

    char* filename_1 = "file1.txt";
    char* filename_2 = "file2.txt";

    char buffer1[MAX_LENGTH_OF_FILE];
    char buffer2[MAX_LENGTH_OF_FILE];

    int nBytes_1;
    int nBytes_2;

    int position;

    fileId_1 = Open(filename_1);
    PrintNum(fileId_1);
    PrintChar('\n');
    
    fileId_2 = Open(filename_2);
    PrintNum(fileId_2);
    PrintChar('\n');

    nBytes_1 = Read(buffer1, MAX_LENGTH_OF_FILE, fileId_1);
    nBytes_2 = Read(buffer2, MAX_LENGTH_OF_FILE, fileId_2);

    position = Seek(nBytes_1, fileId_1);
    Write(buffer2, nBytes_2, fileId_1);

    Close(fileId_1);
    Close(fileId_2);

    Read(buffer1, nBytes_1 + nBytes_2, fileId_1);

    PrintString(buffer1);

    Halt();
    return 0;
}