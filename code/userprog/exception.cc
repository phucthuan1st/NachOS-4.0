// exception.cc
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "main.h"
#include "copyright.h"
#include "kernel.h"
#include "ksyscall.h"
#include "synchconsole.h"
#include "syscall.h"

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2.
//
// If you are handling a system call, don't forget to increment the pc
// before returning. (Or else you'll loop making the same system call forever!)
//
//	"which" is the kind of exception.  The list of possible exceptions
//	is in machine.h.
//----------------------------------------------------------------------
#define MAX_LENGTH_STRING 2147483647
#define MAX_SHORT_FILE_NAME 32
#define INT_MIN -2147483647
#define INT_MAX 2147483647

void increasePC()
{
	/* set previous programm counter (debugging only)*/
	kernel->machine->WriteRegister(PrevPCReg,
								   kernel->machine->ReadRegister(PCReg));

	/* set programm counter to next instruction (all Instructions are 4 byte
	 * wide)*/
	kernel->machine->WriteRegister(PCReg,
								   kernel->machine->ReadRegister(PCReg) + 4);

	/* set next programm counter for brach execution */
	kernel->machine->WriteRegister(NextPCReg,
								   kernel->machine->ReadRegister(PCReg) + 4);
}

void clearInputBuffer()
{
	char c;
	while ((c = kernel->synchConsoleIn->GetChar()) != '\n' && c != EOF)
	{
	};
}

// Copy buffer from User memory space to system memory space
char *User2System(int virAddr, int limit)
{
	int i;
	int oneChar;
	char *kernelBuf = NULL;
	kernelBuf = new char[limit + 1];
	if (kernelBuf == NULL)
		return kernelBuf;
	memset(kernelBuf, 0, limit + 1);
	// printf("\n Filename u2s: ");
	for (i = 0; i < limit; i++)
	{
		kernel->machine->ReadMem(virAddr + i, 1,
								 &oneChar); // doc gia tri tu virArr den oneChar
		kernelBuf[i] = (char)oneChar;		// sau do gan vao buffer
		// printf("%c",kernelBuf[i]);
		if (oneChar == 0) // ki tu null cuoi chuoi
			break;
	}
	return kernelBuf;
}

// Input: 	- User space address (int)
// 			- Limit of buffer (int)
// 			- Buffer (char[])
// Output:- Number of bytes copied (int)
// Purpose: Copy buffer from System memory space to User memory space
int System2User(int virtAddr, int len, char *buffer)
{
	if (len < 0)
		return -1;
	if (len == 0)
		return len;
	int i = 0;
	int oneChar = 0;
	do
	{
		oneChar = (int)buffer[i];
		kernel->machine->WriteMem(virtAddr + i, 1, oneChar);
		i++;
	} while (i < len && oneChar != 0);
	return i;
}

