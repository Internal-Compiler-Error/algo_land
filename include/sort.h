//
// Created by internal_compiler_error on 2021-02-20.
//

#ifndef ALGO_LAND_SORT_H
#define ALGO_LAND_SORT_H
#include <priority_queue.h>

#include <cassert>

namespace algo {
template <typename T>
void selection_sort(std::vector<T>& vec) noexcept {
    for (auto iterator = vec.begin(); iterator != vec.end(); ++iterator) {
        auto smallest_yet_iter = iterator;
        for (auto begin = iterator; begin != vec.end(); ++begin) {
            if (*begin < *smallest_yet_iter) {
                smallest_yet_iter = begin;
            }
        }
        std::iter_swap(iterator, smallest_yet_iter);
    }
    assert(std::is_sorted(vec.begin(), vec.end()));
}

template <typename T>
void insertion_sort(std::vector<T>& vec) noexcept {
    for (auto front_iter = std::next(vec.begin()); front_iter != vec.end(); ++front_iter) {
        // goes from the end of sorted bit and
        for (auto middle_iter = front_iter; middle_iter != vec.begin() && *middle_iter < *std::prev(middle_iter); --middle_iter) {
            std::swap(middle_iter, std::prev(middle_iter));
        }
    }
    assert(std::is_sorted(vec.begin(), vec.end()));
}

template <typename T>
void bubble_sort(std::vector<T>& vec) {
    for (auto it = vec.begin(); it != vec.end(); ++it) {
        for (auto iterator = vec.begin(); iterator != std::prev(vec.end()); ++iterator) {
            if (*iterator > *std::next(iterator)) {
                std::iter_swap(iterator, std::next(iterator));
            }
            for (int i : vec) {
                std::cout << i << ' ';
            }
        }
        std::cout << '\n';
    }
    assert(std::is_sorted(vec.begin(), vec.end()));
}

/**
 * Performs a quicksort partition by picking the element in the middle and moving all those that are smaller to the left_ of the element and all those that are
 * bigger to the right of the pivot. The pivot will be the in the correct final position of the sorted range
 * @tparam BiDirectionalIterator must satisfy the requirement of LegacyBidirectionalIterator
 * @param begin [begin, end) of a range
 * @param end [begin, end) of a range
 * @return An BiDirectionalIterator to the pivot
 */
template <typename BiDirectionalIterator>
BiDirectionalIterator partition(BiDirectionalIterator begin, BiDirectionalIterator end) noexcept {
    using std::distance, std::advance, std::prev, std::next;

    // pivot_it is the middle element
    // move the pivot to the end
    std::iter_swap(next(begin, distance(begin, end) / 2), std::prev(end));
    auto const pivot_it = prev(end);

    auto front_iter = begin;
    auto back_iter = prev(end, 2);

    bool manual = false;

    while (distance(begin, front_iter) <= distance(begin, back_iter)) {
        while (*front_iter < *pivot_it) {
            ++front_iter;
        }

        while (*back_iter > *pivot_it) {
            --back_iter;
        }

        if (distance(begin, front_iter) <= distance(begin, back_iter)) {
            std::iter_swap(front_iter, back_iter);
            ++front_iter;
            --back_iter;
        }

        if (distance(begin, front_iter) > distance(begin, back_iter)) {
            manual = true;
            break;
        }
    }

    if (!manual) {
        std::cout << "manual = " << std::boolalpha << manual << '\n';
    }
    std::iter_swap(front_iter, pivot_it);
    return front_iter;
}

template <typename Iterator>
void quick_sort(Iterator begin, Iterator end) noexcept {
    if (begin == end) {
        return;
    }
    auto index = partition(begin, end);
    quick_sort(begin, index);
    quick_sort(std::next(index), end);
}

template <typename T>
void quick_sort(std::vector<T>& vec) noexcept {
    quick_sort(vec.begin(), vec.end());
    assert(std::is_sorted(vec.begin(), vec.end()));
}

template <typename Iter>
void heap_sort(Iter begin, Iter end) {
    ::priority_queue<std::remove_reference_t<decltype(*std::declval<Iter>())>, std::greater<>> heap;

    for (Iter it = begin; it != end; ++it) {
        heap.insert(std::move(*it));
    }
    assert(heap.validate());

    for (Iter it = begin; it != end; ++it) {
        auto ret = heap.pop();
        heap.print();
        std::cout << "Just popped out: " << ret << "\n\n";
        *it = ret;
    }

    for (Iter it = begin; it != end; ++it) {
        std::cout << *it << '\n';
    }
    assert(std::is_sorted(begin, end));
}
}  // namespace algo
#endif  // ALGO_LAND_SORT_H
