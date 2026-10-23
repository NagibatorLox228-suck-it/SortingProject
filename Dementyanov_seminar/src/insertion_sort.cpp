#include "insertion_sort.h"

template <typename T>
void insertionSort(T* arr, int size)
{
    if (arr == nullptr || size < 2) return;

    for (int i = 1; i < size; ++i)
    {
        T key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j] > key)
        {
            arr[j + 1] = arr[j];
            --j;
        }
        arr[j + 1] = key;
    }
}

template void insertionSort<int>(int*, int);
template void insertionSort<double>(double*, int);