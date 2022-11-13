#include "syscall.h"

#define MAX_SHORT_FILE_NAME 32
#define MAX_LENGTH_OF_FILE 256

int main()
{
    // int fileId_1;
    // int fileId_2;

    // char filename_1[MAX_SHORT_FILE_NAME];
    // char* filename_2 = "thuan.txt";

    OpenFileId fileId_1;
    OpenFileId fileId_2;
    char filename_1[MAX_SHORT_FILE_NAME];
    char filename_2[MAX_SHORT_FILE_NAME];

    char buffer1[MAX_LENGTH_OF_FILE];
    char buffer2[MAX_LENGTH_OF_FILE];

    int nBytes_1;
    int nBytes_2;

    int position;

    PrintString("First source file path: ");
    ReadString(filename_1, MAX_SHORT_FILE_NAME);
    PrintString("Second source file path: ");
    ReadString(filename_2, MAX_SHORT_FILE_NAME);

    fileId_1 = Open(filename_1);

    if (fileId_1 == -1)
    {
        PrintString("First source file path not exist!\n");
    }
    else
    {
        fileId_2 = Open(filename_2);

        if (fileId_2 == -1)
        {
            PrintString("Second source file path not exist!\n");
        }
        else
        {
            nBytes_1 = Read(buffer1, MAX_LENGTH_OF_FILE, fileId_1);
            nBytes_2 = Read(buffer2, MAX_LENGTH_OF_FILE, fileId_2);

            position = Seek(nBytes_1, fileId_1);
            Write(buffer2, nBytes_2, fileId_1);

            Close(fileId_1);
            Close(fileId_2);

            Read(buffer1, nBytes_1 + nBytes_2, fileId_1);

            PrintString("Concatenate two files successfully!\n");
        }
    }

    Halt();
    return 0;
}
