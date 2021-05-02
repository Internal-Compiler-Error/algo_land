#ifndef ALGO_LAND_MAP_H
#define ALGO_LAND_MAP_H

#include <concepts>
#include <cstddef>
#include <memory>
#include <stdexcept>
#include <utility>

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
    };

   public:
    using node_type = typename node_t<K, V>::node_type;
    using key_type = typename node_type::pair_type::first_type;
    using value_type = typename node_type::pair_type::second_type;

    map() noexcept = default;

    [[nodiscard]] constexpr V const& at(K const& key) const {
        auto* value = find(root, key);
        if (!value) {
            throw std::out_of_range{"such key does not exist!"};
        } else {
            return value->key_val_;
        }
    }

    [[nodiscard]] constexpr V& at(K const& key) {
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
            auto* iter = &root;
            while (*iter) {
                auto comp = key_val.first <=> (*iter)->key_val_.first;
                if (comp == std::strong_ordering::equal) {
                    (*iter)->key_val_ = std::move(key_val);
                } else if (comp == std::strong_ordering::less) {
                    iter = &(*iter)->left_;
                } else {
                    iter = &(*iter)->right_;
                }
            }
            *iter = std::make_unique<node_type>(node_type{.key_val_ = std::forward<std::pair<K, V>&&>(key_val), .left_ = nullptr, .right_ = nullptr});
        }
    }

    void remove(K const& key) {}

    [[nodiscard]] constexpr std::size_t size() const { return size(root); }

    constexpr key_type const& min() const noexcept { return static_cast<key_type const&>(*min_impl()); }
    constexpr key_type& min() noexcept { return *min_impl(); }

    constexpr key_type& floor(key_type const& key) const noexcept {}

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
            if (node_iter->key == key) {
                return node_iter;
            }

            if (key < node_iter->key) {
                node_iter = node_iter->left_.get();
            }
            if (key > node_iter->key) {
                node_iter = node_iter->right_.get();
            }
        }
        return node_iter;
    }

    constexpr key_type* min_impl() noexcept {
        auto* iter = root.get();
        if (iter) {
            while (iter) {
                iter = iter->left_.get();
            }
            return iter->get().key_val_.first;
        }
        return nullptr;
    }

    constexpr key_type* floor_impl(node_type const* node, key_type const& key) noexcept {
        if (!node) {
            return nullptr;
        }

        auto comp = key <=> node->key();

        if (comp == std::strong_ordering::equal) {
            return node->key();
        } else if (comp == std::strong_ordering::less) {
            return floor_impl(node->left_.get(), key);
        } else {
            auto* smallest_in_right_tree = floor(node->right_.get(), key);
            if (smallest_in_right_tree) {
                return smallest_in_right_tree;
            } else {
                return node->key();
            }
        }
    }

    std::unique_ptr<node_type> root = nullptr;
};

#endif  // ALGO_LAND_MAP_H
