#include <iostream>
#include <functional>
#include <vector>
#include <thread>
#include <map>
#include <random>
#include <algorithm>

/* Header */
template<typename it_t>
using sort_f = std::function<void(it_t &&begin, it_t &&end)>;

/* Sorting algo`s */

/** @name Useless Sort */
template<typename it_t>
void my_sort(it_t &&begin, it_t &&end) {
    std::string stuff = {"Stupid stuff to slow things down"};
    auto &counter = const_cast<char&>(*stuff.c_str());
    for(;counter >= 0x20; --counter){
        std::cout << stuff << "\r" << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }
    std::cout << "                                     \r";
}

/** @name std::sort */
template<typename it_t>
void std_sort(it_t &&begin, it_t &&end) {
    std::sort(begin, end);
}

/** @name Bubble Sort */
template<typename it_t>
void bubble_sort(it_t &&begin, it_t &&end) {
    /* Iterate untill no swap required. O(.. * logN) */
    for (bool compleated; !compleated;) {
        compleated = true;

        /* Iterate over and swap if required. O(N * ..)  */
        for (auto element = begin; element != end - 1 ; ++element) {
            auto next_element = element + 1;
            if(*element > *next_element) {
                std::iter_swap(element, next_element);
                compleated = false;
            }
        }

        /* After each pass we guranted feed MAX element 
           at the end, drop a checking the end next time */
        --end;
    }
}

/* Settings */
using test_val_t = unsigned long long int;
using test_struct_t = std::vector<test_val_t>;
constexpr auto test_size_array = {5, 8, 129, 9453, 2348990, 20457645};
const std::map<std::string, sort_f<test_struct_t::iterator>> list_of_tests = {
                                {"Useless Stuff", my_sort<test_struct_t::iterator> },
                                {"Bubble Sort", bubble_sort<test_struct_t::iterator> },
                                {"std::sort", std_sort<test_struct_t::iterator> }};

/* Utility`s */
template<typename it_t>
bool is_sorted_custom(it_t &&begin, it_t &&end) {
    /* Iterate over and check if next element less then previous */
    for (auto element = begin; element != end - 1 ; ++element)
        if(*element > *(element + 1))
            return false;
    return true;
}

template<typename it_t>
bool range_print(it_t &&begin, it_t &&end, std::size_t max_width) {

    auto data_size = std::distance(begin, end);
    auto print_step = data_size / max_width;
    auto skip_commas = print_step <= 1;

    print_step = print_step ? print_step : 1;

    /* Iterate over and print: a value, comma in between
       if step is 1 and double-dot if some element skipped */
    std::cout << "\t\t[";
    for (auto element = begin; element < end; element += print_step)
        std::cout << std::hex << *element << std:: dec
            << std::string((element < end - print_step) ? (print_step != 1 ? ".." : ", ") : "");
    std::cout << "]" << std::endl;
}

/* Test suite */
int main (int argc, const char**argv) {
    std::cout << ">~~< Program Begin >~~<" << std::endl;

    /* Test arrays of different length */
    for (auto test_size : test_size_array) {
        std::cout << " --> Test with Array Length: " << test_size << std::endl;

        /* Allocate new array for tests */
        test_struct_t unsorted_list;
        unsorted_list.resize(test_size);

        /* Use Mersenne Twister random number engine to fill test array */
        std::random_device rd;
        std::mt19937 gen(rd());
        for(auto &el : unsorted_list) el = gen();

        /* Test each algoritm */
        for(auto &test_pair : list_of_tests) {
            std::cout << "\t --> " << test_pair.first << std::endl;

            /* Copy same data for each test */
            test_struct_t test_list = {unsorted_list};

            /* Test */
            auto start = std::chrono::high_resolution_clock::now();
            test_pair.second(unsorted_list.begin(), unsorted_list.end());
            auto end = std::chrono::high_resolution_clock::now();

            /* Show result */
            const std::chrono::duration<double, std::milli> duration = end - start;
            std::cout << "\t     " << duration.count() << " ms   \t--> "
                      << std::string(is_sorted_custom(unsorted_list.begin(), unsorted_list.end()) ? "OK" : "ERROR")
                      << std::endl;
        }
    }

    /* Exit  */
    std::cout << std::endl << std::endl << "...press ^M to exit" << std::flush;
    std::cin.get();

    std::cout << ">~~<   Exit 0    >~~<" << std::endl;
    return 0;

} 