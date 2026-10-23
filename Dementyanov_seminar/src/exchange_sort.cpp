#include "exchange_sort.h"
#include <utility>

template <typename T>
void exchangeSort(T* arr, int size)
{
    if (arr == nullptr || size < 2) return;

    for (int i = 0; i < size - 1; ++i)
    {
        for (int j = i + 1; j < size; ++j)
        {
            if (arr[j] < arr[i])
                std::swap(arr[i], arr[j]);
        }
    }
}

template void exchangeSort<int>(int*, int);
template void exchangeSort<double>(double*, int);