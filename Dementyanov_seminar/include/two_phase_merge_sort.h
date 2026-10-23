#pragma once
#include <cstddef>
#include <string>

// Двухфазная сортировка простым слиянием текстового файла (число в строке).
// inputFile - файл (сортируется на месте), tempDir - папка для временных файлов,
// runSize   - сколько элементов сортируется в памяти за раз,
// keepTemp  - не удалять временные файлы (для демонстрации).
template <typename T>
void twoPhaseMergeSort(const std::string& inputFile, const std::string& tempDir,
    std::size_t runSize, bool keepTemp = false);