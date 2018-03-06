#include <chrono>
#include <iostream>
#include <unistd.h>
#include "binary_heap.h"

double get_millis()
{
    auto now = std::chrono::high_resolution_clock::now();
    return std::chrono::time_point_cast<std::chrono::milliseconds>(now)
           .time_since_epoch()
           .count();
}

int main()
{
    double start = get_millis();
    binary_heap<int> heap;
    std::cout << "Time: " << get_millis() - start << std::endl;
    return 0;
}
