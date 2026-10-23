#include "bubble_sort.h"
#include <utility>

template <typename T>
void bubbleSort(T* arr, int size)
{
    if (arr == nullptr || size < 2) return;

    for (int i = 0; i < size - 1; ++i)
    {
        for (int j = 0; j < size - 1 - i; ++j)
        {
            if (arr[j] > arr[j + 1])
                std::swap(arr[j], arr[j + 1]);
        }
    }
}

template void bubbleSort<int>(int*, int);
template void bubbleSort<double>(double*, int);