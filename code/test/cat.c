#include "syscall.h"

#define MAX_SHORT_FILE_NAME 32
#define MAX_LENGTH_OF_FILE 3000

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
	PrintString("We're here\n");

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
			// PrintNum(nBytes);
			// PrintChar('\n');
			PrintString(buffer);
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