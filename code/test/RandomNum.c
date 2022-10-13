#include "syscall.h"

int main(){
    
    int r;
    r = RandomNum();
    PrintString("Your random number is: ");
    PrintNum(r);
    PrintChar('\n');
    Halt();

}