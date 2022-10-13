#include "syscall.h"

#define MAX_SIZE_ARR 100 // n <= 100

void bubbleSort(int arr[], int n, char type)
{
    int i, j, temp;
    if (type == 'a') // neu type == 1 thi sap xep tang dan
    {
        for (i = 0; i < n - 1; i++)
            for (j = 0; j < n - i - 1; j++)
                if (arr[j] > arr[j + 1])
                {
                    temp = arr[j + 1];
                    arr[j + 1] = arr[j];
                    arr[j] = temp;
                }
    }
    if (type == 'd') // sap xep giam dan bang bubble sort
    {
        for (i = 0; i < n - 1; i++)
            for (j = 0; j < n - i - 1; j++)
                if (arr[j] < arr[j + 1])
                {
                    temp = arr[j + 1];
                    arr[j + 1] = arr[j];
                    arr[j] = temp;
                }
    }
}
void showArr(int arr[], int n)
{ // print array
    int i;
    i = 0;
    PrintString("Array: \n");
    for (i = 0; i < n; i++)
    {
        PrintNum(arr[i]);
        PrintChar(' ');
    }
    PrintChar('\n');
}

int main()
{
    int n;
    int Arr[MAX_SIZE_ARR];
    int i;
    char type;
    i = 0;
    // Input n

    do {
        PrintString("Input size of array: ");
        n = ReadNum();
        if (n > 100 || n <= 0) {
            PrintString("Range of size [1,100]. Please input again\n");
        }
    } while (n > 100 || n <= 0);

    // Input cac phan tu array
    PrintString("Input array: \n");
    while (i < n)
    {
        PrintString("A[");
        PrintNum(i);
        PrintString("]:");
        Arr[i] = ReadNum();
        i++;
    }
    PrintString("Array before Sorting: \n");
    showArr(Arr, n);
    do
    {
        PrintString("Input a to sort this array in ascending, or d to sort it in descending: \n");
        type = ReadChar();
    } while (type != 'a' && type != 'd');
    
    bubbleSort(Arr, n, type);
    PrintString("Array after Sorting: \n");
    showArr(Arr, n);
    Halt();
}

