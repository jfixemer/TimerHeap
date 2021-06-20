#include <algorithm>

namespace std
{
// This is another function in the STL Algorightm of heap related functionality.
// This adds a way to remove and arbitrary element in a heapified structure with random access
// iterators. Note: Finding the iterator of the node is likely inefficient due to the heap
// structure, but this function preseves the O(log n) similar to the existing push_heap and pop_heap
// operations offered in <algorithms>
template <class RandomIt, class Compare>
void remove_heap(RandomIt begin, RandomIt end, RandomIt it, Compare comp)
{
    // One or Zero elements - we are done.
    if ((distance(begin, end) < 2) || (distance(begin, it) >= distance(begin, end)))
        return;

    auto last = distance(begin, end);
    iter_swap(it, end - 1); // put the node to remove at the end.
    --last; // don't consider that node when re-heapifying.

    // So a push heap with our new end node will 'heapify up'.
    // (because we would be a proper subheap up to this point with a 'new node' at the end.)
    push_heap<RandomIt, Compare>(begin, it + 1, comp);

    // We don't know if we did anything to heapify up, so we will go ahead and try heapify down.
    // (heapify down would mean that comparing childern isn't really needed.)
    while (true)
    {
        // Mathmatically flat tree is root(k) = 1, left = k*2 and right = k*2 + 1.
        // Vector (and all C arrays) are
        //  - root index/distance = 0
        //  - dist = index + 1  (= k)
        //  - left = dist*2 - 1 = right - 1
        //  - right = dist*2 + 1 - 1 = dist*2
        auto dist = std::distance(begin, it) + 1;
        auto right = 2 * dist;
        auto left = right - 1;

        // We are at a leaf, we are done.
        if (left >= last)
            break;
        auto test = right;

        // We are at a 'half leaf' (only one child) Have to compare with the only child.
        if (right >= last)
            test = left;

        // If the left is greater than the right we should swap with that node instead.
        else if (comp(*(begin + right), *(begin + left)))
            test = left; // only need to swap with better of the 2 children.

        // Convert distance to iterator.
        auto test_it = begin + test;

        // if we are better than the best child then we are done.
        if (comp(*test_it, *it))
            break;

        // Finally - looks like we need to swap down the tree and loop again.
        iter_swap(test_it, it);
        it = test_it;
    }
}

// Provides default 'less than' operator for the Iterator's value_type.
template <class RandomIt> void remove_heap(RandomIt begin, RandomIt end, RandomIt it)
{
    remove_heap(begin, end, it, std::less<typename RandomIt::value_type>());
}

}
