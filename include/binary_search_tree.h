#ifndef ALGO_LAND_BINARY_SEARCH_TREE_H
#define ALGO_LAND_BINARY_SEARCH_TREE_H

#include <cstddef>
#include <memory>
#include <stdexcept>

template <typename K, typename V>
class binary_search_tree {
   private:
    /**
     * Internal node type definition
     * @tparam T Key type
     * @tparam U Value type
     */
    template <typename T, typename U>
    struct node_t {
        using node_type = node_t<T, U>;

        T key;
        U value;
        std::unique_ptr<node_type> left;
        std::unique_ptr<node_type> right;
        std::size_t num_subtrees;

        friend constexpr auto operator<=>(node_t<T, U> const& lhs, node_t<T, U> const& rhs) noexcept(noexcept(lhs.key <=> rhs.key)) {
            return lhs.key <=> rhs.key;
        }
    };

   public:
    using node_type = typename node_t<K, V>::node_type;

    binary_search_tree() noexcept : root{std::make_unique<node_type>()} {}

    [[nodiscard]] constexpr V& at(K const& key) const {
        auto* value = search_through(root, key);
        if (!value) {
            throw std::out_of_range{"such key does not exist!"};
        }
        return value->value;
    }

    void add(K key, V value) {
        if (!root) {
            root = new node_t<K, V>{.key = key, .value = value, .left = nullptr, .right = nullptr, .sum_subtrees = 0};
            return;
        }

        auto** iter = &root;
        while (*iter) {
            if (*(iter)->key == key) {
                *iter->value = std::move(value);
                return;
            } else if (*iter->key > key) {
                iter = &iter->left;
            } else {
                iter = &iter->right;
            }
        }
        *iter = new node_t<K, V>{.key = key, .value = value, .left = nullptr, .right = nullptr, .sum_subtrees = 0};
    }

    void remove(K const& key) {}

    [[nodiscard]] constexpr std::size_t size() const { return size(root); }

   private:
    template <typename T, typename U>
    [[nodiscard]] static constexpr std::size_t size(node_t<T, U>* root) noexcept {
        if (root) {
            return root->num_subtrees;
        }
        return 0;
    }

    static node_t<K, V>* search_through(node_t<K, V>* root, K const& key) {
        auto* node_iter = root;
        while (node_iter) {
            if (node_iter->key == key) {
                return node_iter;
            }

            if (key < node_iter->key) {
                node_iter = node_iter->left;
            }
            if (key > node_iter->key) {
                node_iter = node_iter->right;
            }
        }
        return node_iter;
    }

    std::unique_ptr<node_type> root;
};

#endif  // ALGO_LAND_BINARY_SEARCH_TREE_H
