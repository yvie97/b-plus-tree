#include "../include/BPlusTree.h"
#include <iostream>
#include <cassert>
#include <string>

using namespace bptree;

/**
 * Edge case tests for B+ tree implementation
 * Includes: deleting from internal nodes, boundary conditions, and corner cases
 */

void testDeleteRootLeaf() {
    BPlusTree<int, std::string> tree(4);

    // Single element tree (root is a leaf)
    tree.insert(10, "value10");
    assert(tree.remove(10));
    assert(tree.isEmpty());
    assert(!tree.remove(10));  // Try to remove from empty tree

    std::cout << "✓ Delete root leaf test passed" << std::endl;
}

void testDeleteCausingRootChange() {
    BPlusTree<int, std::string> tree(4);

    // Build tree with height > 1
    for (int i = 1; i <= 10; i++) {
        tree.insert(i, "value" + std::to_string(i));
    }

    int initialHeight = tree.height();
    assert(initialHeight > 1);

    // Delete elements to cause root to change
    for (int i = 1; i <= 7; i++) {
        tree.remove(i);
    }

    assert(tree.validate());

    // Verify remaining elements
    std::string value;
    for (int i = 8; i <= 10; i++) {
        assert(tree.search(i, value));
    }

    std::cout << "✓ Delete causing root change test passed" << std::endl;
}

void testDeleteInternalNodeKeyUpdate() {
    BPlusTree<int, int> tree(4);

    // Create tree structure
    for (int i = 1; i <= 20; i++) {
        tree.insert(i, i * 10);
    }

    assert(tree.validate());

    // Delete keys that likely appear in internal nodes
    // Keys at split points typically propagate to internal nodes
    assert(tree.remove(5));
    assert(tree.validate());

    assert(tree.remove(10));
    assert(tree.validate());

    assert(tree.remove(15));
    assert(tree.validate());

    // Verify structure integrity
    int value;
    assert(!tree.search(5, value));
    assert(!tree.search(10, value));
    assert(!tree.search(15, value));

    // Verify other keys still exist
    assert(tree.search(1, value) && value == 10);
    assert(tree.search(20, value) && value == 200);

    std::cout << "✓ Delete internal node key update test passed" << std::endl;
}

void testMergeLeafNodes() {
    BPlusTree<int, std::string> tree(4);

    // Create structure that will force leaf merging
    for (int i = 1; i <= 15; i++) {
        tree.insert(i * 10, "value" + std::to_string(i * 10));
    }

    assert(tree.validate());

    // Delete to force leaf merging
    for (int i = 1; i <= 10; i++) {
        assert(tree.remove(i * 10));
        assert(tree.validate());
    }

    // Verify remaining elements
    std::string value;
    for (int i = 11; i <= 15; i++) {
        assert(tree.search(i * 10, value));
    }

    std::cout << "✓ Merge leaf nodes test passed" << std::endl;
}

void testMergeInternalNodes() {
    BPlusTree<int, int> tree(4);

    // Build larger tree to create multiple levels
    for (int i = 1; i <= 50; i++) {
        tree.insert(i, i * 100);
    }

    assert(tree.validate());
    int initialHeight = tree.height();

    // Delete many elements to force internal node merging
    for (int i = 1; i <= 40; i++) {
        assert(tree.remove(i));
        assert(tree.validate());
    }

    // Tree should be smaller now
    int finalHeight = tree.height();
    assert(finalHeight <= initialHeight);

    // Verify remaining elements
    int value;
    for (int i = 41; i <= 50; i++) {
        assert(tree.search(i, value));
        assert(value == i * 100);
    }

    std::cout << "✓ Merge internal nodes test passed" << std::endl;
}

void testBorrowFromLeftSibling() {
    BPlusTree<int, std::string> tree(5);

    // Create specific pattern to trigger left sibling borrowing
    for (int i = 1; i <= 20; i++) {
        tree.insert(i, "value" + std::to_string(i));
    }

    assert(tree.validate());

    // Delete elements to trigger borrowing from left sibling
    tree.remove(19);
    assert(tree.validate());

    tree.remove(18);
    assert(tree.validate());

    std::string value;
    assert(!tree.search(18, value));
    assert(!tree.search(19, value));
    assert(tree.search(17, value));
    assert(tree.search(20, value));

    std::cout << "✓ Borrow from left sibling test passed" << std::endl;
}

void testBorrowFromRightSibling() {
    BPlusTree<int, std::string> tree(5);

    // Create specific pattern to trigger right sibling borrowing
    for (int i = 1; i <= 20; i++) {
        tree.insert(i, "value" + std::to_string(i));
    }

    assert(tree.validate());

    // Delete elements to trigger borrowing from right sibling
    tree.remove(2);
    assert(tree.validate());

    tree.remove(3);
    assert(tree.validate());

    std::string value;
    assert(!tree.search(2, value));
    assert(!tree.search(3, value));
    assert(tree.search(1, value));
    assert(tree.search(4, value));

    std::cout << "✓ Borrow from right sibling test passed" << std::endl;
}

void testDeleteFirstKey() {
    BPlusTree<int, std::string> tree(4);

    for (int i = 1; i <= 10; i++) {
        tree.insert(i, "value" + std::to_string(i));
    }

    // Repeatedly delete the first key
    for (int i = 1; i <= 5; i++) {
        assert(tree.remove(i));
        assert(tree.validate());
    }

    // Verify remaining elements
    std::string value;
    for (int i = 6; i <= 10; i++) {
        assert(tree.search(i, value));
    }

    std::cout << "✓ Delete first key test passed" << std::endl;
}

