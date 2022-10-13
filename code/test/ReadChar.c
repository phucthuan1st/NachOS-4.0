#include"syscall.h"

int main()
{
    char c = ReadChar();
    PrintString("Your character is: ");
    PrintChar(c);
    PrintChar('\n');
    Halt();
}