#include "../include/BPlusTree.h"
#include <iostream>
#include <cassert>
#include <string>

using namespace bptree;

/**
 * Tests for minimum order B+ tree (order = 3)
 * This is the smallest valid B+ tree configuration
 * - maxKeys = 2
 * - minKeys = 1
 */

void testMinOrderConstruction() {
    BPlusTree<int, std::string> tree(3);
    assert(tree.isEmpty());
    assert(tree.validate());
    std::cout << "✓ Min order construction test passed" << std::endl;
}

void testMinOrderBasicInsert() {
    BPlusTree<int, std::string> tree(3);

    tree.insert(10, "value10");
    tree.insert(20, "value20");
    tree.insert(30, "value30");

    std::string value;
    assert(tree.search(10, value) && value == "value10");
    assert(tree.search(20, value) && value == "value20");
    assert(tree.search(30, value) && value == "value30");
    assert(tree.validate());

    std::cout << "✓ Min order basic insert test passed" << std::endl;
}

void testMinOrderSplitLeaf() {
    BPlusTree<int, std::string> tree(3);  // maxKeys = 2

    // Insert 3 elements to trigger leaf split
    tree.insert(10, "value10");
    tree.insert(20, "value20");
    tree.insert(30, "value30");  // This will trigger split

    assert(tree.validate());
    assert(tree.height() == 2);  // Should have root + leaf level

    // Verify all elements
    std::string value;
    assert(tree.search(10, value) && value == "value10");
    assert(tree.search(20, value) && value == "value20");
    assert(tree.search(30, value) && value == "value30");

    std::cout << "✓ Min order leaf split test passed" << std::endl;
}

void testMinOrderSplitInternal() {
    BPlusTree<int, std::string> tree(3);  // maxKeys = 2

    // Insert enough elements to trigger internal node split
    for (int i = 1; i <= 10; i++) {
        tree.insert(i, "value" + std::to_string(i));
        assert(tree.validate());
    }

    // Verify structure
    std::string value;
    for (int i = 1; i <= 10; i++) {
        assert(tree.search(i, value));
        assert(value == "value" + std::to_string(i));
    }

    std::cout << "✓ Min order internal split test passed" << std::endl;
}

void testMinOrderDeleteWithUnderflow() {
    BPlusTree<int, std::string> tree(3);  // minKeys = 1

    // Build tree
    for (int i = 1; i <= 10; i++) {
        tree.insert(i, "value" + std::to_string(i));
    }

    assert(tree.validate());

    // Delete elements to trigger underflow and merging
    for (int i = 1; i <= 8; i++) {
        assert(tree.remove(i));
        assert(tree.validate());
    }

    // Verify remaining elements
    std::string value;
    assert(tree.search(9, value) && value == "value9");
    assert(tree.search(10, value) && value == "value10");

    std::cout << "✓ Min order delete with underflow test passed" << std::endl;
}

void testMinOrderBorrowFromSibling() {
    BPlusTree<int, std::string> tree(3);

    // Create structure that will trigger borrowing
    for (int i = 1; i <= 10; i++) {
        tree.insert(i * 10, "value" + std::to_string(i * 10));
    }

    assert(tree.validate());

    // Delete to trigger borrowing from sibling
    tree.remove(20);
    assert(tree.validate());

    tree.remove(30);
    assert(tree.validate());

    // Verify structure integrity
    std::string value;
    assert(tree.search(10, value));
    assert(!tree.search(20, value));
    assert(!tree.search(30, value));
    assert(tree.search(40, value));

    std::cout << "✓ Min order borrow from sibling test passed" << std::endl;
}

void testMinOrderSequentialInsertDelete() {
    BPlusTree<int, int> tree(3);

    // Sequential insert
    for (int i = 1; i <= 20; i++) {
        tree.insert(i, i * 100);
        assert(tree.validate());
    }

    // Sequential delete
    for (int i = 1; i <= 15; i++) {
        assert(tree.remove(i));
        assert(tree.validate());
    }

    // Verify remaining elements
    int value;
    for (int i = 16; i <= 20; i++) {
        assert(tree.search(i, value));
        assert(value == i * 100);
    }

    std::cout << "✓ Min order sequential insert/delete test passed" << std::endl;
}

void testMinOrderReverseInsert() {
    BPlusTree<int, std::string> tree(3);

    // Reverse insert
    for (int i = 20; i >= 1; i--) {
        tree.insert(i, "value" + std::to_string(i));
        assert(tree.validate());
    }

    // Verify all elements
    std::string value;
    for (int i = 1; i <= 20; i++) {
        assert(tree.search(i, value));
        assert(value == "value" + std::to_string(i));
    }

    std::cout << "✓ Min order reverse insert test passed" << std::endl;
}

void testMinOrderRandomInsert() {
    BPlusTree<int, std::string> tree(3);

    int keys[] = {15, 3, 27, 8, 42, 1, 19, 33, 6, 11, 50, 25, 38, 12, 47};

    for (int key : keys) {
        tree.insert(key, "value" + std::to_string(key));
        assert(tree.validate());
    }

    // Verify all elements
    std::string value;
    for (int key : keys) {
        assert(tree.search(key, value));
        assert(value == "value" + std::to_string(key));
    }

    std::cout << "✓ Min order random insert test passed" << std::endl;
}

void testMinOrderDeleteAll() {
    BPlusTree<int, std::string> tree(3);

    // Insert elements
    for (int i = 1; i <= 15; i++) {
        tree.insert(i, "value" + std::to_string(i));
    }

    assert(tree.validate());

    // Delete all elements
    for (int i = 1; i <= 15; i++) {
        assert(tree.remove(i));
        if (i < 15) {  // Don't validate after last deletion (tree is empty)
            assert(tree.validate());
        }
    }

    assert(tree.isEmpty());
    std::cout << "✓ Min order delete all test passed" << std::endl;
}

void testMinOrderRangeQuery() {
    BPlusTree<int, std::string> tree(3);

    // Insert elements
    for (int i = 1; i <= 20; i++) {
        tree.insert(i * 5, "value" + std::to_string(i * 5));
    }

    assert(tree.validate());

    // Range query
    auto results = tree.rangeQuery(20, 50);

    // Should get: 20, 25, 30, 35, 40, 45, 50
    assert(results.size() == 7);
    assert(results[0].first == 20);
    assert(results[6].first == 50);

    std::cout << "✓ Min order range query test passed" << std::endl;
}

void testMinOrderBelowMinimum() {
    // Test that order below MIN_ORDER (3) is clamped to MIN_ORDER
    BPlusTree<int, std::string> tree(2);  // Should be clamped to 3

    tree.insert(10, "value10");
    tree.insert(20, "value20");
    tree.insert(30, "value30");

    assert(tree.validate());

    std::string value;
    assert(tree.search(10, value));
    assert(tree.search(20, value));
    assert(tree.search(30, value));

    std::cout << "✓ Min order below minimum test passed" << std::endl;
}

int main() {
    std::cout << "Running minimum order (order = 3) tests..." << std::endl;
    std::cout << "==========================================" << std::endl;

    testMinOrderConstruction();
    testMinOrderBasicInsert();
    testMinOrderSplitLeaf();
    testMinOrderSplitInternal();
    testMinOrderDeleteWithUnderflow();
    testMinOrderBorrowFromSibling();
    testMinOrderSequentialInsertDelete();
    testMinOrderReverseInsert();
    testMinOrderRandomInsert();
    testMinOrderDeleteAll();
    testMinOrderRangeQuery();
    testMinOrderBelowMinimum();

    std::cout << "\n✓ All minimum order tests passed!" << std::endl;
    return 0;
}
