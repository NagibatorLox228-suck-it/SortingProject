#define CATCH_CONFIG_MAIN   // main() генерируется ТОЛЬКО в одном .cpp
#include <catch2/catch.hpp>

#include <algorithm>
#include <cstdio>
#include <fstream>
#include <random>
#include <string>
#include <vector>
#include <climits>
#include <filesystem>
#include <functional>
#include <iomanip>
#include <limits>
#include "insertion_sort.h"
#include "selection_sort.h"
#include "exchange_sort.h"
#include "bubble_sort.h"
#include "optimized_bubble_sort.h"
#include "binary_insertion_sort.h"
#include "shaker_sort.h"
#include "shell_sort.h"
#include "quick_sort.h"
#include "two_phase_merge_sort.h"
#include "one_phase_merge_sort.h"
#include "natural_merge_sort.h"

// ============================================================
//  Внутренние сортировки (9 штук)
// ============================================================

template <typename T>
struct InternalSort
{
    const char* name;
    void (*fn)(T*, int);
};

template <typename T>
std::vector<InternalSort<T>> internalSorts()
{
    return {
        { "insertion",         insertionSort<T> },
        { "selection",         selectionSort<T> },
        { "exchange",          exchangeSort<T> },
        { "bubble",            bubbleSort<T> },
        { "optimized_bubble",  optimizedBubbleSort<T> },
        { "binary_insertion",  binaryInsertionSort<T> },
        { "shaker",            shakerSort<T> },
        { "shell",             shellSort<T> },
        { "quick",             quickSort<T> }
    };
}

// Прогоняет входные данные через все внутренние сортировки
// и сравнивает результат с std::sort.
template <typename T>
void checkAllInternal(const std::vector<T>& input)
{
    std::vector<T> expected = input;
    std::sort(expected.begin(), expected.end());

    for (const auto& s : internalSorts<T>())
    {
        INFO("Sort: " << s.name);
        std::vector<T> v = input;
        s.fn(v.data(), static_cast<int>(v.size()));
        REQUIRE(v == expected);
    }
}

TEST_CASE("Internal: nullptr and zero size do not crash", "[internal][edge]")
{
    for (const auto& s : internalSorts<int>())
    {
        INFO("Sort: " << s.name);
        REQUIRE_NOTHROW(s.fn(nullptr, 0));
        REQUIRE_NOTHROW(s.fn(nullptr, 5));

        int dummy = 42;
        REQUIRE_NOTHROW(s.fn(&dummy, 0));
        REQUIRE(dummy == 42);
    }
}

TEST_CASE("Internal: empty array", "[internal][edge]")
{
    checkAllInternal<int>({});
    checkAllInternal<double>({});
}

TEST_CASE("Internal: single element", "[internal][edge]")
{
    checkAllInternal<int>({ 7 });
    checkAllInternal<double>({ -3.5 });
}

TEST_CASE("Internal: two elements", "[internal][edge]")
{
    checkAllInternal<int>({ 2, 1 });
    checkAllInternal<int>({ 1, 2 });
    checkAllInternal<int>({ 5, 5 });
}

TEST_CASE("Internal: already sorted array", "[internal]")
{
    checkAllInternal<int>({ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 });
    checkAllInternal<double>({ -2.5, -1.0, 0.0, 0.5, 3.14 });
}

TEST_CASE("Internal: reverse sorted array", "[internal]")
{
    checkAllInternal<int>({ 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 });
    checkAllInternal<double>({ 3.14, 0.5, 0.0, -1.0, -2.5 });
}

TEST_CASE("Internal: array with duplicates", "[internal]")
{
    checkAllInternal<int>({ 5, 2, 5, 1, 2, 5, 1, 1, 9, 2 });
    checkAllInternal<double>({ 1.5, 0.5, 1.5, 0.5, 1.5 });
}

TEST_CASE("Internal: all elements equal", "[internal][edge]")
{
    checkAllInternal<int>({ 4, 4, 4, 4, 4, 4, 4 });
}

