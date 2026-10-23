#include "selection_sort.h"
#include <utility>

template <typename T>
void selectionSort(T* arr, int size)
{
    if (arr == nullptr || size < 2) return;

    for (int i = 0; i < size - 1; ++i)
    {
        int minIndex = i;
        for (int j = i + 1; j < size; ++j)
        {
            if (arr[j] < arr[minIndex])
                minIndex = j;
        }
        if (minIndex != i)
            std::swap(arr[i], arr[minIndex]);
    }
}

template void selectionSort<int>(int*, int);
template void selectionSort<double>(double*, int);