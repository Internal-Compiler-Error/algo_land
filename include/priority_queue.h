//
// Created by internal_compiler_error on 2021-02-20.
//

#ifndef ALGO_LAND_PRIORITY_QUEUE_H
#define ALGO_LAND_PRIORITY_QUEUE_H

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <utility>
#include <vector>

namespace algo {

template <typename T, typename Compare = std::less<>>
class priority_queue {
   public:
    using value_type = T;

    template <typename U>
    requires std::convertible_to<U, T>
    void insert(U&& u) noexcept {
        using std::is_heap, std::begin, std::end, std::forward;

        arr_.push_back(forward<U>(u));
        swim(arr_.size() - 1);
        assert(validate());
    }

    [[nodiscard]] bool empty() const noexcept { return arr_.empty(); }

    T pop() {
        using std::swap, std::move;

        if (!empty()) {
            auto top_element = move(arr_.front());

            // put the last element to the top and fix invariance
            swap(arr_.front(), arr_.back());
            arr_.pop_back();

            if (!arr_.empty()) {
                sink(0);
            }
            assert(validate());
            return top_element;
        } else {
            assert(validate());
            throw std::out_of_range{"The heap is empty!"};
        }
    }

    /**
     * Access the top element. The reference is const so that the caller can't destroy the weak ordering invariance.
     * @return top element
     */
    T const& top() { return arr_.front(); }

    void print() const noexcept {
        for (auto item : arr_) {
            std::cout << item << ' ';
        }
        std::cout << '\n';
    }

    [[nodiscard]] bool validate() const { return std::is_heap(std::begin(arr_), std::begin(arr_), comp_); }

   private:
    [[nodiscard]] constexpr std::size_t left_child(std::size_t pos) const noexcept { return 2 * pos + 1; }
    [[nodiscard]] constexpr std::size_t right_child(std::size_t pos) const noexcept { return 2 * pos + 2; }
    [[nodiscard]] constexpr std::size_t parent(std::size_t pos) const noexcept { return (pos - 1) / 2; }

    void swim(std::size_t pos) noexcept {
        while (pos != 0 && comp_(arr_[parent(pos)], arr_[pos])) {
            using std::swap;
            swap(arr_[parent(pos)], arr_[pos]);
            pos = parent(pos);
        }
        assert(validate());
    }

    void sink(std::size_t pos) noexcept {
        // stop when left_ child is the last element
        while (left_child(pos) < arr_.size()) {
            std::size_t const next_child = [pos, this]() {
                auto const left_child_pos = left_child(pos);
                auto const right_child_pos = right_child(pos);

                // there are two conditions that prevent us from selecting the right child as the next one to consider
                // 1. there are no right child, i.e. the current child is already the last one in the arr
                // 2. the right child is not the correct choice (for instance the right child is smaller in a max heap)
                return (right_child_pos < arr_.size() && comp_(arr_[left_child_pos], arr_[right_child_pos])) ? right_child_pos : left_child_pos;
            }();

            // this means we have fix the invariance
            if (!comp_(arr_[pos], arr_[next_child])) {
                break;
            } else {
                using std::swap;
                swap(arr_[pos], arr_[next_child]);
                pos = next_child;
            }
        }

        assert(validate());
    }

    std::vector<T> arr_;
    Compare comp_;
};
}  // namespace algo
#endif  // ALGO_LAND_PRIORITY_QUEUE_H