TEST_CASE("Internal: negative and mixed values", "[internal]")
{
    checkAllInternal<int>({ 0, -5, 3, -1, 8, -10, 2, 0 });
    checkAllInternal<double>({ 0.1, -0.1, 100.25, -100.25, 0.0, 1e-6, -1e-6 });
}

TEST_CASE("Internal: extreme int values", "[internal][edge]")
{
    checkAllInternal<int>({ INT_MAX, 0, INT_MIN, 1, -1, INT_MAX, INT_MIN });
}

TEST_CASE("Internal: random arrays of different sizes", "[internal][random]")
{
    std::mt19937 gen(12345);   // фиксированный seed -> тест воспроизводим
    std::uniform_int_distribution<int> distInt(-1000, 1000);
    std::uniform_real_distribution<double> distDbl(-1000.0, 1000.0);

    for (int size : { 3, 10, 17, 100, 1000 })
    {
        INFO("Size: " << size);

        std::vector<int> vi(size);
        for (auto& x : vi) x = distInt(gen);
        checkAllInternal<int>(vi);

        std::vector<double> vd(size);
        for (auto& x : vd) x = distDbl(gen);
        checkAllInternal<double>(vd);
    }
}

TEST_CASE("Internal: sort works on part of a larger buffer", "[internal][edge]")
{
    // сортируем только первые 5 элементов, хвост не должен измениться
    for (const auto& s : internalSorts<int>())
    {
        INFO("Sort: " << s.name);
        int buf[8] = { 5, 4, 3, 2, 1, 99, 98, 97 };
        s.fn(buf, 5);
        REQUIRE(std::vector<int>(buf, buf + 8) ==
            std::vector<int>{ 1, 2, 3, 4, 5, 99, 98, 97 });
    }
}

// ============================================================
//  Внешние сортировки (3 штуки), текстовые файлы
// ============================================================

namespace fs = std::filesystem;

template <typename T>
struct ExternalSort
{
    const char* name;
    std::function<void(const std::string&, const std::string&, std::size_t)> fn;
};

template <typename T>
std::vector<ExternalSort<T>> externalSorts()
{
    return {
        { "two_phase", [](const std::string& f, const std::string& t, std::size_t r)
                       { twoPhaseMergeSort<T>(f, t, r); } },
        { "one_phase", [](const std::string& f, const std::string& t, std::size_t r)
                       { onePhaseMergeSort<T>(f, t, r); } },
        { "natural",   [](const std::string& f, const std::string& t, std::size_t)
                       { naturalMergeSort<T>(f, t); } }
    };
}

template <typename T>
void writeTxt(const std::string& name, const std::vector<T>& data)
{
    std::ofstream out(name, std::ios::trunc);
    out << std::setprecision(std::numeric_limits<T>::max_digits10);
    for (const T& x : data) out << x << '\n';
}

template <typename T>
std::vector<T> readTxt(const std::string& name)
{
    std::vector<T> res;
    std::ifstream in(name);
    T x;
    while (in >> x) res.push_back(x);
    return res;
}

// Сортирует файл тремя внешними сортировками при разных runSize,
// сравнивает с std::sort и проверяет, что временные файлы удалены.
template <typename T>
void checkAllExternal(const std::vector<T>& input)
{
    std::vector<T> expected = input;
    std::sort(expected.begin(), expected.end());
    fs::create_directories("test_data");

    for (const auto& s : externalSorts<T>())
    {
        for (std::size_t runSize : { std::size_t(1), std::size_t(3),
            std::size_t(64), std::size_t(100000) })
        {
            INFO("Sort: " << s.name << ", runSize: " << runSize);
            const std::string file = std::string("test_data/") + s.name + ".txt";
            const std::string tmp = std::string("test_tmp/") + s.name;

            writeTxt<T>(file, input);
            REQUIRE_NOTHROW(s.fn(file, tmp, runSize));
            REQUIRE(readTxt<T>(file) == expected);

            // временные файлы должны быть удалены
            if (fs::exists(tmp)) REQUIRE(fs::is_empty(tmp));

            fs::remove(file);
        }
    }
}

