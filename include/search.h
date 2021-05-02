#ifndef ALGO_LAND_SEARCH_H
#define ALGO_LAND_SEARCH_H
#include <concepts>
#include <iterator>
#include <numeric>

namespace algo {

template <typename Iter, typename T>
Iter binary_search(Iter begin, Iter end, T const& value) requires(std::totally_ordered_with<typename std::iterator_traits<Iter>::value_type, T>,
                                                                  std::random_access_iterator<Iter>) {
    if (begin == end) {
        return end;
    } else {
        auto mid_point = begin + (end - begin) / 2;

        auto comp = *mid_point <=> value;
        if (comp == std::strong_ordering::equal) {
            return mid_point;
        } else if (comp == std::strong_ordering::less) {
            return binary_search(mid_point, end, value);
        } else {
            return binary_search(begin, mid_point, value);
        }
    }
}
}  // namespace algo
#endif  // ALGO_LAND_SEARCH_H
