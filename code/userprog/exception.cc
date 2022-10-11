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

		case SC_PrintChar:
		{
			// input: 1 char
			// output: none
			// muc dich: in ra console 1 ki tu char
			char c = kernel->machine->ReadRegister(4);
			kernel->synchConsoleOut->PutChar(c);
			increasePC();
			return;
		}

		case SC_PrintNum:
		{
			// Input: 1 so nguyen kieu int
			// Return: none
			// Tac dung: in mot so nguyen kieu int ra console
			int number;
			int temp;
			char *buffer;
			int countDigits, indexStart, MAX_SIZE_BUFFER;
			int i;
			countDigits = 0; // So digit cua number
			indexStart = 0;	 // Vi tri bat dau in chu so

			MAX_SIZE_BUFFER = 11; // 1: sign, 10: digits vi so nguyen kieu int toi da can 10 digits
			number = kernel->machine->ReadRegister(4);

			if (number == 0)
			{										   // truong hop de xu ly nhat
				kernel->synchConsoleOut->PutChar('0'); // In ra man hinh so 0
				increasePC();
				return;
			}

			/*Chuyen so thanh chuoi roi in ra man hinh*/
			if (number < 0)
			{

				number = -number; // Chuyen thanh so duong => easy
				indexStart = 1;
			}

			temp = number;	 // bien tam cho number
			while (temp > 0) // dem so digits
			{
				countDigits++;
				temp /= 10;
			}

			// Tao buffer chuoi de in ra man hinh
			// int: [-2147483648, 2147483647]
			buffer = new char[MAX_SIZE_BUFFER + 1];
			for (i = countDigits - 1 + indexStart; i >= indexStart; i--)
			{
				buffer[i] = (char)((number % 10) + '0'); // Lay tung chu so vao buffer
				number /= 10;
			}
			if (indexStart == 1)
			{ // Neu la so am
				buffer[0] = '-';
				buffer[countDigits + 1] = 0;
				for (i = 0; i <= countDigits; i++)
				{
					kernel->synchConsoleOut->PutChar(buffer[i]);
				}

				delete buffer;
			}
			else
			{ // Neu la so duong
				buffer[countDigits] = 0;
				for (i = 0; i <= countDigits - 1; i++)
				{
					kernel->synchConsoleOut->PutChar(buffer[i]);
				}

				delete buffer;
			}
			increasePC();

			return;
		}
		case SC_ReadNum:
		{
			// input : None
			// output : So nguyen hoac 0
			long long res; // khai bao ket qua ra ve kieu long long de sau nay ep kieu
			char c;
			bool isNegative;
			bool isEnd, flagZero;
			isEnd = false;
			isNegative = false;
			flagZero = false;
			res = 0;
			while ((c = kernel->synchConsoleIn->GetChar()) == ' ')
			{ // kiem tra xem co khoang trang o dau hay khong
			}

			if (c == '-') // check xem co phai so am hay khong
				isNegative = true;
			else if (c >= '0' &&
					 c <= '9') // check xem cac ki tu co nam tu 0->9 hay khong
				res = res * 10 + (c - '0');
			else if (c == '\n')
			{
				DEBUG(dbgSys, "\nNguoi dung chua nhap so");
				printf("\nNguoi dung chua nhap so");
				res = 0;
				kernel->machine->WriteRegister(2, int(res));
				increasePC();
				return;
			}
			else
			{
				DEBUG(dbgSys, "\nError!!!Nguoi dung nhap ki tu sai so nguyen");
				res = 0;
				kernel->machine->WriteRegister(2, int(res));
				printf("khong phai so nguyen");
				// while(kernel->synchConsoleIn->GetChar()!='\n')
				// {

				// }
				clearInputBuffer();
				increasePC();
				return;
			}
			while ((c = kernel->synchConsoleIn->GetChar()) != '\n')
			{ // doc den luc Enter xuong dong

				if (c >= '0' && c <= '9' && isEnd == false)
				{
					res = res * 10 + (c - '0');
					flagZero = true;
				}
				else if (c == ' ')
				{ // neu nhan them khoang trang thi den day la ket thuc
					isEnd = true;
				}
				else
				{ // neu la cac ki tu khac thi loi
					DEBUG(dbgSys, "\nError!!!This is not a number");
					res = 0;
					;
					printf("\nError!!!This is not a number");
					kernel->machine->WriteRegister(2, int(res));

					clearInputBuffer();
					// while(kernel->synchConsoleIn->GetChar()!='\n')
					// {

					// }
					increasePC();

					return;
				}
			}
			if (isNegative)
				res = -res;
			if (res > INT_MAX)
			{ // kiem tra so nguyen co lon hon quy dinh hay khong INT MAX 2147483647
				DEBUG(dbgSys, "\nError!!!This number is so large");
				printf("\nSo qua lon");
				res = 0;
				kernel->machine->WriteRegister(2, int(res));

				increasePC();
				return;
			}
			else if (res < INT_MIN)
			{ // kiem tra so nguyen co be hon quy dinh hay khong INT MIN -2147483647
				DEBUG(dbgSys, "\nError!!!This number is so small");
				printf("\nSo qua be");
				res = 0;
				kernel->machine->WriteRegister(2, int(res));

				increasePC();
				return;
			}
			if (isNegative && res == 0 && !flagZero)
			{ // kiem tra truong hop nguoi dung nhap '-0'
				DEBUG(dbgSys, "\nError!!!This is (-) only");
				res = 0;
				printf("\nKhong phai la so nguyen ma chi la dau (-)");
				kernel->machine->WriteRegister(2, int(res));
				increasePC();
				return;
			}

			DEBUG(dbgSys, "\nThis is a number");
			kernel->machine->WriteRegister(2, int(res));
			// printf("Value: %d", res);
			//  cout << "so do la" << res;

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
