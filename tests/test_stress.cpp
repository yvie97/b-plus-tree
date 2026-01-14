#include "../include/BPlusTree.h"
#include <iostream>
#include <cassert>
#include <string>
#include <random>
#include <algorithm>
#include <set>
#include <vector>

using namespace bptree;

/**
 * Stress tests for B+ tree with random mixed operations
 * Tests the robustness of the implementation under heavy load
 */

// Utility function to generate random integers
std::vector<int> generateRandomIntegers(int count, int min, int max, unsigned int seed) {
    std::vector<int> result;
    std::mt19937 gen(seed);
    std::uniform_int_distribution<> dist(min, max);

    for (int i = 0; i < count; i++) {
        result.push_back(dist(gen));
    }

    return result;
}

void testRandomInsertAndSearch() {
    BPlusTree<int, int> tree(4);
    std::set<int> insertedKeys;

    auto randomKeys = generateRandomIntegers(1000, 1, 10000, 42);

    // Random inserts
    for (int key : randomKeys) {
        tree.insert(key, key * 10);
        insertedKeys.insert(key);
    }

    assert(tree.validate());

    // Verify all inserted keys
    int value;
    for (int key : insertedKeys) {
        assert(tree.search(key, value));
        assert(value == key * 10);
    }

    std::cout << "✓ Random insert and search test passed (1000 ops)" << std::endl;
}

void testMixedInsertDelete() {
    BPlusTree<int, std::string> tree(4);
    std::set<int> currentKeys;

    std::mt19937 gen(123);
    std::uniform_int_distribution<> keyDist(1, 500);
    std::uniform_int_distribution<> opDist(0, 1);  // 0 = insert, 1 = delete

    // Perform 2000 random operations
    for (int i = 0; i < 2000; i++) {
        int op = opDist(gen);
        int key = keyDist(gen);

        if (op == 0 || currentKeys.empty()) {
            // Insert
            tree.insert(key, "value" + std::to_string(key));
            currentKeys.insert(key);
        } else {
            // Delete a random existing key
            if (!currentKeys.empty()) {
                auto it = currentKeys.begin();
                std::advance(it, gen() % currentKeys.size());
                int keyToDelete = *it;

                tree.remove(keyToDelete);
                currentKeys.erase(keyToDelete);
            }
        }

        // Validate every 100 operations
        if (i % 100 == 0) {
            assert(tree.validate());
        }
    }

    // Final validation
    assert(tree.validate());

    // Verify all current keys exist
    std::string value;
    for (int key : currentKeys) {
        assert(tree.search(key, value));
    }

    std::cout << "✓ Mixed insert/delete test passed (2000 ops)" << std::endl;
}

void testHeavyInsertDelete() {
    BPlusTree<int, int> tree(5);

    // Insert 5000 elements
    for (int i = 1; i <= 5000; i++) {
        tree.insert(i, i * 100);
    }

    assert(tree.validate());

    // Delete 4000 elements in random order
    auto deleteOrder = generateRandomIntegers(4000, 1, 5000, 999);

    for (int key : deleteOrder) {
        tree.remove(key);
    }

    assert(tree.validate());

    std::cout << "✓ Heavy insert/delete test passed (5000 inserts, 4000 deletes)" << std::endl;
}

void testRandomOperationsWithValidation() {
    BPlusTree<int, int> tree(4);
    std::set<int> expectedKeys;

    std::mt19937 gen(456);
    std::uniform_int_distribution<> keyDist(1, 1000);
    std::uniform_int_distribution<> opDist(0, 2);  // 0 = insert, 1 = delete, 2 = search

    // Perform 3000 operations with validation after each
    for (int i = 0; i < 3000; i++) {
        int op = opDist(gen);
        int key = keyDist(gen);

        if (op == 0) {
            // Insert
            tree.insert(key, key * 5);
            expectedKeys.insert(key);
        } else if (op == 1) {
            // Delete
            bool removed = tree.remove(key);
            if (removed) {
                expectedKeys.erase(key);
            }
        } else {
            // Search
            int value;
            bool found = tree.search(key, value);
            bool shouldExist = expectedKeys.count(key) > 0;
            assert(found == shouldExist);
            if (found) {
                assert(value == key * 5);
            }
        }

        // Validate periodically
        if (i % 500 == 0) {
            assert(tree.validate());
        }
    }

    // Final validation
    assert(tree.validate());

    // Verify final state
    int value;
    for (int key : expectedKeys) {
        assert(tree.search(key, value));
        assert(value == key * 5);
    }

    std::cout << "✓ Random operations with validation test passed (3000 ops)" << std::endl;
}

void testSequentialInsertRandomDelete() {
    BPlusTree<int, std::string> tree(4);

    // Sequential insert
    for (int i = 1; i <= 1000; i++) {
        tree.insert(i, "value" + std::to_string(i));
    }

    assert(tree.validate());

    // Random delete
    auto deleteOrder = generateRandomIntegers(800, 1, 1000, 777);
    std::set<int> deletedKeys(deleteOrder.begin(), deleteOrder.end());

    for (int key : deleteOrder) {
        tree.remove(key);
    }

    assert(tree.validate());

    // Verify remaining keys
    std::string value;
    for (int i = 1; i <= 1000; i++) {
        bool shouldExist = deletedKeys.count(i) == 0;
        bool exists = tree.search(i, value);
        assert(exists == shouldExist);
    }

    std::cout << "✓ Sequential insert, random delete test passed (1000 inserts, 800 deletes)" << std::endl;
}

