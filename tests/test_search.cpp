#include "../include/BPlusTree.h"
#include <iostream>
#include <cassert>
#include <string>

using namespace bptree;

void testEmptyTreeSearch() {
    BPlusTree<int, std::string> tree(4);
    std::string value;
    assert(!tree.search(10, value));
    std::cout << "✓ Empty tree search test passed" << std::endl;
}

void testSingleElementSearch() {
    BPlusTree<int, std::string> tree(4);
    tree.insert(10, "value10");

    std::string value;
    assert(tree.search(10, value));
    assert(value == "value10");
    assert(!tree.search(20, value));

    std::cout << "✓ Single element search test passed" << std::endl;
}

void testMultipleElementSearch() {
    BPlusTree<int, std::string> tree(4);

    // Insert multiple elements
    for (int i = 1; i <= 10; i++) {
        tree.insert(i * 10, "value" + std::to_string(i * 10));
    }

    // Search for existing elements
    std::string value;
    for (int i = 1; i <= 10; i++) {
        assert(tree.search(i * 10, value));
        assert(value == "value" + std::to_string(i * 10));
    }

    // Search for non-existing elements
    assert(!tree.search(5, value));
    assert(!tree.search(15, value));
    assert(!tree.search(105, value));

    std::cout << "✓ Multiple element search test passed" << std::endl;
}

void testSearchAfterUpdate() {
    BPlusTree<int, std::string> tree(4);
    tree.insert(10, "original");

    std::string value;
    assert(tree.search(10, value));
    assert(value == "original");

    // Update value
    tree.insert(10, "updated");
    assert(tree.search(10, value));
    assert(value == "updated");

    std::cout << "✓ Search after update test passed" << std::endl;
}

void testSearchWithSplits() {
    BPlusTree<int, std::string> tree(4);  // Order 4, max 3 keys per node

    // Insert enough to cause splits
    for (int i = 1; i <= 20; i++) {
        tree.insert(i, "value" + std::to_string(i));
    }

    // Verify all can be found
    std::string value;
    for (int i = 1; i <= 20; i++) {
        assert(tree.search(i, value));
        assert(value == "value" + std::to_string(i));
    }

    std::cout << "✓ Search with splits test passed" << std::endl;
}

int main() {
    std::cout << "Running search tests..." << std::endl;

    testEmptyTreeSearch();
    testSingleElementSearch();
    testMultipleElementSearch();
    testSearchAfterUpdate();
    testSearchWithSplits();

    std::cout << "\n✓ All search tests passed!" << std::endl;
    return 0;
}
