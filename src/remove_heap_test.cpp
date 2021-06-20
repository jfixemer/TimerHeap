#include <iostream>
#include <algorithm>
#include <vector>
#include <assert.h>
#include "remove_heap.hpp"

bool min_heap_compare(const int& lhs, const int& rhs) { return lhs > rhs; }

int main(int argc, const char* argv[])
{
    std::vector<int> orig { 3, 1, 4, 1, 5, 9, 7, 6, 8, 19, 32, 12, 16 };

    // To help show this algorithm works with the STL heap algorithms we
    // setup or heap from random-ish data.
    //
    // this heap ends up hitting most / all 'scenarios'
    // like removing the heapified element 7 (value 6) causes an up-heapify
    // instead of the more common down heapify or leaf.
    std::make_heap(orig.begin(), orig.end());

    int c = orig.size();
    std::cout << "Orig Max Heap  -> ";
    for (auto i : orig)
        std::cout << i << (--c > 0 ? ", " : "");
    std::cout << '\n';

    for (int remove_index = 0; remove_index < orig.size(); ++remove_index)
    {
        std::vector<int> v { orig };
        // Works like pop_heap -- Moves the specified element to the end and leaves
        // a subheap from begin to end - 1.
        std::remove_heap(v.begin(), v.end(), v.begin() + remove_index);
        v.pop_back(); // actually removes the removing element
        std::cout << "Remove index " << remove_index << " -> ";
        c = v.size();
        for (auto i : v)
            std::cout << i << (--c > 0 ? ", " : "");
        std::cout << std::endl;
        assert(std::is_heap(v.begin(), v.end()));
    }

    // reset the original hardcoded heap.
    orig = { 3, 1, 4, 1, 5, 9, 7, 6, 8, 19, 32, 12, 16 };

    // now use custom comparison to create a min-heap instead.
    std::make_heap(orig.begin(), orig.end(), min_heap_compare);
    c = orig.size();
    std::cout << "\n\nOrig Min Heap  -> ";
    for (auto i : orig)
        std::cout << i << (--c > 0 ? ", " : "");
    std::cout << '\n';

    for (int remove_index = 0; remove_index < orig.size(); ++remove_index)
    {
        std::vector<int> v { orig };
        // Works like pop_heap -- Moves the specified element to the end and leaves
        // a subheap from begin to end - 1.
        std::remove_heap(v.begin(), v.end(), v.begin() + remove_index, min_heap_compare);
        v.pop_back(); // actually removes the removing element
        std::cout << "Remove index " << remove_index << " -> ";
        c = v.size();
        for (auto i : v)
            std::cout << i << (--c > 0 ? ", " : "");
        std::cout << std::endl;
        assert(std::is_heap(v.begin(), v.end(), min_heap_compare));
    }
}