This is a simple extension of functionality offered by the
C++ STL for heap algorithms. 

This functionality is not written at all similar to how the 
GCC libstd algorithms are written but it follows the behavior
of the peer C++ STL heap algorithms and runs in logarithmic time.

Unlike STL's map, or set, a heap offers no good way to 'find' any
particular node by value so the algorithm only deals with iterators.