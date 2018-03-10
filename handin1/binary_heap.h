#include <functional>
#include <vector>
#include <iostream>

template<typename T>
struct null_tracker
{
    void operator()(T& value, size_t index) { }
};

template<typename T, typename Compare = std::less<T>, typename Track = null_tracker<T>>
class binary_heap
{
    std::vector<T> m_elements;

private:
    size_t parent(size_t i) { return (i - 1) / 2; }
    size_t left(size_t i) { return 2 * i + 1; }
    size_t right(size_t i) { return 2 * i + 2; }

    void min_heapify(size_t index)
    {
        size_t l = left(index);
        size_t r = right(index);
        Compare comp;
        size_t smallest;
        if (l < m_elements.size() && comp(m_elements[l], m_elements[index]))
            smallest = l;
        else
            smallest = index;

        if (r < m_elements.size() && comp(m_elements[r], m_elements[smallest]))
            smallest = r;

        if (smallest != index)
        {
            std::swap(m_elements[index], m_elements[smallest]);

            Track track;
            track(m_elements[smallest], smallest);
            track(m_elements[index], index);

            min_heapify(smallest);
        }
    }

public:
    size_t size() { return m_elements.size(); }

    void dump_list()
    {
        for (T val : m_elements)
            std::cout << val << " ";
        std::cout << "\n";
    }

    const T& find_min() const { return m_elements[0]; }

    void delete_min()
    {
        m_elements[0] = m_elements[m_elements.size() - 1];
        m_elements.pop_back();
        if (m_elements.size() > 0)
            min_heapify(0);
    }

    void decrease_key(size_t index, const T& newValue)
    {
        Compare comp;
        Track track;
        while (index > 0 && comp(newValue, m_elements[parent(index)]))
        {
            m_elements[index] = m_elements[parent(index)];
            track(m_elements[index], index);

            index = parent(index);
        }

        m_elements[index] = newValue;
        track(m_elements[index], index);
    }

    void insert(const T& element)
    {
        m_elements.push_back(element);
        decrease_key(m_elements.size() - 1, element);
    }

    static binary_heap<T, Compare, Track> make_heap(const std::vector<T>& elems)
    {
        binary_heap<T, Compare, Track> heap;
        heap.m_elements = elems;
        for (int i = static_cast<int>(heap.m_elements.size() / 2 - 1); i >= 0; i--)
        {
            heap.min_heapify(static_cast<size_t>(i));
        }

        return heap;
    }
};