void handle_SC_ReadNum()
{
	long long res; // khai bao ket qua ra ve kieu long long de sau nay ep kieu
	char c;
	bool isEnd = false;			 // Kiem tra dau cach ket thuc so
	bool isNegative = false;	 // Kiem tra so am
	bool isContainDigit = false; // Kiem tra chi nhap dau -
	res = 0;
	while ((c = kernel->synchConsoleIn->GetChar()) == ' ')
	{ // kiem tra xem co khoang trang o dau hay khong
	}
	if (c == '-')
	{ // check xem co phai so am hay khong
		isNegative = true;
	}
	else if (c >= '0' && c <= '9')
	{ // check xem cac ki tu co nam tu 0->9 hay khong
		res = res * 10 + (c - '0');
	}
	else if (c == '\n')
	{
		DEBUG(dbgSys, "\nUser has not entered any number yet\n");
		printf("\nUser has not entered any number yet\n");
		res = 0;
		kernel->machine->WriteRegister(2, int(res));
		return;
	}
	else
	{
		DEBUG(dbgSys, "\nError!!!User entered invalid character\n");
		res = 0;
		kernel->machine->WriteRegister(2, int(res));
		printf("\nError!!!User entered invalid character\n");
		clearInputBuffer();
		return;
	}
	while ((c = kernel->synchConsoleIn->GetChar()) != '\n')
	{ // doc den luc Enter xuong dong
		if (c >= '0' && c <= '9' && isEnd == false)
		{
			res = res * 10 + (c - '0');
			isContainDigit = true;
		}
		else if (c == ' ')
		{ // neu nhan them khoang trang thi den day la ket thuc
			isEnd = true;
		}
		else
		{ // neu la cac ki tu khac thi loi
			DEBUG(dbgSys, "\nError!!!This is not a number\n");
			res = 0;
			printf("\nError!!!This is not a number\n");
			kernel->machine->WriteRegister(2, int(res));
			clearInputBuffer();
			return;
		}
	}
	if (isNegative)
	{ // So am thi lay am gia tri res
		res = -res;
	}
	if (res > INT_MAX)
	{ // kiem tra so nguyen co lon hon quy dinh hay khong INT MAX 2147483647
		DEBUG(dbgSys, "\nError!!!This number is so large\n");
		printf("\nError!!!This number is so large\n");
		res = 0;
		kernel->machine->WriteRegister(2, int(res));
		return;
	}
	else if (res < INT_MIN)
	{ // kiem tra so nguyen co be hon quy dinh hay khong INT MIN -2147483647
		DEBUG(dbgSys, "\nError!!!This number is so small\n");
		printf("\nError!!!This number is so small\n");
		res = 0;
		kernel->machine->WriteRegister(2, int(res));
		return;
	}
	if (isNegative && res == 0 && !isContainDigit)
	{ // kiem tra truong hop nguoi dung nhap '-0'
		DEBUG(dbgSys, "\nError!!!This is (-) only\n");
		res = 0;
		printf("\nError!!!This is (-) only\n");
		kernel->machine->WriteRegister(2, int(res));
		return;
	}
	DEBUG(dbgSys, "\nThis is a number\n");
	kernel->machine->WriteRegister(2, int(res));
}

void handle_SC_ReadString()
{
	int virAddr; // khai bao dia chi nhan tu thanh ghi
	int length;
	int inputLength;
	char *strName;
	char c;
	bool over_length = false; // Check max_length

	virAddr = kernel->machine->ReadRegister(4); // lay dia chi tu thanh ghi (char buffer[] o user space)
	length = kernel->machine->ReadRegister(5);	// lay dia chi tu thanh
	strName = new char[length];					// day se la bien buffer duoc tra ve cho nguoi dung
	inputLength = 0;

	while ((c = kernel->synchConsoleIn->GetChar()) != '\n')
	{
		if (inputLength == length)
		{
			over_length = true;
			break;
		}
		strName[inputLength] = c;
		inputLength++;
	}
	strName[inputLength] = '\0';
	if (over_length)
	{
		printf("\nInput out of length\n");
		delete[] strName;
		strName = NULL;
		clearInputBuffer();
		return;
	}
	else
	{
		int numBytes = System2User(virAddr, inputLength, strName); // chuyen bo nho qua user
		if (numBytes == 0)
		{
			printf("\nEmpty string\n");
		}
		else if (numBytes > MAX_LENGTH_STRING)
		{
			printf("\nString out of max system length\n");
			return;
		}
	}
}

void handle_SC_ReadChar()
{
	char result;
	char c = 0;

	result = kernel->synchConsoleIn->GetChar();
	if (result == '\n') // nguoi dung khong nhap
	{
		DEBUG(dbgSys, "\nERROR: Empty!\n");
		printf("\nERROR: Empty!\n");
	}
	else // check xem co phai dung 1 ky tu khong?
	{
		c = kernel->synchConsoleIn->GetChar();
		if (c != '\n')
		{
			result = 0;
			DEBUG(dbgSys, "\nERROR: More than one character!\n");
			printf("\nERROR: More than one character!\n");
			clearInputBuffer();
		}
	}
	kernel->machine->WriteRegister(2, result);
}

