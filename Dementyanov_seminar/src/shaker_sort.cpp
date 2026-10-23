#include "shaker_sort.h"
#include <utility>

template <typename T>
void shakerSort(T* arr, int size)
{
    if (arr == nullptr || size < 2) return;

    int left = 0;
    int right = size - 1;

    while (left < right)
    {
        int last = left;
        for (int i = left; i < right; ++i)
        {
            if (arr[i] > arr[i + 1])
            {
                std::swap(arr[i], arr[i + 1]);
                last = i;
            }
        }
        right = last;

        last = right;
        for (int i = right; i > left; --i)
        {
            if (arr[i - 1] > arr[i])
            {
                std::swap(arr[i - 1], arr[i]);
                last = i;
            }
        }
        left = last;
    }
}

template void shakerSort<int>(int*, int);
template void shakerSort<double>(double*, int);