#include "../include/BPlusTree.h"
#include <iostream>
#include <cassert>
#include <string>

using namespace bptree;

void testDeleteSingle() {
    BPlusTree<int, std::string> tree(4);
    tree.insert(10, "value10");

    assert(tree.remove(10));
    assert(tree.isEmpty());

    std::string value;
    assert(!tree.search(10, value));

    std::cout << "✓ Single delete test passed" << std::endl;
}

void testDeleteNonExistent() {
    BPlusTree<int, std::string> tree(4);
    tree.insert(10, "value10");

    assert(!tree.remove(20));
    assert(tree.validate());

    std::cout << "✓ Delete non-existent test passed" << std::endl;
}

void testDeleteFromLeaf() {
    BPlusTree<int, std::string> tree(4);

    for (int i = 1; i <= 10; i++) {
        tree.insert(i, "value" + std::to_string(i));
    }

    // Delete some elements
    assert(tree.remove(5));
    assert(tree.remove(7));

    std::string value;
    assert(!tree.search(5, value));
    assert(!tree.search(7, value));

    // Others should still exist
    for (int i : {1, 2, 3, 4, 6, 8, 9, 10}) {
        assert(tree.search(i, value));
    }

    assert(tree.validate());
    std::cout << "✓ Delete from leaf test passed" << std::endl;
}

void testDeleteAll() {
    BPlusTree<int, std::string> tree(4);

    for (int i = 1; i <= 10; i++) {
        tree.insert(i, "value" + std::to_string(i));
    }

    // Delete all elements
    for (int i = 1; i <= 10; i++) {
        assert(tree.remove(i));
    }

    assert(tree.isEmpty());
    std::cout << "✓ Delete all test passed" << std::endl;
}

void testDeleteWithMerge() {
    BPlusTree<int, int> tree(4);

    // Insert elements to create structure
    for (int i = 1; i <= 20; i++) {
        tree.insert(i, i * 10);
    }

    assert(tree.validate());

    // Delete elements to trigger merges
    for (int i = 1; i <= 15; i++) {
        assert(tree.remove(i));
        assert(tree.validate());
    }

    // Remaining elements should be accessible
    int value;
    for (int i = 16; i <= 20; i++) {
        assert(tree.search(i, value));
        assert(value == i * 10);
    }

    std::cout << "✓ Delete with merge test passed" << std::endl;
}

void testDeleteRandom() {
    BPlusTree<int, std::string> tree(4);

    // Insert elements
    for (int i = 1; i <= 30; i++) {
        tree.insert(i, "value" + std::to_string(i));
    }

    // Delete in random order
    int deleteOrder[] = {15, 3, 27, 8, 22, 1, 19, 30, 6, 11};
    for (int key : deleteOrder) {
        assert(tree.remove(key));
        assert(tree.validate());
    }

    // Verify deleted elements don't exist
    std::string value;
    for (int key : deleteOrder) {
        assert(!tree.search(key, value));
    }

    std::cout << "✓ Random delete test passed" << std::endl;
}

void testDeleteAndReinsert() {
    BPlusTree<int, std::string> tree(4);

    for (int i = 1; i <= 10; i++) {
        tree.insert(i, "value" + std::to_string(i));
    }

    // Delete some
    tree.remove(5);
    tree.remove(7);

    // Reinsert
    tree.insert(5, "new_value5");
    tree.insert(7, "new_value7");

    std::string value;
    assert(tree.search(5, value));
    assert(value == "new_value5");
    assert(tree.search(7, value));
    assert(value == "new_value7");

    assert(tree.validate());
    std::cout << "✓ Delete and reinsert test passed" << std::endl;
}

int main() {
    std::cout << "Running delete tests..." << std::endl;

    testDeleteSingle();
    testDeleteNonExistent();
    testDeleteFromLeaf();
    testDeleteAll();
    testDeleteWithMerge();
    testDeleteRandom();
    testDeleteAndReinsert();

    std::cout << "\n✓ All delete tests passed!" << std::endl;
    return 0;
}
