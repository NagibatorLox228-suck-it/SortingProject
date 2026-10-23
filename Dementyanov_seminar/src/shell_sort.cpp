#include "shell_sort.h"

template <typename T>
void shellSort(T* arr, int size)
{
    if (arr == nullptr || size < 2) return;

    int gap = 1;
    while (gap < size / 3)
        gap = gap * 3 + 1;

    for (; gap >= 1; gap /= 3)
    {
        for (int i = gap; i < size; ++i)
        {
            T temp = arr[i];
            int j = i;
            while (j >= gap && arr[j - gap] > temp)
            {
                arr[j] = arr[j - gap];
                j -= gap;
            }
            arr[j] = temp;
        }
    }
}

template void shellSort<int>(int*, int);
template void shellSort<double>(double*, int);