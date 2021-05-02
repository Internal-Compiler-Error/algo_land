#ifndef ALGO_LAND_MAP_H
#define ALGO_LAND_MAP_H

#include <concepts>
#include <cstddef>
#include <memory>
#include <stdexcept>
#include <utility>
namespace algo {

template <typename K, typename V>
requires std::totally_ordered<K>
class map {
private:
    /**
     * Internal node type definition
     * @tparam T Key type
     * @tparam U Value type
     */
    template <typename T, typename U>
    struct node_t {
        using node_type = node_t<T, U>;
        using pair_type = std::pair<T, U>;

        pair_type key_val_;
        std::unique_ptr<node_type> left_;
        std::unique_ptr<node_type> right_;

        friend constexpr auto operator<=>(node_t<T, U> const& lhs, node_t<T, U> const& rhs) noexcept(noexcept(lhs.key <=> rhs.key)) {
            return lhs.key <=> rhs.key;
        }

        constexpr auto& key() noexcept { return key_val_.first; }

        constexpr auto& value() noexcept { return key_val_.second; }

        constexpr auto& key_val() noexcept { return key_val_; }
    };

public:
    using node_type = typename node_t<K, V>::node_type;
    using key_type = typename node_type::pair_type::first_type;
    using value_type = typename node_type::pair_type::second_type;

    map() noexcept = default;

    [[nodiscard]] constexpr value_type const& at(K const& key) const {
        auto* value = find(root, key);
        if (!value) {
            throw std::out_of_range{"such key does not exist!"};
        } else {
            return value->key_val_;
        }
    }

    [[nodiscard]] constexpr value_type& at(K const& key) {
        auto* target = find(root, key);
        if (!target) {
            throw std::out_of_range{"such key does not exist!"};
        }
        return target->key_val_;
    }

    void insert(std::pair<K, V>&& key_val) {
        // special case for first insert
        if (!root) {
            root = std::make_unique<node_type>(node_type{.key_val_ = std::forward<std::pair<K, V>&&>(key_val), .left_ = nullptr, .right_ = nullptr});
            return;
        } else {
            // we walk through every unique pointer until it points to null, meaning we have reached a leaf node
            // the `iter` is is a pointer to a std::unique_ptr, hence double de-referencing is required to get the underlying element
            auto* iter = &root;
            while (*iter) {
                auto comp = key_val.first <=> (*iter)->key();
                if (comp == std::strong_ordering::equal) {
                    (*iter)->key_val() = std::move(key_val);
                } else if (comp == std::strong_ordering::less) {
                    iter = &(*iter)->left_;
                } else {
                    iter = &(*iter)->right_;
                }
            }
            *iter = std::make_unique<node_type>(node_type{.key_val_ = std::forward<std::pair<K, V>&&>(key_val), .left_ = nullptr, .right_ = nullptr});
        }
    }

    // TODO: implement this!
    void remove(K const& key) {}

    [[nodiscard]] constexpr std::size_t size() const { return size(root); }

    constexpr node_type const& min() const noexcept { return static_cast<key_type const&>(*min_impl()); }
    constexpr node_type& min() noexcept { return *min_impl(); }

    constexpr node_type const& floor(key_type const& key) const { return static_cast<node_type const&>(*floor_impl(root.get(), key)); }
    constexpr node_type& floor(key_type const& key) { return *floor_impl(root.get(), key); }

    constexpr node_type const& ceiling(key_type const& key) const { return static_cast<node_type const&>(*ceiling_impl(root.get(), key)); }
    constexpr node_type& ceiling(key_type const& key) { return *ceiling_impl(root.get(), key); }

private:
    template <typename T, typename U>
    [[nodiscard]] static constexpr std::size_t size(node_t<T, U>* root) noexcept {
        if (root) {
            return root->num_subtrees_;
        }
        return 0;
    }

    constexpr node_type* find(node_type* node, K const& key) const noexcept {
        auto* node_iter = node;
        while (node_iter) {
            if (node_iter->key() == key) {
                return node_iter;
            }

            if (key < node_iter->key()) {
                node_iter = node_iter->left_.get();
            }
            if (key > node_iter->key()) {
                node_iter = node_iter->right_.get();
            }
        }
        return node_iter;
    }

    constexpr node_type* min_impl() noexcept {
        auto* iter = root.get();
        if (iter) {
            while (iter) {
                iter = iter->left_.get();
            }
            return iter->get();
        }
        return nullptr;
    }

    constexpr node_type* floor_impl(node_type const* node, key_type const& key) noexcept {
        if (!node) {
            return nullptr;
        }

        auto const comp = key <=> node->key();

        if (comp == std::strong_ordering::equal) {
            return node;
        } else if (comp == std::strong_ordering::less) {
            return floor_impl(node->left_.get(), key);
        } else {
            // there might be an element in the right subtree that is greater than the key of `node` but smaller than `key`, we do not know if such element
            // exists until we search through all of them
            if (auto* smaller_in_right_subtree = floor_impl(node->right_.get(), key)) {
                // if there is an element that is bigger than `node` but smaller than `key`, return it
                return smaller_in_right_subtree;
            } else {
                // otherwise, that means all elements in the right subtrees are bigger, `node` is the floor of key
                return node;
            }
        }
    }

    consteval node_type* ceiling_impl(node_type const* node, key_type const& key) {
        if (!node) {
            return nullptr;
        }

        auto const comp = key <=> node->key();
        if (comp == std::strong_ordering::equal) {
            return node;
        } else if (comp == std::strong_ordering::greater) {
            return ceiling_impl(node->right_.get(), key);
        } else {
            // if `key` is smaller than the key of node`, then there might be an element that is even smaller than node but bigger than key
            if (auto* larger_element = ceiling_impl(node->left_.get(), key)) {
                // if there is an element that is smaller than `node` but bigger than `key`, return it
                return larger_element;
            } else {
                // otherwise, `node` is the ceiling of `key`
                return node;
            }
        }
    }

    std::unique_ptr<node_type> root = nullptr;
};
}  // namespace algo
#endif  // ALGO_LAND_MAP_H
