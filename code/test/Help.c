#include"syscall.h"

int main()
{   
    // Thong tin nhom
    PrintString("Group Information: 3 members \n");
    PrintString("  - First member: Trieu Quoc Thai, ID: 20120370 \n");
    PrintString("  - Second member: Tran Van That, ID: 20120376 \n");
    PrintString("  - Last member: Nguyen Phuc Thuan, ID: 20120380 \n");

    // Mo ta chuong trinh my_sort
    PrintString("Description Program: Bubble_sort \n");
    PrintString("  - Input n by system call ReadNum, handle condition n in [1;100] \n");
    PrintString("  - Input n value of array by system call ReadNum \n");
    PrintString("  - Print array before sorted to console \n");
    PrintString("  - Select type-sort: sort-ascending (Enter a) - sort-descending (Enter d), if don't enter a or d then re-enter \n");
    PrintString("  - Sort array \n");
    PrintString("  - Print array after sorted to console \n");

    // Mo ta chuong trinh ascii
    PrintString("Description Program: ascii \n");
    PrintString("  - Using one variable type char to print readable characters (32 - 127), print eight characters per line \n");
    PrintString("  - Using system call PrintChar to print ascii table \n");
    
    Halt();
}