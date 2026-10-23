#pragma once
#include <algorithm>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <limits>
#include <stdexcept>
#include <string>
#include <system_error>
#include <vector>

// Вспомогательные функции для внешних сортировок.
// Файлы текстовые: по одному числу в строке.
namespace merge_utils
{
    // Последовательное чтение чисел из файла: в памяти только текущий элемент.
    template <typename T>
    class Reader
    {
    public:
        explicit Reader(const std::string& path) : in_(path)
        {
            if (!in_) throw std::runtime_error("Cannot open file: " + path);
            fetch();
        }
        bool valid() const { return valid_; }
        const T& current() const { return cur_; }
        void fetch() { valid_ = static_cast<bool>(in_ >> cur_); }

    private:
        std::ifstream in_;
        T cur_{};
        bool valid_ = false;
    };

    // Запись чисел в файл (файл перезаписывается).
    template <typename T>
    class Writer
    {
    public:
        explicit Writer(const std::string& path) : out_(path, std::ios::trunc)
        {
            if (!out_) throw std::runtime_error("Cannot create file: " + path);
            out_.precision(std::numeric_limits<T>::max_digits10); // double без потерь
        }
        void write(const T& x) { out_ << x << '\n'; }

    private:
        std::ofstream out_;
    };

    inline std::string tempPath(const std::string& dir, const std::string& name)
    {
        return (std::filesystem::path(dir) / name).string();
    }

    inline void removeIfExists(const std::string& path)
    {
        std::error_code ec;
        std::filesystem::remove(path, ec);
    }

    // Копирует from поверх to и удаляет from.
    inline void replaceFile(const std::string& from, const std::string& to)
    {
        std::filesystem::copy_file(from, to,
            std::filesystem::copy_options::overwrite_existing);
        removeIfExists(from);
    }

    // Чтение кусками по runSize элементов, сортировка куска в памяти,
    // запись серий поочерёдно в f1 и f2. Возвращает число серий.
    template <typename T>
    std::size_t distributeSorted(const std::string& in, const std::string& f1,
        const std::string& f2, std::size_t runSize)
    {
        Reader<T> src(in);
        Writer<T> o1(f1), o2(f2);
        std::vector<T> run;
        std::size_t count = 0;
        bool first = true;

        while (src.valid())
        {
            run.clear();
            while (run.size() < runSize && src.valid())
            {
                run.push_back(src.current());
                src.fetch();
            }
            std::sort(run.begin(), run.end());

            Writer<T>& out = first ? o1 : o2;
            for (const T& x : run) out.write(x);
            first = !first;
            ++count;
        }
        return count;
    }

    // Раскладка серий фиксированной длины len поочерёдно в f1 и f2 (без сортировки).
    template <typename T>
    void distributeFixed(const std::string& in, const std::string& f1,
        const std::string& f2, std::size_t len)
    {
        Reader<T> src(in);
        Writer<T> o1(f1), o2(f2);
        bool first = true;
        std::size_t cnt = 0;

        while (src.valid())
        {
            (first ? o1 : o2).write(src.current());
            src.fetch();
            if (++cnt == len)
            {
                cnt = 0;
                first = !first;
            }
        }
    }

    // Раскладка естественных серий (неубывающих кусков) поочерёдно в f1 и f2.
    // Возвращает число серий.
    template <typename T>
    std::size_t distributeNatural(const std::string& in, const std::string& f1,
        const std::string& f2)
    {
        Reader<T> src(in);
        Writer<T> o1(f1), o2(f2);
        if (!src.valid()) return 0;

        std::size_t runs = 1;
        bool first = true;
        T prev = src.current();

        while (src.valid())
        {
            const T x = src.current();
            if (x < prev)          // серия закончилась
            {
                first = !first;
                ++runs;
            }
            (first ? o1 : o2).write(x);
            prev = x;
            src.fetch();
        }
        return runs;
    }

    // Слияние одной серии (до len элементов) из a с одной серией из b.
    template <typename T>
    void mergeFixed(Reader<T>& a, Reader<T>& b, Writer<T>& out, std::size_t len)
    {
        std::size_t ca = a.valid() ? len : 0;
        std::size_t cb = b.valid() ? len : 0;

        while (ca > 0 || cb > 0)
        {
            bool takeA;
            if (ca == 0)       takeA = false;
            else if (cb == 0)  takeA = true;
            else               takeA = !(b.current() < a.current());

            if (takeA)
            {
                out.write(a.current());
                a.fetch();
                --ca;
                if (!a.valid()) ca = 0;
            }
            else
            {
                out.write(b.current());
                b.fetch();
                --cb;
                if (!b.valid()) cb = 0;
            }
        }
    }

    // Слияние одной естественной серии из a с одной серией из b.
    // Серия заканчивается, когда следующий элемент меньше предыдущего или файл кончился.
    template <typename T>
    void mergeNatural(Reader<T>& a, Reader<T>& b, Writer<T>& out)
    {
        bool endA = !a.valid();
        bool endB = !b.valid();

        while (!endA || !endB)
        {
            bool takeA;
            if (endA)       takeA = false;
            else if (endB)  takeA = true;
            else            takeA = !(b.current() < a.current());

            if (takeA)
            {
                const T prev = a.current();
                out.write(prev);
                a.fetch();
                if (!a.valid() || a.current() < prev) endA = true;
            }
            else
            {
                const T prev = b.current();
                out.write(prev);
                b.fetch();
                if (!b.valid() || b.current() < prev) endB = true;
            }
        }
    }
}