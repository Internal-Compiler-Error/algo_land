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

        std::unique_ptr<node_type> left_;
        std::unique_ptr<node_type> right_;
        node_type* parent_;
        pair_type key_val_;

        [[nodiscard]] friend constexpr auto operator<=>(node_t<T, U> const& lhs, node_t<T, U> const& rhs) noexcept(noexcept(lhs.key <=> rhs.key)) {
            return lhs.key <=> rhs.key;
        }
        [[nodiscard]] constexpr auto& key() const noexcept { return key_val_.first; }
        [[nodiscard]] constexpr auto& value() noexcept { return key_val_.second; }

        [[nodiscard]] constexpr node_type* left() const noexcept { return left_.get(); }
        [[nodiscard]] constexpr node_type* right() const noexcept { return right_.get(); }
        [[nodiscard]] constexpr node_type*& parent() noexcept { return parent_; }
        [[nodiscard]] constexpr auto& key_val() noexcept { return key_val_; }
        [[nodiscard]] constexpr auto const& key_val() const noexcept { return key_val_; }
    };

public:
    using node_type = typename node_t<K, V>::node_type;
    using key_type = typename node_type::pair_type::first_type;
    using value_type = typename node_type::pair_type::second_type;
    using ssize_type = long long;  // this is a deliberate decision to to signed integers for size, sizes are never negative and hence should be unsigned is a
                                   // bad argument

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
        auto* target = find(root.get(), key);
        if (!target) {
            throw std::out_of_range{"such key does not exist!"};
        }
        return target->value();
    }

    void insert(std::pair<K, V>&& key_val) {
        // we walk through every unique pointer until it points to null, meaning we have reached a leaf node
        // the `iter` is is a pointer to a std::unique_ptr, hence double de-referencing is required to get the underlying element
        auto* iter = &root;
        decltype(iter->get()) parent = nullptr;
        while (*iter) {
            auto comp = key_val.first <=> (*iter)->key();
            if (comp == std::strong_ordering::equal) {
                (*iter)->key_val() = std::move(key_val);
                return;
            } else if (comp == std::strong_ordering::less) {
                parent = iter->get();
                iter = &(*iter)->left_;
            } else {
                parent = iter->get();
                iter = &(*iter)->right_;
            }
        }
        ++ssize_;
        *iter = std::make_unique<node_type>(
            node_type{.left_ = nullptr, .right_ = nullptr, .parent_ = parent, .key_val_ = std::forward<std::pair<K, V>&&>(key_val)});
    }
    
    void remove(key_type const& key) {
        auto* node = find(root.get(), key);

        if (!node) {
            throw std::out_of_range{"Key doesn't exist, deleting non-existent keys are nonsense!"};
        }

        if (!node->left() && !node->right()) {
            auto* parent = node->parent();
            if (parent) {
                // normal case when parent is just another node
                if (parent->left() == node) {
                    parent->left_.reset();
                } else {
                    parent->right_.reset();
                }
            } else {
                // special case when node is root
                root.reset();
            }
            --ssize_;
            return;
        } else if (!node->left()) {
            // when the node only has a right sub tree
            auto* parent = node->parent();
            auto right_sub_tree = std::move(node->right_);
            if (parent) {
                right_sub_tree->parent() = parent;
                if (parent->left() == node) {
                    parent->left_ = std::move(right_sub_tree);
                } else {
                    parent->right_ = std::move(right_sub_tree);
                }
            } else {
                // special case when node is root

                right_sub_tree->parent() = root.get();
                root = std::move(right_sub_tree);
            }
            --ssize_;
            return;
        } else if (!node->right()) {
            // when the node only has a left sub tree
            auto* parent = node->parent();
            auto left_sub_tree = std::move(node->left_);

            if (parent) {
                left_sub_tree->parent() = parent;
                if (parent->left() == node) {
                    parent->left_ = std::move(left_sub_tree);
                } else {
                    parent->right_ = std::move(left_sub_tree);
                }
            } else {
                // special case when node is root
                left_sub_tree->parent() = root.get();
                root = std::move(left_sub_tree);
            }
            --ssize_;
            return;
        }

        // or we are in the most difficult case when the node has two subtrees

        // the successor node should be moved into the place where `node` currently is
        auto successor = pop_min(node->right());
        auto* successor_parent = successor->parent();

        // take over the two subtrees of `node`
        auto left = std::move(node->left_);
        auto right = std::move(node->right_);

        auto* node_parent = node->parent();

        if (node_parent) {
            if (node_parent->left() == node) {
                node_parent->left_ = std::move(successor);
            } else {
                node_parent->right_ = std::move(successor);
            }

            successor->parent() = node_parent;
            successor->left_ = std::move(left);
            if (successor->left_) {
                successor->left()->parent() = successor.get();
            }

            successor->right_ = std::move(right);
            if (successor->right_) {
                successor->right()->parent() = successor.get();
            }
        } else {
            successor->left_ = std::move(left);

            if (successor->left_) {
                successor->left()->parent() = successor.get();
            }

            successor->right_ = std::move(right);

            if (successor->right_) {
                successor->right()->parent() = successor.get();
            }

            successor->parent() = nullptr;
            root = std::move(successor);
        }
        --ssize_;
        return;
    }

    [[nodiscard]] constexpr ssize_type size() const { return ssize_; }

    constexpr node_type const& min() const noexcept { return static_cast<key_type const&>(*min_impl(root.get())); }
    constexpr node_type& min() noexcept { return *min_impl(root.get()); }

    constexpr node_type const& lower_bound(key_type const& key) const { return static_cast<node_type const&>(*floor_impl(root.get(), key)); }
    constexpr node_type& lower_bound(key_type const& key) { return *floor_impl(root.get(), key); }

    constexpr node_type const& upper_bound(key_type const& key) const { return static_cast<node_type const&>(*ceiling_impl(root.get(), key)); }
    constexpr node_type& upper_bound(key_type const& key) { return *ceiling_impl(root.get(), key); }

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
            } else if (key < node_iter->key()) {
                node_iter = node_iter->left();
            } else if (key > node_iter->key()) {
                node_iter = node_iter->right();
            }
        }
        return node_iter;
    }

    constexpr node_type* min_impl(node_type* node) const noexcept {
        auto* iter = node;
        if (iter) {
            while (iter->left_) {
                iter = iter->left();
            }
            return iter;
        }
        return nullptr;
    }

    constexpr node_type* floor_impl(node_type* node, key_type const& key) const noexcept {
        if (!node) {
            return nullptr;
        }

        auto const comp = key <=> node->key();

        if (comp == std::strong_ordering::equal) {
            return node;
        } else if (comp == std::strong_ordering::less) {
            return floor_impl(node->left(), key);
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

    constexpr node_type* ceiling_impl(node_type* node, key_type const& key) const {
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

    constexpr std::unique_ptr<node_type> pop_min(node_type* node) noexcept {
        auto* iter = node;
        while (iter->left_) {
            iter = iter->left();
        }

        auto* parent = iter->parent();

        bool const from_left = iter == parent->left();

        auto min_node = [&] {
            if (from_left) {
                return std::move(parent->left_);
            } else {
                return std::move(parent->right_);
            }
        }();
        auto right_sub_tree = std::move(min_node->right_);
        if (right_sub_tree) {
            right_sub_tree->parent() = parent;
        }

        if (from_left) {
            parent->left_ = std::move(right_sub_tree);
        } else {
            parent->right_ = std::move(right_sub_tree);
        }

        return std::move(min_node);
    }

    std::unique_ptr<node_type> root = nullptr;
    ssize_type ssize_ = 0;
};
}  // namespace algo
#endif  // ALGO_LAND_MAP_H
