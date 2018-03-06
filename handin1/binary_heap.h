#include <functional>
#include <vector>

template<typename T, typename Compare = std::less<T>>
class binary_heap
{
    std::vector<T> m_elements;

private:
    void min_heapify(size_t index)
    {

    }

public:
    const T& find_min() const { return m_elements[0]; }

    void insert(const T& element)
    {
    }
};
