#include "syscall.h"

int main()
{
    int a;
    a = ReadNum();
    PrintString("Your number is: ");
    PrintNum(a);
    PrintChar('\n');
    
    Halt();
}