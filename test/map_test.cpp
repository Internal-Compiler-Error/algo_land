//
// Created by internal_compiler_error on 2021-02-21.
//
#include <map.h>

#include <catch2/catch.hpp>
#include <limits>
#include <random>

TEST_CASE("map can construct as <int, int> pair") { algo::map<int, int> m; }

TEST_CASE("map can insert") {
    algo::map<int, int> m;

    std::random_device seeder;
    std::mt19937_64 rand_engine{seeder()};
    std::uniform_int_distribution<int> distribution{std::numeric_limits<int>::min(), std::numeric_limits<int>::max()};

    for (std::size_t i = 0; i != 20000; ++i) {
        m.insert({distribution(rand_engine), distribution(rand_engine)});
    }
}