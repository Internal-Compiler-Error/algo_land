#ifndef ALGO_LAND_BALANCED_MAP_H
#define ALGO_LAND_BALANCED_MAP_H

#include <memory>

namespace algo {
namespace rb_details {

enum class color : bool { red, black };

struct node_base {
    node_base* parent_ = nullptr;
    color color_ = color::red;

    node_base() noexcept = default;
    node_base(node_base* parent, color color) noexcept : parent_{parent}, color_{color} {}
};

static_assert(std::is_standard_layout_v<node_base>);

/**
 * Internal node type definition
 * @tparam T Key type
 * @tparam U Value type
 */
template <typename T, typename U>
struct node_t final : node_base {
    using node_type = node_t<T, U>;
    using pair_type = std::pair<T, U>;
    using edge_type = std::unique_ptr<node_type>;

    edge_type left_;
    edge_type right_;
    pair_type key_val_;

    node_t(std::unique_ptr<node_type> left, std::unique_ptr<node_type> right, pair_type&& key_val, node_base* parent, color color)
        : left_{std::move(left)}, right_{std::move(right)}, key_val_{std::forward<pair_type>(key_val)}, node_base::node_base{parent, color} {}

    [[nodiscard]] friend constexpr auto operator<=>(node_t<T, U> const& lhs, node_t<T, U> const& rhs) noexcept(noexcept(lhs.key <=> rhs.key)) {
        return lhs.key <=> rhs.key;
    }
    [[nodiscard]] constexpr auto& key() const noexcept { return key_val_.first; }
    [[nodiscard]] constexpr auto& value() noexcept { return key_val_.second; }

    [[nodiscard]] constexpr node_type* left() const noexcept { return left_.get(); }
    [[nodiscard]] constexpr node_type* right() const noexcept { return right_.get(); }
    [[nodiscard]] constexpr auto& key_val() noexcept { return key_val_; }
    [[nodiscard]] constexpr auto const& key_val() const noexcept { return key_val_; }
};

}  // namespace rb_details

using namespace rb_details;

template <typename T, typename V>

struct rb_header final : node_base {
    std::unique_ptr<node_t<T, V>> next_node_ = nullptr;
    node_t<T, V>* min_node_ = nullptr;
    node_t<T, V>* max_node_ = nullptr;
};

template <typename K, typename V>
requires std::totally_ordered<K>
class rb_map {
public:
    using node_type = node_t<K, V>;
    using key_type = K const;
    using value_type = V;
    using pair_type = typename node_type::pair_type;

    void insert(pair_type&& pair) noexcept;

    [[nodiscard]] constexpr value_type const& at(K const& key) const {
        auto* value = find(header_.next_node_.get(), key);
        if (!value) {
            throw std::out_of_range{"such key does not exist!"};
        } else {
            return value->key_val_;
        }
    }

    [[nodiscard]] constexpr value_type& at(K const& key) {
        auto* target = find(header_.next_node_.get(), key);
        if (!target) {
            throw std::out_of_range{"such key does not exist!"};
        }
        return target->value();
    }

private:
    using edge_type = typename node_type::edge_type;
    constexpr node_type* find(node_type* node, key_type& key) const noexcept;
    [[nodiscard]] constexpr edge_type insert(edge_type& current, pair_type&& pair) noexcept;
    constexpr edge_type left_rotate(edge_type node) noexcept;
    constexpr edge_type right_rotate(edge_type node) noexcept;
    constexpr void flip_color(node_type* node) noexcept;
    constexpr bool is_red(node_type* node) const noexcept;

    rb_header<K, V> header_;
};

template <typename K, typename V>
requires std::totally_ordered<K>
constexpr typename rb_map<K, V>::edge_type rb_map<K, V>::left_rotate(edge_type node) noexcept {
    auto target = std::move(node->right_);
    node->right_ = std::move(target->left_);
    target->left_ = std::move(node);
    node->color_ = target->color_;
    target->color_ = color::red;

    node->parent_ = target.get();
    if (node->left_) {
        node->left()->parent_ = node.get();
    }
    if (node->right_) {
        node->right()->parent_ = node.get();
    }

    node->parent_ = target.get();
    if (target->right_) {
        target->right()->parent_ = target.get();
    }

    return std::move(target);
}
template <typename K, typename V>
requires std::totally_ordered<K>
constexpr typename rb_map<K, V>::edge_type rb_map<K, V>::right_rotate(edge_type node) noexcept {
    auto target = std::move(node->left_);
    node->left_ = std::move(target->right_);
    target->right_ = std::move(node);
    target->color_ = node->color_;
    node->color_ = color::red;

    if (node->left_) {
        node->left()->parent_ = node.get();
    }
    if (node->right_) {
        node->right()->parent_ = node.get();
    }

    node->parent_ = target.get();
    if (node->left()) {
        node->left()->parent_ = node.get();
    }

    return std::move(target);
}

template <typename K, typename V>
requires std::totally_ordered<K>
void rb_map<K, V>::insert(pair_type&& pair) noexcept {
    header_.next_node_ = insert(header_.next_node_, std::forward<pair_type>(pair));
    header_.next_node_->color_ = color::black;
}
template <typename K, typename V>
requires std::totally_ordered<K>
constexpr void rb_map<K, V>::flip_color(rb_map::node_type* node) noexcept {
    node->color_ = color::red;

    node->left_->color_ = color::black;
    node->right_->color_ = color::black;
}
template <typename K, typename V>
requires std::totally_ordered<K>
constexpr bool rb_map<K, V>::is_red(rb_map::node_type* node) const noexcept {
    if (node == nullptr) {
        return false;
    } else {
        return node->color_ == color::red;
    }
}
template <typename K, typename V>
requires std::totally_ordered<K>
[[nodiscard]] constexpr typename rb_map<K, V>::edge_type rb_map<K, V>::insert(edge_type& current, rb_map::pair_type&& pair) noexcept {
    if (current == nullptr) {
        return std::make_unique<node_type>(nullptr, nullptr, std::forward<rb_map::pair_type>(pair), nullptr, color::black);
    }

    auto const comp = pair.first <=> current->key();

    if (comp == std::strong_ordering::less) {
        current->left_ = insert(current->left_, std::forward<rb_map::pair_type>(pair));
    } else if (comp == std::strong_ordering::greater) {
        current->right_ = insert(current->right_, std::forward<rb_map::pair_type>(pair));
    } else {
        current->value() == std::move(pair.second);
    }

    if (is_red(current->right()) && !is_red(current->left())) {
        current = left_rotate(std::move(current));
    }

    if (is_red(current->left()) && is_red(current->left()->left())) {
        current = right_rotate(std::move(current));
    }

    if (is_red(current->left()) && is_red(current->right())) {
        flip_color(current.get());
    }

    return std::move(current);
}
template <typename K, typename V>
requires std::totally_ordered<K>
constexpr typename rb_map<K, V>::node_type* rb_map<K, V>::find(rb_map::node_type* node, rb_map::key_type& key) const noexcept {
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

}  // namespace algo

#ifndef NDEBUG
template class algo::rb_details::node_t<int, int>;
// template class algo::rb_map<int, int>;
#endif

#endif  // ALGO_LAND_BALANCED_MAP_H
