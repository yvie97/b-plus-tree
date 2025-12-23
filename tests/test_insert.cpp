#include "../include/BPlusTree.h"
#include <iostream>
#include <cassert>
#include <string>

using namespace bptree;

void testInsertSingle() {
    BPlusTree<int, std::string> tree(4);
    tree.insert(10, "value10");

    std::string value;
    assert(tree.search(10, value));
    assert(value == "value10");
    assert(tree.validate());

    std::cout << "✓ Single insert test passed" << std::endl;
}

void testInsertSequential() {
    BPlusTree<int, std::string> tree(4);

    for (int i = 1; i <= 10; i++) {
        tree.insert(i, "value" + std::to_string(i));
    }

    // Verify all inserted
    std::string value;
    for (int i = 1; i <= 10; i++) {
        assert(tree.search(i, value));
        assert(value == "value" + std::to_string(i));
    }

    assert(tree.validate());
    std::cout << "✓ Sequential insert test passed" << std::endl;
}

void testInsertReverse() {
    BPlusTree<int, std::string> tree(4);

    for (int i = 10; i >= 1; i--) {
        tree.insert(i, "value" + std::to_string(i));
    }

    // Verify all inserted
    std::string value;
    for (int i = 1; i <= 10; i++) {
        assert(tree.search(i, value));
        assert(value == "value" + std::to_string(i));
    }

    assert(tree.validate());
    std::cout << "✓ Reverse insert test passed" << std::endl;
}

void testInsertRandom() {
    BPlusTree<int, std::string> tree(4);

    int keys[] = {15, 3, 27, 8, 42, 1, 19, 33, 6, 11};
    for (int key : keys) {
        tree.insert(key, "value" + std::to_string(key));
    }

    // Verify all inserted
    std::string value;
    for (int key : keys) {
        assert(tree.search(key, value));
        assert(value == "value" + std::to_string(key));
    }

    assert(tree.validate());
    std::cout << "✓ Random insert test passed" << std::endl;
}

void testInsertDuplicate() {
    BPlusTree<int, std::string> tree(4);

    tree.insert(10, "original");
    tree.insert(10, "updated");

    std::string value;
    assert(tree.search(10, value));
    assert(value == "updated");
    assert(tree.validate());

    std::cout << "✓ Duplicate insert test passed" << std::endl;
}

void testInsertWithSplits() {
    BPlusTree<int, std::string> tree(4);  // Order 4, max 3 keys per node

    // Insert enough to cause multiple splits
    for (int i = 1; i <= 30; i++) {
        tree.insert(i, "value" + std::to_string(i));
        assert(tree.validate());
    }

    // Verify structure
    std::string value;
    for (int i = 1; i <= 30; i++) {
        assert(tree.search(i, value));
    }

    std::cout << "✓ Insert with splits test passed" << std::endl;
}

void testInsertLargeDataset() {
    BPlusTree<int, int> tree(5);

    // Insert 1000 elements
    for (int i = 0; i < 1000; i++) {
        tree.insert(i, i * 2);
    }

    // Verify
    int value;
    for (int i = 0; i < 1000; i++) {
        assert(tree.search(i, value));
        assert(value == i * 2);
    }

    assert(tree.validate());
    std::cout << "✓ Large dataset insert test passed" << std::endl;
}

int main() {
    std::cout << "Running insert tests..." << std::endl;

    testInsertSingle();
    testInsertSequential();
    testInsertReverse();
    testInsertRandom();
    testInsertDuplicate();
    testInsertWithSplits();
    testInsertLargeDataset();

    std::cout << "\n✓ All insert tests passed!" << std::endl;
    return 0;
}
