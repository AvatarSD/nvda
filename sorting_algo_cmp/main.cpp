#include <iostream>
#include <functional>
#include <vector>
#include <thread>
#include <map>
#include <random>
#include <algorithm>
#include <stack>

/* Header */
template<typename it_t>
using sort_f = std::function<void(it_t &&begin, it_t &&end)>;

/* Sorting algo`s */

/** @name Useless Sort */
template<typename it_t>
void not_a_sort(it_t &&begin, it_t &&end) {
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

/** @name std::stable_sort */
template<typename it_t>
void std_stable_sort(it_t &&begin, it_t &&end) {
    std::stable_sort(begin, end);
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

/** @name Non-Recursive Quick Sort */
template<typename it_t>
void nr_quick_sort(it_t &&begin, it_t &&end) {
    std::stack<std::pair<it_t, it_t>> range_stack;
    it_t left, right;

    /* Push initial range */
    range_stack.push({begin, end});

    /* Iterate over all ranges */
    while (!range_stack.empty()) {
        /* Get latest range to sort */
        std::tie(left, right) = range_stack.top();
        range_stack.pop();

        /* If there is more then one element in range*/
        if (left < right) {
            /* Choose middle element as pivot */
            it_t pivot = left + (right - left) / 2;

            /* Move all elements to left or rigth of pivot */
            std::nth_element(left, pivot, right);

            /* Add two sub-ranges for nest cycles to ranges stack */
            range_stack.push({pivot + 1, right});
            range_stack.push({left, pivot});
        }
    }
}

/** @name NR quick sort with O(.. * 2NlogN) numeral(op+() & op/()
          must exist) pivot selection */
template<typename it_t>
void nr_quick_sort_numeral(it_t &&begin, it_t &&end) {
    std::stack<std::pair<it_t, it_t>> range_stack;
    it_t left, right;

    auto chose_pivot = [](auto left, auto right) -> it_t {
        /* Extract position from iterators, not an loop */
        const auto length = std::distance(left, right);

        /* Averaging of all elements */
        auto avg = *left;
        for(auto it = left + 1; it < right; ++it)
            avg += *it;
        auto closest_median = avg / length;

        /* Seeking for  */
        auto closest_it = left;
        for(auto it = left; it < right; ++it)
            if(*it < *closest_it) closest_it = it;

        /* Return closest to average value */
        return closest_it;

    };

    /* Push initial range */
    range_stack.push({begin, end});

    /* Iterate over all ranges */
    while (!range_stack.empty()) {
        /* Get latest range to sort */
        std::tie(left, right) = range_stack.top();
        range_stack.pop();

        /* If there is more then one element in range*/
        if (left < right) {
            /* Choose average element as pivot */
            it_t pivot = chose_pivot(left, right);

            /* Move all elements to left or rigth of pivot */
            std::nth_element(left, pivot, right);

            /* Add two sub-ranges for nest cycles to ranges stack */
            range_stack.push({pivot + 1, right});
            range_stack.push({left, pivot});
        }
    }
}

/** @name Regular quick sort, based on nth_element() */
template<typename it_t>
void quick_sort(it_t begin, it_t end) {
    /* Base case */
    if (end - begin <= 1) return;

    /* Choose middle element as pivot */
    it_t pivot = begin + (end - begin) / 2;

    /* Move all elements to left or rigth of pivot */
    std::nth_element(begin, pivot, end);

    /* Call subroutines for sub-ranges */
    quick_sort(begin, pivot);
    quick_sort(pivot + 1, end);
}

/* Helper */
template<typename it_t>
void chose_pivot_canonical(it_t begin, it_t end, it_t &pivot) {
    /* Move pivot to end */
    std::iter_swap(pivot, end - 1);
    pivot = end - 1;

    /* Iterate over and Swap(j) within next non-swapped(i)
       if element(j) less then pivot */
    it_t i = begin;
    for (it_t j = begin; j < end - 1; ++j) {
        if (*j <= *pivot) {
            // ++i;
            std::iter_swap(i++, j);
        }
    }

    /* Move pivot at the midddle of distribution */
    std::iter_swap(i, pivot);

    /* As it random access iterator, we need manually reposition it */
    pivot = i;
}

/** @name Canonical quick sort with canonical pivot selection */
template<typename it_t>
void quick_sort_canonical(it_t begin, it_t end) {
    /* Base case */
    if (end - begin <= 1) return;

    /* Choose middle element as pivot */
    it_t pivot = begin + (end - begin) / 2;

    /* Move all elements to left or rigth of pivot */
    chose_pivot_canonical(begin, end, pivot);

    /* Call subroutines for sub-ranges */
    quick_sort_canonical(begin, pivot);
    quick_sort_canonical(pivot + 1, end);
}

/** @todo
** @name NR quick sort with canonical pivot selection *
* NR quick sort wirh custom nth_element()
  and memory-expensive pivot redistribvution using insertion to list
* insertion sort
* selection sort
* insertion memory-expensive sort
*/

/* Settings */
using test_val_t = unsigned long long int;
using test_struct_t = std::vector<test_val_t>;
constexpr auto test_size_array = {5, 8, 129, 9453, 34524/*, 2348990, 20457645*/};
const std::map<std::string, sort_f<test_struct_t::iterator>> list_of_tests = {
                                // {"Useless Stuff", not_a_sort<test_struct_t::iterator> },
                                {"Bubble Sort     ", bubble_sort<test_struct_t::iterator> },
                                {"NR Quick Sort   ", nr_quick_sort<test_struct_t::iterator> },
                                {"Numeral QSort   ", nr_quick_sort_numeral<test_struct_t::iterator> },
                                {"Recursive QSort ", quick_sort<test_struct_t::iterator> },
                                {"Canonical QSort ", quick_sort_canonical<test_struct_t::iterator> },
                                {"std::stable_sort", std_stable_sort<test_struct_t::iterator> },
                                {"std::sort       ", std_sort<test_struct_t::iterator> }};

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
void range_print(it_t &&begin, it_t &&end, std::size_t max_width) {

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
        std::cout << "\r --> Test with Array Length: " << test_size << std::endl;

        /* Allocate new array for tests */
        test_struct_t unsorted_list = {}, test_list = {};
        unsorted_list.resize(test_size);

        /* Use Mersenne Twister random number engine to fill test array */
        std::random_device rd;
        std::mt19937 gen(rd());
        for(auto &el : unsorted_list) el = gen();
        range_print(unsorted_list.begin(), unsorted_list.end(), 8);

        /* Test each algoritm */
        for(auto &test_pair : list_of_tests) {
            std::cout << "\t --> " << test_pair.first << std::flush << "   \t";

            /* Copy same data for each test */
            test_list = {unsorted_list};

            /* Test */
            auto start = std::chrono::high_resolution_clock::now();
            test_pair.second(test_list.begin(), test_list.end());
            auto end = std::chrono::high_resolution_clock::now();

            /* Show result */
            const std::chrono::duration<double, std::milli> duration = end - start;
            std::cout << "\t     " << duration.count() << " ms   \t--> "
                      << std::string(is_sorted_custom(test_list.begin(), test_list.end())
                      ? "OK" : "ERROR") << std::endl;
        }
        range_print(test_list.begin(), test_list.end(), 8);
    }

    /* Exit  */
    std::cout << std::endl << std::endl << "\t...press ^M to exit" << std::flush;
    std::cin.get();

    std::cout << ">~~<   Exit 0    >~~<" << std::endl;
    return 0;

} 