#include "../include/BPlusTree.h"
#include <iostream>
#include <cassert>
#include <string>
#include <vector>

using namespace bptree;

void testRangeEmptyTree() {
    BPlusTree<int, std::string> tree(4);
    auto results = tree.rangeQuery(1, 10);
    assert(results.empty());

    std::cout << "✓ Empty tree range query test passed" << std::endl;
}

void testRangeSingleElement() {
    BPlusTree<int, std::string> tree(4);
    tree.insert(5, "value5");

    auto results = tree.rangeQuery(1, 10);
    assert(results.size() == 1);
    assert(results[0].first == 5);
    assert(results[0].second == "value5");

    std::cout << "✓ Single element range query test passed" << std::endl;
}

void testRangeFullRange() {
    BPlusTree<int, std::string> tree(4);

    for (int i = 1; i <= 10; i++) {
        tree.insert(i, "value" + std::to_string(i));
    }

    auto results = tree.rangeQuery(1, 10);
    assert(results.size() == 10);

    for (size_t i = 0; i < results.size(); i++) {
        assert(results[i].first == static_cast<int>(i + 1));
        assert(results[i].second == "value" + std::to_string(i + 1));
    }

    std::cout << "✓ Full range query test passed" << std::endl;
}

void testRangePartialRange() {
    BPlusTree<int, std::string> tree(4);

    for (int i = 1; i <= 20; i++) {
        tree.insert(i, "value" + std::to_string(i));
    }

    auto results = tree.rangeQuery(5, 15);
    assert(results.size() == 11);  // 5 to 15 inclusive

    for (size_t i = 0; i < results.size(); i++) {
        assert(results[i].first == static_cast<int>(i + 5));
    }

    std::cout << "✓ Partial range query test passed" << std::endl;
}

void testRangeNoMatch() {
    BPlusTree<int, std::string> tree(4);

    for (int i = 1; i <= 10; i++) {
        tree.insert(i, "value" + std::to_string(i));
    }

    auto results = tree.rangeQuery(15, 20);
    assert(results.empty());

    std::cout << "✓ No match range query test passed" << std::endl;
}

void testRangeExactMatch() {
    BPlusTree<int, std::string> tree(4);

    for (int i = 1; i <= 10; i++) {
        tree.insert(i, "value" + std::to_string(i));
    }

    auto results = tree.rangeQuery(5, 5);
    assert(results.size() == 1);
    assert(results[0].first == 5);

    std::cout << "✓ Exact match range query test passed" << std::endl;
}

void testRangeWithGaps() {
    BPlusTree<int, std::string> tree(4);

    // Insert with gaps: 2, 4, 6, 8, 10
    for (int i = 1; i <= 5; i++) {
        tree.insert(i * 2, "value" + std::to_string(i * 2));
    }

    auto results = tree.rangeQuery(1, 10);
    assert(results.size() == 5);

    for (size_t i = 0; i < results.size(); i++) {
        assert(results[i].first == static_cast<int>((i + 1) * 2));
    }

    std::cout << "✓ Range query with gaps test passed" << std::endl;
}

void testRangeLargeDataset() {
    BPlusTree<int, int> tree(5);

    // Insert 1000 elements
    for (int i = 0; i < 1000; i++) {
        tree.insert(i, i * 2);
    }

    // Query range 100-200
    auto results = tree.rangeQuery(100, 200);
    assert(results.size() == 101);  // 100 to 200 inclusive

    for (size_t i = 0; i < results.size(); i++) {
        assert(results[i].first == static_cast<int>(i + 100));
        assert(results[i].second == static_cast<int>((i + 100) * 2));
    }

    std::cout << "✓ Large dataset range query test passed" << std::endl;
}

void testRangeBoundaryConditions() {
    BPlusTree<int, std::string> tree(4);

    for (int i = 1; i <= 10; i++) {
        tree.insert(i, "value" + std::to_string(i));
    }

    // Start at beginning
    auto results1 = tree.rangeQuery(1, 5);
    assert(results1.size() == 5);

    // End at end
    auto results2 = tree.rangeQuery(6, 10);
    assert(results2.size() == 5);

    // Start before first element
    auto results3 = tree.rangeQuery(0, 3);
    assert(results3.size() == 3);

    // End after last element
    auto results4 = tree.rangeQuery(8, 15);
    assert(results4.size() == 3);

    std::cout << "✓ Boundary conditions range query test passed" << std::endl;
}

int main() {
    std::cout << "Running range query tests..." << std::endl;

    testRangeEmptyTree();
    testRangeSingleElement();
    testRangeFullRange();
    testRangePartialRange();
    testRangeNoMatch();
    testRangeExactMatch();
    testRangeWithGaps();
    testRangeLargeDataset();
    testRangeBoundaryConditions();

    std::cout << "\n✓ All range query tests passed!" << std::endl;
    return 0;
}
