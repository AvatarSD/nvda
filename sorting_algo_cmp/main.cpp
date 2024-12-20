#include <iostream>
#include <functional>
#include <vector>
#include <thread>
#include <map>

/* Header */
template<typename it_t>
using sort_f = std::function<void(it_t &&begin, it_t &&end)>;

/* Sorting algo`s */

/** @name Useless Sort */
template<typename it_t>
void my_sort(it_t &&begin, it_t &&end){
    std::string stuff = {"Stupid stuff to slow things down"};
    auto &counter = const_cast<char&>(*stuff.c_str());
    for(;counter >= 0x20; --counter){
        std::cout << stuff << "\r" << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(80));
    }
    std::cout << std::endl;

    std::sort(begin, end);
}

/* Settings */
using test_val_t = unsigned long long int;
using test_struct_t = std::vector<test_val_t>;
constexpr auto array_test_size = {5, 8, 129, 9453, 2348990};
const std::map<std::string, sort_f<test_struct_t::iterator>> list_of_tests = {
                                {"Useless Test", my_sort<test_struct_t::iterator> }};

/* Utility`s */
void print_array() {}

/* Test suite */
int main (int argc, const char**argv) {

    test_struct_t unsorted_list = {3425,2345,234,523,45,234,5,234,52,345,23,546,23,456,23,45,123};
    
    std::cout << ">~~< Program Begin >~~<" << std::endl;

    for(auto &test_pair : list_of_tests) {
        std::cout << "\t --> " << test_pair.first << std::endl;
        auto start = std::chrono::high_resolution_clock::now();
        test_pair.second(unsorted_list.begin(), unsorted_list.end());
        auto end = std::chrono::high_resolution_clock::now();
        
        const std::chrono::duration<double, std::milli> duration = end - start;
        std::cout << "\t     " << duration.count() << " ms\t  --> OK!" << std::endl;
    }


    /* Exit  */
    std::cout << std::endl << std::endl << "...press ^M to exit" << std::flush;
    std::cin.get();

    std::cout << ">~~<   Exit 0    >~~<" << std::endl;
    return 0;

}