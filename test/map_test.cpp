//
// Created by internal_compiler_error on 2021-02-21.
//
#include <map.h>

#include <catch2/catch.hpp>
#include <iostream>
#include <limits>
#include <random>

TEST_CASE("map can construct as <int, int> pair", "[construct]") { algo::map<int, int> m; }

TEST_CASE("map can construct as <std::string, int> pair", "[construct]") { algo::map<std::string, int> map; }

TEST_CASE("map cannot construct as <std::vector<int>, int> pair", "[construct]") { algo::map<std::vector<int>, int> map; }

TEST_CASE("map::at returns the node", "[access]") {
    algo::map<int, int> map;

    map.insert({1, 2});
    map.insert({3, 4});
    map.insert({-1, 55});
    map.insert({99, 42});

    REQUIRE(map.at(1) == 2);
    REQUIRE(map.at(3) == 4);
    REQUIRE(map.at(-1) == 55);
    REQUIRE(map.at(99) == 42);
}

TEST_CASE("map::at throws on non-existent key", "[access]") {
    algo::map<int, int> map;

    map.insert({1, 2});
    map.insert({3, 4});
    map.insert({-1, 55});
    map.insert({99, 42});

    REQUIRE_THROWS(map.at(0));
    REQUIRE_THROWS(map.at(4));
    REQUIRE_THROWS(map.at(100));
    REQUIRE_THROWS(map.at(-42));
}

TEST_CASE("map can insert", "[insert]") {
    algo::map<int, int> m;

    std::random_device seeder;
    std::mt19937_64 rand_engine{seeder()};
    std::uniform_int_distribution<int> distribution{std::numeric_limits<int>::min(), std::numeric_limits<int>::max()};

    for (std::size_t i = 0; i != 2000; ++i) {
        m.insert({distribution(rand_engine), distribution(rand_engine)});
    }
}

TEST_CASE("map::min returns the minimum", "[min]") {
    algo::map<int, int> map;

    map.insert({1, 2});
    map.insert({3, 4});
    map.insert({-1, 55});
    map.insert({99, 42});

    REQUIRE(map.min().key() == -1);
}

TEST_CASE("map::lower_bound returns the floor of key", "[lower_bound]") {
    algo::map<int, int> map;

    map.insert({1, 2});
    map.insert({3, 4});
    map.insert({-1, 55});
    map.insert({99, 42});

    REQUIRE(map.lower_bound(4).key() == 3);
    REQUIRE(map.lower_bound(1).key() == 1);
    REQUIRE(map.lower_bound(98).key() == 3);
    REQUIRE(map.lower_bound(-1).key() == -1);
}

TEST_CASE("map::upper_bound returns the ceiling of key", "[upper_bound]") {
    algo::map<int, int> map;

    map.insert({1, 2});
    map.insert({3, 4});
    map.insert({-1, 55});
    map.insert({99, 42});

    REQUIRE(map.upper_bound(4).key() == 99);
    REQUIRE(map.upper_bound(1).key() == 1);
    REQUIRE(map.upper_bound(98).key() == 99);
    REQUIRE(map.upper_bound(0).key() == 1);
}

TEST_CASE("map::delete_min deletes the minimum element", "[delete_min]") {
    algo::map<int, int> map;

    map.insert({1, 2});
    map.insert({3, 4});
    map.insert({-1, 55});
    map.insert({99, 42});
}