void testRandomInsertSequentialDelete() {
    BPlusTree<int, int> tree(5);

    // Random insert
    auto randomKeys = generateRandomIntegers(1000, 1, 2000, 888);
    std::set<int> insertedKeys(randomKeys.begin(), randomKeys.end());

    for (int key : insertedKeys) {
        tree.insert(key, key * 7);
    }

    assert(tree.validate());

    // Sequential delete (from the inserted keys)
    std::vector<int> sortedKeys(insertedKeys.begin(), insertedKeys.end());
    int deleteCount = sortedKeys.size() / 2;

    for (int i = 0; i < deleteCount; i++) {
        tree.remove(sortedKeys[i]);
    }

    assert(tree.validate());

    // Verify remaining keys
    int value;
    for (size_t i = deleteCount; i < sortedKeys.size(); i++) {
        assert(tree.search(sortedKeys[i], value));
        assert(value == sortedKeys[i] * 7);
    }

    std::cout << "✓ Random insert, sequential delete test passed" << std::endl;
}

void testDuplicateInsertStress() {
    BPlusTree<int, std::string> tree(4);

    // Insert same keys multiple times with different values
    for (int round = 0; round < 10; round++) {
        for (int i = 1; i <= 100; i++) {
            tree.insert(i, "round" + std::to_string(round) + "_" + std::to_string(i));
        }
        assert(tree.validate());
    }

    // Verify last round values
    std::string value;
    for (int i = 1; i <= 100; i++) {
        assert(tree.search(i, value));
        assert(value == "round9_" + std::to_string(i));
    }

    std::cout << "✓ Duplicate insert stress test passed (1000 ops)" << std::endl;
}

void testRangeQueryAfterMixedOps() {
    BPlusTree<int, int> tree(5);
    std::set<int> currentKeys;

    // Mixed operations
    auto insertKeys = generateRandomIntegers(500, 1, 1000, 111);
    for (int key : insertKeys) {
        tree.insert(key, key * 3);
        currentKeys.insert(key);
    }

    auto deleteKeys = generateRandomIntegers(200, 1, 1000, 222);
    for (int key : deleteKeys) {
        if (tree.remove(key)) {
            currentKeys.erase(key);
        }
    }

    assert(tree.validate());

    // Range query
    auto results = tree.rangeQuery(200, 800);

    // Verify range query results
    for (const auto& pair : results) {
        assert(pair.first >= 200 && pair.first <= 800);
        assert(currentKeys.count(pair.first) > 0);
        assert(pair.second == pair.first * 3);
    }

    std::cout << "✓ Range query after mixed ops test passed" << std::endl;
}

void testAlternatingPatterns() {
    BPlusTree<int, int> tree(4);

    // Insert even numbers
    for (int i = 0; i < 1000; i += 2) {
        tree.insert(i, i * 10);
    }

    assert(tree.validate());

    // Insert odd numbers
    for (int i = 1; i < 1000; i += 2) {
        tree.insert(i, i * 10);
    }

    assert(tree.validate());

    // Delete even numbers
    for (int i = 0; i < 1000; i += 2) {
        tree.remove(i);
    }

    assert(tree.validate());

    // Verify only odd numbers remain
    int value;
    for (int i = 0; i < 1000; i++) {
        if (i % 2 == 1) {
            assert(tree.search(i, value));
            assert(value == i * 10);
        } else {
            assert(!tree.search(i, value));
        }
    }

    std::cout << "✓ Alternating patterns test passed (2000 inserts, 500 deletes)" << std::endl;
}

void testHighOrderStress() {
    // Test with larger order
    BPlusTree<int, int> tree(20);  // Higher order

    // Insert many elements
    for (int i = 1; i <= 3000; i++) {
        tree.insert(i, i * 2);
    }

    assert(tree.validate());

    // Random deletes
    auto deleteKeys = generateRandomIntegers(2000, 1, 3000, 555);
    std::set<int> deletedKeys(deleteKeys.begin(), deleteKeys.end());

    for (int key : deleteKeys) {
        tree.remove(key);
    }

    assert(tree.validate());

    // Verify remaining keys
    int value;
    for (int i = 1; i <= 3000; i++) {
        bool shouldExist = deletedKeys.count(i) == 0;
        bool exists = tree.search(i, value);
        assert(exists == shouldExist);
    }

    std::cout << "✓ High order stress test passed (order=20, 3000 inserts)" << std::endl;
}

void testRepeatedInsertDeleteCycles() {
    BPlusTree<int, std::string> tree(4);

    for (int cycle = 0; cycle < 5; cycle++) {
        // Insert phase
        for (int i = 1; i <= 200; i++) {
            tree.insert(i, "cycle" + std::to_string(cycle) + "_" + std::to_string(i));
        }

        assert(tree.validate());

        // Delete phase
        for (int i = 1; i <= 200; i++) {
            tree.remove(i);
        }

        assert(tree.isEmpty());
        assert(tree.validate());
    }

    std::cout << "✓ Repeated insert/delete cycles test passed (5 cycles, 200 ops each)" << std::endl;
}

int main() {
    std::cout << "Running stress tests..." << std::endl;
    std::cout << "========================" << std::endl;

    testRandomInsertAndSearch();
    testMixedInsertDelete();
    testHeavyInsertDelete();
    testRandomOperationsWithValidation();
    testSequentialInsertRandomDelete();
    testRandomInsertSequentialDelete();
    testDuplicateInsertStress();
    testRangeQueryAfterMixedOps();
    testAlternatingPatterns();
    testHighOrderStress();
    testRepeatedInsertDeleteCycles();

    std::cout << "\n✓ All stress tests passed!" << std::endl;
    std::cout << "  Total operations: 20000+" << std::endl;
    return 0;
}
