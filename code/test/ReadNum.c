#include "syscall.h"


int main()
{
    int a;
    a=ReadNum();
    PrintNum(a);
    PrintChar('\n');
    
    Halt();
}