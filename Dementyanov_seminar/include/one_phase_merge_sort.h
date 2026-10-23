#pragma once
#include <cstddef>
#include <string>

// Однофазная сортировка простым слиянием (4 временных файла).
template <typename T>
void onePhaseMergeSort(const std::string& inputFile, const std::string& tempDir,
    std::size_t runSize, bool keepTemp = false);