TEST_CASE("External: empty file", "[external][edge]")
{
    checkAllExternal<int>({});
    checkAllExternal<double>({});
}

TEST_CASE("External: single element", "[external][edge]")
{
    checkAllExternal<int>({ 42 });
    checkAllExternal<double>({ 3.5 });
}

TEST_CASE("External: two elements", "[external][edge]")
{
    checkAllExternal<int>({ 2, 1 });
    checkAllExternal<int>({ 1, 2 });
}

TEST_CASE("External: already sorted file", "[external]")
{
    checkAllExternal<int>({ 1, 2, 3, 4, 5, 6, 7, 8 });
    checkAllExternal<double>({ -1.5, 0.0, 2.5, 10.25 });
}

TEST_CASE("External: reverse sorted file", "[external]")
{
    checkAllExternal<int>({ 8, 7, 6, 5, 4, 3, 2, 1 });
    checkAllExternal<double>({ 10.25, 2.5, 0.0, -1.5 });
}

TEST_CASE("External: duplicates and negatives", "[external]")
{
    checkAllExternal<int>({ 5, -2, 5, 1, -2, 5, 0, 1, 9, -2 });
    checkAllExternal<int>({ 3, 3, 3, 3, 3 });
}

TEST_CASE("External: size is not a power of two", "[external]")
{
    checkAllExternal<int>({ 9, 3, 7, 1, 8, 2, 6 });
    checkAllExternal<int>({ 4, 1, 3, 9, 7, 5, 2, 8, 6, 0, 11 });
}

TEST_CASE("External: the variant array", "[external]")
{
    checkAllExternal<int>({ 3, 19, 10, 1, 15, 4, 11, 20 });
}

TEST_CASE("External: random files of different sizes", "[external][random]")
{
    std::mt19937 gen(54321);
    std::uniform_int_distribution<int> distInt(-1000, 1000);
    std::uniform_real_distribution<double> distDbl(-1000.0, 1000.0);

    for (int size : { 3, 10, 33, 500, 2000 })
    {
        INFO("Size: " << size);

        std::vector<int> vi(size);
        for (auto& x : vi) x = distInt(gen);
        checkAllExternal<int>(vi);

        std::vector<double> vd(size);
        for (auto& x : vd) x = distDbl(gen);
        checkAllExternal<double>(vd);
    }
}

TEST_CASE("External: keepTemp leaves temporary files", "[external]")
{
    fs::create_directories("test_data");
    const std::string file = "test_data/keep.txt";
    writeTxt<int>(file, { 5, 3, 1, 4, 2 });

    twoPhaseMergeSort<int>(file, "test_tmp/keep", 2, true);

    REQUIRE(fs::exists("test_tmp/keep/two_phase_b.txt"));
    REQUIRE(readTxt<int>(file) == std::vector<int>{ 1, 2, 3, 4, 5 });

    fs::remove_all("test_tmp/keep");
    fs::remove(file);
}

TEST_CASE("External: nonexistent file throws", "[external][edge]")
{
    const std::string missing = "this_file_does_not_exist.txt";
    REQUIRE_THROWS(twoPhaseMergeSort<int>(missing, "test_tmp/x", 4));
    REQUIRE_THROWS(onePhaseMergeSort<int>(missing, "test_tmp/x", 4));
    REQUIRE_THROWS(naturalMergeSort<int>(missing, "test_tmp/x"));
}

TEST_CASE("Random: new data on every run", "[random][external][internal]")
{
    std::random_device rd;
    const unsigned seed = rd();
    INFO("Seed: " << seed);          

    std::mt19937 gen(seed);
    std::uniform_int_distribution<int> distSize(0, 3000);
    std::uniform_int_distribution<int> distInt(-100000, 100000);

    for (int round = 0; round < 5; ++round)
    {
        std::vector<int> v(distSize(gen));
        for (auto& x : v) x = distInt(gen);

        checkAllInternal<int>(v);
        checkAllExternal<int>(v);
    }
}