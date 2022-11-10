#include "syscall.h"

#define MAX_SHORT_FILE_NAME 32
#define MAX_LENGTH_OF_FILE 1024

int main()
{
	// declaration area
	OpenFileId fd;
	char filename[MAX_SHORT_FILE_NAME];
	int isClosed;
	char buffer[MAX_LENGTH_OF_FILE];
	int nBytes;
	int i;

	PrintString("Enter filename: ");
	ReadString(filename, MAX_SHORT_FILE_NAME);
	fd = Open(filename);
	if (fd == -1)
	{
		PrintString("File is not exist!\n");
	}
	else
	{
		PrintString("\nContent in ");
		PrintString(filename);
		PrintString(" is: \n");
		nBytes = Read(buffer, MAX_LENGTH_OF_FILE, fd);
		if (nBytes == 0)
		{
			PrintString("File empty!");
		}
		else
		{
			for (i = 0; i < nBytes; i++)
			{
				PrintChar(buffer[i]);
			}
			PrintChar('\n');
		}

		isClosed = Close(fd);
		if (isClosed != 1)
		{
			PrintString("File are not close correctly\n");
		}
	}

	Halt();
	return 0;
}