void handle_SC_PrintString()
{
	int virtual_address; // khai bao bien dia chi de doc tu thanh ghi
	char *strName;		 // ten cua chuoi o phia user space
	int length;			 // chieu dai chuoi nguoi dung nhap
	int temp;			 // bien tam de luu
	char c;

	virtual_address = kernel->machine->ReadRegister(4);
	temp = virtual_address;

	length = 0;
	// tinh chieu dai chuoi ma nguoi dung nhap vao
	do
	{
		kernel->machine->ReadMem(temp, 1, (int *)&c);
		length++;
		temp = temp + 1;
	} while (c != '\0');

	strName = User2System(virtual_address, length); // truyen du lieu qua kernel space de xu ly

	if (strName == NULL)
	{ // kiem tra Truong hop khong du bo nho trong kernel space
		printf("\nNot enough memory in system\n");
		DEBUG(dbgAddr, "\nNot enough memory in system\n");
		kernel->machine->WriteRegister(2, -1); // return error
		delete strName;
		return;
	}
	else
	{
		if (strlen(strName) > MAX_LENGTH_STRING)
		{ // kiem tra neu chieu dai chuoi vuot qua quy dinh 1 chuoi cho phep
			printf("\nOut of index\n");
			kernel->machine->WriteRegister(2, -1); // return error
			delete strName;
			return;
		}
		else if (strlen(strName) == 0)
		{ // nguoi dung ko nhap gi
			kernel->synchConsoleOut->PutChar('\0');
			printf("\nEmpty string\n");
			DEBUG(dbgAddr, "\nEmpty string\n");
			delete strName;
			return;
		}
		else
		{
			for (int i = 0; i < length; i++)
			{
				kernel->synchConsoleOut->PutChar(strName[i]);
			}
			DEBUG(dbgAddr, "\nSuccessful!\n");
			return;
		}
	}
}

void handle_SC_PrintNum()
{
	int number;
	int temp_number;
	char *buffer;
	int i;
	int count_digits = 0; // Dem so chu so trong number (bo ki tu - neu la so am)
	int index_start = 0;  // Vi tri bat dau in chu so (trong truong hop so am thi la 1)
	int max_size = 11;	  // 1: dau, 10: digit so nguyen thi 10 phan tu la du
	number = kernel->machine->ReadRegister(4);
	if (number == 0)
	{										   // truong hop de xu ly nhat
		kernel->synchConsoleOut->PutChar('0'); // In ra man hinh so 0
		return;
	}
	/*Chuyen so thanh chuoi roi in ra man hinh*/
	if (number < 0)
	{
		number = -number; // Chuyen thanh so duong => easy
		index_start = 1;
	}
	temp_number = number; // bien tam cho number
	while (temp_number > 0)
	{ // dem so digits
		count_digits++;
		temp_number /= 10;
	}
	// int: [-2147483648, 2147483647]
	buffer = new char[max_size + 1];
	for (i = count_digits - 1 + index_start; i >= index_start; i--)
	{
		buffer[i] = (char)((number % 10) + '0'); // Lay tung chu so vao buffer
		number /= 10;
	}
	if (index_start == 1)
	{ // Neu la so am
		buffer[0] = '-';
		buffer[count_digits + 1] = 0; // Ket thuc chuoi la 0
		for (i = 0; i <= count_digits; i++)
		{
			kernel->synchConsoleOut->PutChar(buffer[i]);
		}
		delete[] buffer;
	}
	else
	{							  // Neu la so duong
		buffer[count_digits] = 0; // Ket thuc chuoi la 0
		for (i = 0; i <= count_digits - 1; i++)
		{
			kernel->synchConsoleOut->PutChar(buffer[i]);
		}
		delete[] buffer;
	}
}

void handle_SC_Create()
{
	int virtAddr = kernel->machine->ReadRegister(4);
	char *filename = User2System(virtAddr, MAX_SHORT_FILE_NAME);

	if (strlen(filename) <= 0)
	{
		printf("\nFile name is invalid\n");
		kernel->machine->WriteRegister(2, -1);
		delete[] filename;
		return;
	}

	if (filename == NULL)
	{
		printf("\n Not enough memory in system\n");
		kernel->machine->WriteRegister(2, -1);
		delete[] filename;
		return;
	}

	OpenFile *file = kernel->fileSystem->Open(filename);
	bool isExisted = (file != NULL);
	delete file;

	if (isExisted == true)
	{
		printf("\nFile is already existed\n");
		kernel->machine->WriteRegister(2, -1);
		delete[] filename;
		return;
	}
	else
	{
		try
		{
			kernel->fileSystem->Create(filename);
			printf("\nFile %s created successfully!!\n", filename);
			kernel->machine->WriteRegister(2, 0);
			delete[] filename;
		}
		catch (const char *error)
		{
			printf("%s", error);
			kernel->machine->WriteRegister(2, -1);
			delete[] filename;
		}

		return;
	}
}

