//
// Created by internal_compiler_error on 2021-02-20.
//

#ifndef ALGO_LAND_PRIORITY_QUEUE_H
#define ALGO_LAND_PRIORITY_QUEUE_H

#include <iostream>
#include <stdexcept>
#include <utility>
#include <vector>

template <typename T>
class priority_queue {
   public:
    template <typename U>
    requires std::convertible_to<U, T> void insert(U&& u) noexcept {
        using std::is_heap, std::begin, std::end, std::forward;

        arr_.push_back(forward<U>(u));
        swim(arr_.size() - 1);
        assert(is_heap(begin(arr_), end(arr_)));
    }

    [[nodiscard]] bool empty() const noexcept { return arr_.empty(); }

    T pop_max() {
        using std::swap, std::move;

        if (!empty()) {
            auto top_element = move(arr_.front());

            // put the last element to the top and fix invariant
            swap(arr_.front(), arr_.back());
            arr_.pop_back();
            if (!arr_.empty()) {
                sink(0);
            }

            assert(is_heap(begin(arr_), end(arr_)));
            return top_element;
        } else {
            throw std::out_of_range{"The heap is empty!"};
        }
    }

    void print() const noexcept {
        for (auto item : arr_) {
            std::cout << item << ' ';
        }
        std::cout << '\n';
    }

   private:
    [[nodiscard]] constexpr std::size_t left_child(std::size_t pos) const noexcept { return (pos - 1) / 2; }
    [[nodiscard]] constexpr std::size_t right_child(std::size_t pos) const noexcept { return (pos - 2) / 2; }

    void swim(std::size_t pos) noexcept {
        while (pos != 0 && arr_[(pos - 1) / 2] < arr_[pos]) {
            using std::swap;
            swap(arr_[(pos - 1) / 2], arr_[pos]);
            pos = (pos - 1) / 2;
        }
    }

    void sink(std::size_t index) noexcept {
        // stop when left child is the last element
        auto iter = index;
        while (2 * iter + 1 < arr_.size()) {
            std::size_t const bigger_child = [iter, this]() {
                auto left_children = 2 * iter + 1;
                return (left_children != arr_.size() && arr_[left_children] < arr_[left_children + 1]) ? left_children + 1 : left_children;
            }();

            if (arr_[iter] > arr_[bigger_child]) {
                break;
            }

            using std::swap;
            swap(arr_[iter], arr_[bigger_child]);
            iter = bigger_child;
        }
    }

    std::vector<T> arr_;
};

#endif  // ALGO_LAND_PRIORITY_QUEUE_H
