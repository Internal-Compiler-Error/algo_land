//
// Created by internal_compiler_error on 2021-02-21.
//
#include <map.h>

#include <catch2/catch.hpp>
#include <iostream>
#include <limits>
#include <random>
#include <set>
#include <unordered_set>

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

TEST_CASE("map::erase removes the node with the matching key", "[erase]") {
    algo::map<int, int> map;

    std::vector<std::pair<int, int>> vec;
    std::unordered_set<int> s;
    std::random_device seeder;
    std::mt19937_64 rand_engine{seeder()};
    std::uniform_int_distribution<int> distribution{std::numeric_limits<int>::min(), std::numeric_limits<int>::max()};

    for (std::size_t i = 0; i != 20000; ++i) {
        auto [key, value] = std::pair<int, int>{distribution(rand_engine), distribution(rand_engine)};
        if (!s.contains(key)) {
            s.insert(key);
            vec.emplace_back(key, value);
            map.insert({vec.back().first, vec.back().second});
        }
    }

    std::shuffle(vec.begin(), vec.end(), rand_engine);

    for (auto& i : vec) {
        map.erase(i.first);
    }
}

TEST_CASE("map iterators is in non-increasing order", "[iterator]") {
    algo::map<int, int> map;

    std::vector<int> vec;

    std::random_device seeder;
    std::mt19937_64 rand_engine{seeder()};
    std::uniform_int_distribution<int> distribution{-200, 400};

    vec.resize(2000);
    std::generate(vec.begin(), vec.end(), [&] { return distribution(rand_engine); });

    std::unordered_set<int> s;

    for (auto item : vec) {
        s.insert(item);
    }
    vec.clear();
    vec.insert(vec.begin(), s.begin(), s.end());

    for (auto i : vec) {
        map.insert({i, 0});
    }

    for (int i = 0; i < 600; ++i) {
        map.insert({vec[i], i + 1});
    }

    std::vector<int> map_order;
    for (auto it = map.begin(); it != map.end(); ++it) {
        map_order.push_back((*it).first);
    }
    std::sort(vec.begin(), vec.end());
    REQUIRE(map_order == vec);
}

TEST_CASE("map begin == end when emtpy", "[iterator]") {
    algo::map<int, int> m;
    REQUIRE(m.begin() == m.end());
}