void handle_SC_Open()
{
	int virtAddr = kernel->machine->ReadRegister(4);
	char *filename = User2System(virtAddr, MAX_SHORT_FILE_NAME);

	if (strlen(filename) <= 0)
	{
		printf("\nFile name is invalid\n");
		kernel->machine->WriteRegister(2, -1);
		delete[] filename;
		return;
	}

	if (filename == NULL)
	{
		printf("\n Not enough memory in system\n");
		kernel->machine->WriteRegister(2, -1);
		delete[] filename;
		return;
	}

	int fileDescriptor = OpenForReadWrite(filename, FALSE);
	kernel->machine->WriteRegister(2, fileDescriptor);
	delete[] filename;
}

void handle_SC_Close()
{
	OpenFileId fileDescriptor = kernel->machine->ReadRegister(4);

	if (fileDescriptor <= -1)
	{
		printf("File are not opened\n");
		kernel->machine->WriteRegister(2, -1);
		return;
	}

	if (fileDescriptor == 0 || fileDescriptor == 1) {
		printf("\nConsole IO detected!! Not a file to read or write\n");
		kernel->machine->WriteRegister(2, -1);
		return;
	}

	int isClosed = Close(fileDescriptor);
	if (isClosed < 0)
	{
		printf("There is some error when close the file with descriptor %d\n", fileDescriptor);
		kernel->machine->WriteRegister(2, isClosed);
		return;
	}

	kernel->machine->WriteRegister(2, 1);
	return;
}

void handle_SC_Read()
{
	int virtAddr = kernel->machine->ReadRegister(4);	  // address of the buffer
	int size = kernel->machine->ReadRegister(5);		  // size to read
	
	if (size <= 0) {
		printf("\nInvalid size to read\n");
		kernel->machine->WriteRegister(-2, 1);
		return;
	}

	OpenFileId fileID = kernel->machine->ReadRegister(6); // file ID

	if (fileID <= -1)
	{
		printf("\nCannot open file with descriptor ID %d\n", fileID);
		kernel->machine->WriteRegister(2, -1);
		return;
	}

	if (fileID == 0 || fileID == 1) {
		printf("\nConsole IO detected!! Not a file to read or write\n");
		kernel->machine->WriteRegister(2, -1);
		return;
	}

	char *buffer = new char[size]; // allocate memory for the buffer
	int nBytes = ReadPartial(fileID, buffer, size);

	System2User(virtAddr, nBytes, buffer);
	kernel->machine->WriteRegister(2, nBytes);
}

void handle_SC_Write() {
	int virtAddr = kernel->machine->ReadRegister(4);	  // address of the buffer
	char* buffer = User2System(virtAddr, MAX_LENGTH_STRING);
	int size = kernel->machine->ReadRegister(5);		  // size to write
	
	int actual_size_of_buffer = strlen(buffer);
	
	OpenFileId fileID = kernel->machine->ReadRegister(6); // file ID

	if (fileID <= -1)
	{
		printf("\nCannot open file with descriptor ID %d\n", fileID);
		kernel->machine->WriteRegister(2, -1);
		return;
	}

	if (fileID == 0 || fileID == 1) {
		printf("\nConsole IO detected!! Not a file to read or write\n");
		kernel->machine->WriteRegister(2, -1);
		return;
	}

	size = (actual_size_of_buffer <= size) ? actual_size_of_buffer : size;
	try {
		WriteFile(fileID, buffer, size);
		kernel->machine->WriteRegister(2, size);
		return;
	}
	catch (const std::exception &e) {
		printf("Error: %s when write to file\n", e.what());
		kernel->machine->WriteRegister(2,-1);
		return;
	}
	
	kernel->machine->WriteRegister(2, 1);
}

