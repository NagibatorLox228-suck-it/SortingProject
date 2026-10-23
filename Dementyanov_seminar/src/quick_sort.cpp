#include "quick_sort.h"
#include <utility>

namespace
{
    // Разбиение Хоара, опорный элемент - средний.
    // Рекурсия идёт в меньшую часть, большая обрабатывается циклом
    template <typename T>
    void quickSortImpl(T* arr, int low, int high)
    {
        while (low < high)
        {
            int i = low;
            int j = high;
            T pivot = arr[low + (high - low) / 2];

            while (i <= j)
            {
                while (arr[i] < pivot) ++i;
                while (pivot < arr[j]) --j;
                if (i <= j)
                {
                    std::swap(arr[i], arr[j]);
                    ++i;
                    --j;
                }
            }

            if (j - low < high - i)
            {
                quickSortImpl(arr, low, j);
                low = i;
            }
            else
            {
                quickSortImpl(arr, i, high);
                high = j;
            }
        }
    }
}

template <typename T>
void quickSort(T* arr, int size)
{
    if (arr == nullptr || size < 2) return;
    quickSortImpl(arr, 0, size - 1);
}

template void quickSort<int>(int*, int);
template void quickSort<double>(double*, int);