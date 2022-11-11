#include "syscall.h"

#define MAX_SHORT_FILE_NAME 32
#define MAX_LENGTH_OF_FILE 1024

int main() {
    // declaration area
    OpenFileId src_fileId;
    OpenFileId dst_fileId;
    char src_filename[MAX_SHORT_FILE_NAME];
    char dst_filename[MAX_SHORT_FILE_NAME];

    char buffer[MAX_LENGTH_OF_FILE];
    int nBytes;
    int isWrited;
    int isCreated;
    int i;

    // execution area

    // ------------ Get file path of src and dst --------------------------------
    PrintString("Source file path: ");
    ReadString(src_filename, MAX_SHORT_FILE_NAME);
    PrintString("Destination file path: ");
    ReadString(dst_filename, MAX_SHORT_FILE_NAME);

    PrintString("Waiting for copy....\n");

    // ------------ Check for src file existence --------------------------------
    src_fileId = Open(src_filename);
    if (src_fileId == -1) {
        PrintString("Source file not found\n");
    }
    else {
        nBytes = Read(buffer, MAX_LENGTH_OF_FILE, src_fileId);
        if (nBytes <= 0) {
            PrintString("Nothing be done because of empty file\n");
        }
        else {

            dst_fileId = Open(dst_filename);
            if (dst_fileId != -1) {
                PrintString("Destination file already exists\n");
            }
            else {
                isCreated = Create(dst_filename);
                if (isCreated != 0) {
                    PrintString("Cannot create destination file\n");
                }
                else {
                    dst_fileId = Open(dst_filename);
                    isWrited = Write(buffer, nBytes, dst_fileId);
                    if (isWrited == -1) {
                        PrintString("Cannot write to destination file\n");
                    }
                    else {
                        PrintString("File copy successfully\n");
                    }

                    Close(dst_fileId);
                }
            }
        }
        Close(src_fileId);
    }

    Halt();
    return 0;
}