void ExceptionHandler(ExceptionType which)
{
	int type = kernel->machine->ReadRegister(2);

	DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");

	switch (which)
	{
	case NoException:
		return;
	case PageFaultException:
		DEBUG(dbgAddr, "\nPage not found");
		printf("\n\n Page not found");
		SysHalt();
		break;
	case ReadOnlyException:
		DEBUG(dbgAddr, "\nCan not attemp to write to read-only zone");
		printf("\n\n Can not attemp to write to read-only zone");
		SysHalt();
		break;
	case BusErrorException:
		DEBUG(dbgAddr, "\nTranslation resulted in an invalid physical address.");
		printf("\n\n Translation resulted in an invalid physical address");
		SysHalt();
		break;
	case AddressErrorException:
		DEBUG(dbgAddr, "\nUnaligned reference or one that was beyond the end of the address space");
		printf("\n\n Unaligned reference or one that was beyond the end of the address space");
		SysHalt();
		break;
	case OverflowException:
		DEBUG(dbgAddr, "\nInteger overflow in add or sub");
		printf("\n\n Integer overflow in add or sub");
		SysHalt();
		break;
	case IllegalInstrException:
		DEBUG(dbgAddr, "\nUnimplemented or reserved instr");
		printf("\n\n Unimplemented or reserved instr");
		SysHalt();
		break;
	case NumExceptionTypes:
		DEBUG(dbgAddr, "\nIncorrect numeric data type");
		printf("\n\n Incorrect numeric data type");
		SysHalt();
		break;
	case SyscallException:
		switch (type)
		{
		case SC_Halt:
			DEBUG(dbgSys, "Shutdown, initiated by user program\n");
			SysHalt();
			ASSERTNOTREACHED();
			break;
		case SC_Add:
		{
			DEBUG(dbgSys, "Add " << kernel->machine->ReadRegister(4) << " + " << kernel->machine->ReadRegister(5) << "\n");

			/* Process SysAdd Systemcall*/
			int result;
			result = SysAdd(/* int op1 */ (int)kernel->machine->ReadRegister(4),
							/* int op2 */ (int)kernel->machine->ReadRegister(5));

			DEBUG(dbgSys, "Add returning with " << result << "\n");
			/* Prepare Result */
			kernel->machine->WriteRegister(2, (int)result);

			/* Modify return point */

			increasePC();

			return;

			ASSERTNOTREACHED();

			break;
		}

		// Cau 6
		case SC_PrintChar:
		{
			char c = kernel->machine->ReadRegister(4);
			kernel->synchConsoleOut->PutChar(c);
			increasePC();
			return;
		}

		// Cau 4
		case SC_PrintNum:
		{
			handle_SC_PrintNum();
			increasePC();
			return;
		}

		// Cau 9
		case SC_PrintString:
		{
			handle_SC_PrintString();
			increasePC();
			return;
		}

		// Cau 5
		case SC_ReadChar:
		{
			handle_SC_ReadChar();
			increasePC();
			return;
		}

		// Cau 7
		case SC_RandomNum:
		{
			int result;
			RandomInit(time(NULL));
			result = RandomNumber();
			kernel->machine->WriteRegister(2, result);
			increasePC();
			return;
		}

		// Cau 3
		case SC_ReadNum:
		{
			handle_SC_ReadNum();
			increasePC();
			return;
		}

		// Cau 8
		case SC_ReadString:
		{
			handle_SC_ReadString();
			increasePC();
			return;
		}

		case SC_Create:
		{
			handle_SC_Create();
			increasePC();
			return;
		}
		case SC_Open:
		{
			handle_SC_Open();
			increasePC();
			return;
		}
		case SC_Read:
		{
			handle_SC_Read();
			increasePC();
			return;
		}
		case SC_Close:
		{
			handle_SC_Close();
			increasePC();
			return;
		}
		case SC_Write: {
			handle_SC_Write();
			increasePC();
			return;
		}
		default:
			cerr << "Unexpected system call " << type << "\n";
			break;
		}
		break;
	default:
		cerr << "Unexpected user mode exception" << (int)which << "\n";
		break;
	}
	ASSERTNOTREACHED();
}
