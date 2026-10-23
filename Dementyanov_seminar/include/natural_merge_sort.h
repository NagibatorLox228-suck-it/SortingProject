#pragma once
#include <string>

// Естественная сортировка слиянием: серии - уже упорядоченные куски данных,
// поэтому размер блока в памяти не нужен.
template <typename T>
void naturalMergeSort(const std::string& inputFile, const std::string& tempDir,
    bool keepTemp = false);