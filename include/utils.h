#ifndef ALGO_LAND_UTILS_H
#define ALGO_LAND_UTILS_H

namespace algo {
template <typename T>
void fill_random(std::vector<T>& vec) noexcept {
    std::random_device rnd_device;

    std::mt19937_64 mersenne_engine{rnd_device()};
    std::uniform_int_distribution<T> distribution{-300, 600};

    auto gen = [&distribution, &mersenne_engine]() { return distribution(mersenne_engine); };

    generate(vec.begin(), vec.end(), gen);
}

template <typename T>
std::ostream& operator<<(std::ostream& os, std::vector<T> vec) noexcept {
    os << "{ ";
    std::copy(vec.begin(), vec.end(), std::ostream_iterator<T>(os, " "));
    os << "}";
    return os;
}

template <typename T>
bool is_even(T t) {
    return t % 2 == 0;
}
}  // namespace algo
#endif  // ALGO_LAND_UTILS_H
