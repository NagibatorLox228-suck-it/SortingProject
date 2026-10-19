#include <chrono>
#include <filesystem>
#include <functional>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "two_phase_merge_sort.h"
#include "one_phase_merge_sort.h"
#include "natural_merge_sort.h"

namespace fs = std::filesystem;

struct Algo
{
    std::string tag;
    std::function<void(const std::string&, const std::string&, std::size_t, bool)> run;
};

static std::vector<Algo> algos()
{
    return {
        { "two_phase", [](const std::string& f, const std::string& t, std::size_t r, bool k)
                       { twoPhaseMergeSort<int>(f, t, r, k); } },
        { "one_phase", [](const std::string& f, const std::string& t, std::size_t r, bool k)
                       { onePhaseMergeSort<int>(f, t, r, k); } },
        { "natural",   [](const std::string& f, const std::string& t, std::size_t, bool k)
                       { naturalMergeSort<int>(f, t, k); } }
    };
}

static void writeFile(const std::string& name, const std::vector<int>& v)
{
    std::ofstream out(name, std::ios::trunc);
    for (int x : v) out << x << '\n';
}

static void printFile(const std::string& name)
{
    std::ifstream in(name);
    int x;
    while (in >> x) std::cout << x << ' ';
    std::cout << '\n';
}

static void generateRandomFile(const std::string& name, std::size_t count)
{
    std::random_device rd;                        // новый набор при каждом запуске
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, 999999999);
    std::ofstream out(name, std::ios::trunc);
    for (std::size_t i = 0; i < count; ++i) out << dist(gen) << '\n';
}

// Проверяет, что файл отсортирован и содержит expectedCount чисел
static bool checkSorted(const std::string& name, std::size_t expectedCount)
{
    std::ifstream in(name);
    int prev, x;
    std::size_t n = 0;
    bool ok = true;
    while (in >> x)
    {
        if (n > 0 && x < prev) ok = false;
        prev = x;
        ++n;
    }
    return ok && n == expectedCount;
}

static void smallDemo()
{
    fs::create_directories("data");
    const std::vector<int> variant = { 3, 19, 10, 1, 15, 4, 11, 20 };   
    const std::string input = "data/variant_input.txt";
    writeFile(input, variant);

    std::cout << "=== Variant 7 demo ===\n";
    std::cout << "input:        ";
    printFile(input);

    for (const Algo& a : algos())
    {
        const std::string result = "data/variant_result_" + a.tag + ".txt";
        fs::copy_file(input, result, fs::copy_options::overwrite_existing);
        a.run(result, "data/tmp_variant_" + a.tag, 2, true);
        std::cout << a.tag << ": ";
        printFile(result);
    }
    std::cout << "Файлы в папке: " << fs::absolute("data").string() << "\n\n";
}


int main()
{
    LC_ALL("LC_Russian");
    system("chcp 1251 > nul");
    try
    {
        smallDemo();
        
    }
    catch (const std::exception& e)
    {
        std::cerr << "error: " << e.what() << '\n';
        return 1;
    }
    return 0;
}