void testDeleteLastKey() {
    BPlusTree<int, std::string> tree(4);

    for (int i = 1; i <= 10; i++) {
        tree.insert(i, "value" + std::to_string(i));
    }

    // Repeatedly delete the last key
    for (int i = 10; i >= 6; i--) {
        assert(tree.remove(i));
        assert(tree.validate());
    }

    // Verify remaining elements
    std::string value;
    for (int i = 1; i <= 5; i++) {
        assert(tree.search(i, value));
    }

    std::cout << "✓ Delete last key test passed" << std::endl;
}

void testDeleteMiddleKeys() {
    BPlusTree<int, std::string> tree(4);

    for (int i = 1; i <= 20; i++) {
        tree.insert(i, "value" + std::to_string(i));
    }

    // Delete middle keys
    for (int i = 8; i <= 13; i++) {
        assert(tree.remove(i));
        assert(tree.validate());
    }

    // Verify structure
    std::string value;
    for (int i = 1; i <= 7; i++) {
        assert(tree.search(i, value));
    }
    for (int i = 14; i <= 20; i++) {
        assert(tree.search(i, value));
    }
    for (int i = 8; i <= 13; i++) {
        assert(!tree.search(i, value));
    }

    std::cout << "✓ Delete middle keys test passed" << std::endl;
}

void testAlternatingInsertDelete() {
    BPlusTree<int, int> tree(4);

    // Alternating insert and delete
    for (int i = 1; i <= 20; i++) {
        tree.insert(i, i * 10);
        assert(tree.validate());

        if (i > 1) {
            tree.remove(i - 1);
            assert(tree.validate());
        }
    }

    // Only the last key should remain
    int value;
    assert(tree.search(20, value));
    assert(value == 200);

    std::cout << "✓ Alternating insert/delete test passed" << std::endl;
}

void testInsertAfterDeleteAll() {
    BPlusTree<int, std::string> tree(4);

    // Insert, delete all, then insert again
    for (int i = 1; i <= 10; i++) {
        tree.insert(i, "first" + std::to_string(i));
    }

    for (int i = 1; i <= 10; i++) {
        tree.remove(i);
    }

    assert(tree.isEmpty());

    // Insert new data
    for (int i = 11; i <= 20; i++) {
        tree.insert(i, "second" + std::to_string(i));
    }

    assert(tree.validate());

    // Verify new data
    std::string value;
    for (int i = 11; i <= 20; i++) {
        assert(tree.search(i, value));
        assert(value == "second" + std::to_string(i));
    }

    std::cout << "✓ Insert after delete all test passed" << std::endl;
}

void testDuplicateDeleteAttempt() {
    BPlusTree<int, std::string> tree(4);

    tree.insert(10, "value10");
    assert(tree.remove(10));
    assert(!tree.remove(10));  // Second attempt should fail
    assert(tree.validate());

    std::cout << "✓ Duplicate delete attempt test passed" << std::endl;
}

void testEmptyTreeOperations() {
    BPlusTree<int, std::string> tree(4);

    // Operations on empty tree
    std::string value;
    assert(!tree.search(10, value));
    assert(!tree.remove(10));
    assert(tree.isEmpty());
    assert(tree.validate());
    assert(tree.height() == 0);

    auto results = tree.rangeQuery(1, 10);
    assert(results.empty());

    std::cout << "✓ Empty tree operations test passed" << std::endl;
}

void testSingleElementOperations() {
    BPlusTree<int, std::string> tree(4);

    tree.insert(42, "answer");

    std::string value;
    assert(tree.search(42, value));
    assert(value == "answer");
    assert(!tree.isEmpty());
    assert(tree.height() == 1);
    assert(tree.validate());

    // Update
    tree.insert(42, "new_answer");
    assert(tree.search(42, value));
    assert(value == "new_answer");

    // Range query
    auto results = tree.rangeQuery(40, 45);
    assert(results.size() == 1);
    assert(results[0].first == 42);

    std::cout << "✓ Single element operations test passed" << std::endl;
}

void testLargeScaleDeletePattern() {
    BPlusTree<int, int> tree(5);

    // Insert large dataset
    for (int i = 1; i <= 100; i++) {
        tree.insert(i, i * 100);
    }

    assert(tree.validate());

    // Delete every other element
    for (int i = 2; i <= 100; i += 2) {
        assert(tree.remove(i));
    }

    assert(tree.validate());

    // Verify odd numbers remain
    int value;
    for (int i = 1; i <= 100; i++) {
        if (i % 2 == 1) {
            assert(tree.search(i, value));
            assert(value == i * 100);
        } else {
            assert(!tree.search(i, value));
        }
    }

    std::cout << "✓ Large scale delete pattern test passed" << std::endl;
}

int main() {
    std::cout << "Running edge case tests..." << std::endl;
    std::cout << "============================" << std::endl;

    testDeleteRootLeaf();
    testDeleteCausingRootChange();
    testDeleteInternalNodeKeyUpdate();
    testMergeLeafNodes();
    testMergeInternalNodes();
    testBorrowFromLeftSibling();
    testBorrowFromRightSibling();
    testDeleteFirstKey();
    testDeleteLastKey();
    testDeleteMiddleKeys();
    testAlternatingInsertDelete();
    testInsertAfterDeleteAll();
    testDuplicateDeleteAttempt();
    testEmptyTreeOperations();
    testSingleElementOperations();
    testLargeScaleDeletePattern();

    std::cout << "\n✓ All edge case tests passed!" << std::endl;
    return 0;
}
