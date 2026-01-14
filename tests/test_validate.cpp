#include "../include/BPlusTree.h"
#include <iostream>
#include <cassert>
#include <string>
#include <random>

using namespace bptree;

/**
 * Tests for the validate() function
 * Since the tree is well-encapsulated and maintains invariants,
 * we test that validate() correctly returns true for all valid states
 * and that it works correctly throughout the tree's lifecycle
 */

void testValidateEmptyTree() {
    BPlusTree<int, std::string> tree(4);
    assert(tree.validate());
    std::cout << "✓ Validate empty tree test passed" << std::endl;
}

void testValidateSingleElement() {
    BPlusTree<int, std::string> tree(4);
    tree.insert(10, "value10");
    assert(tree.validate());
    std::cout << "✓ Validate single element test passed" << std::endl;
}

void testValidateAfterEachInsert() {
    BPlusTree<int, std::string> tree(4);

    // Validate after each insert
    for (int i = 1; i <= 50; i++) {
        tree.insert(i, "value" + std::to_string(i));
        assert(tree.validate());
    }

    std::cout << "✓ Validate after each insert test passed (50 ops)" << std::endl;
}

void testValidateAfterEachDelete() {
    BPlusTree<int, int> tree(4);

    // Build tree
    for (int i = 1; i <= 50; i++) {
        tree.insert(i, i * 10);
    }

    // Validate after each delete
    for (int i = 1; i <= 50; i++) {
        tree.remove(i);
        if (i < 50) {  // Don't validate empty tree in this test
            assert(tree.validate());
        }
    }

    std::cout << "✓ Validate after each delete test passed (50 ops)" << std::endl;
}

void testValidateAfterSplits() {
    BPlusTree<int, std::string> tree(4);  // Will cause splits

    // Insert in order that causes multiple splits
    for (int i = 1; i <= 30; i++) {
        tree.insert(i, "value" + std::to_string(i));
    }

    assert(tree.validate());

    // Insert in reverse to cause different split patterns
    for (int i = 100; i >= 70; i--) {
        tree.insert(i, "value" + std::to_string(i));
    }

    assert(tree.validate());

    std::cout << "✓ Validate after splits test passed" << std::endl;
}

void testValidateAfterMerges() {
    BPlusTree<int, int> tree(4);

    // Build tree
    for (int i = 1; i <= 40; i++) {
        tree.insert(i, i * 100);
    }

    assert(tree.validate());

    // Delete to trigger merges
    for (int i = 1; i <= 30; i++) {
        tree.remove(i);
        assert(tree.validate());
    }

    std::cout << "✓ Validate after merges test passed" << std::endl;
}

void testValidateAfterBorrowing() {
    BPlusTree<int, std::string> tree(5);

    // Create structure
    for (int i = 1; i <= 25; i++) {
        tree.insert(i, "value" + std::to_string(i));
    }

    assert(tree.validate());

    // Delete to trigger borrowing from siblings
    tree.remove(12);
    assert(tree.validate());

    tree.remove(13);
    assert(tree.validate());

    tree.remove(14);
    assert(tree.validate());

    std::cout << "✓ Validate after borrowing test passed" << std::endl;
}

void testValidateRandomOperations() {
    BPlusTree<int, int> tree(4);

    std::mt19937 gen(42);
    std::uniform_int_distribution<> keyDist(1, 200);
    std::uniform_int_distribution<> opDist(0, 1);

    // Random operations with validation
    for (int i = 0; i < 500; i++) {
        int op = opDist(gen);
        int key = keyDist(gen);

        if (op == 0) {
            tree.insert(key, key * 5);
        } else {
            tree.remove(key);
        }

        // Validate every 10 operations
        if (i % 10 == 0) {
            assert(tree.validate());
        }
    }

    // Final validation
    assert(tree.validate());

    std::cout << "✓ Validate random operations test passed (500 ops)" << std::endl;
}

void testValidateMinOrderTree() {
    BPlusTree<int, std::string> tree(3);  // Minimum order

    // Operations on minimum order tree
    for (int i = 1; i <= 30; i++) {
        tree.insert(i, "value" + std::to_string(i));
        assert(tree.validate());
    }

    for (int i = 1; i <= 25; i++) {
        tree.remove(i);
        assert(tree.validate());
    }

    std::cout << "✓ Validate min order tree test passed" << std::endl;
}

void testValidateHighOrderTree() {
    BPlusTree<int, int> tree(20);  // High order

    // Operations on high order tree
    for (int i = 1; i <= 100; i++) {
        tree.insert(i, i * 2);
        if (i % 10 == 0) {
            assert(tree.validate());
        }
    }

    for (int i = 1; i <= 80; i++) {
        tree.remove(i);
        if (i % 10 == 0) {
            assert(tree.validate());
        }
    }

    assert(tree.validate());

    std::cout << "✓ Validate high order tree test passed" << std::endl;
}

void testValidateAfterUpdate() {
    BPlusTree<int, std::string> tree(4);

    // Insert
    for (int i = 1; i <= 20; i++) {
        tree.insert(i, "original" + std::to_string(i));
    }

    assert(tree.validate());

    // Update values (insert with same key)
    for (int i = 1; i <= 20; i++) {
        tree.insert(i, "updated" + std::to_string(i));
        if (i % 5 == 0) {
            assert(tree.validate());
        }
    }

    assert(tree.validate());

    std::cout << "✓ Validate after update test passed" << std::endl;
}

void testValidateAfterRootChange() {
    BPlusTree<int, int> tree(4);

    // Build tree with multiple levels
    for (int i = 1; i <= 30; i++) {
        tree.insert(i, i * 10);
    }

    int initialHeight = tree.height();
    assert(tree.validate());

    // Delete elements to potentially change root
    for (int i = 1; i <= 25; i++) {
        tree.remove(i);
    }

    int finalHeight = tree.height();
    assert(tree.validate());

    // Height may have changed due to merges
    assert(finalHeight <= initialHeight);

    std::cout << "✓ Validate after root change test passed" << std::endl;
}

void testValidateDifferentKeyTypes() {
    // Test with different key types to ensure validate works generically

    // Integer keys
    BPlusTree<int, std::string> intTree(4);
    for (int i = 1; i <= 20; i++) {
        intTree.insert(i, "value" + std::to_string(i));
    }
    assert(intTree.validate());

    // Double keys
    BPlusTree<double, int> doubleTree(4);
    for (int i = 1; i <= 20; i++) {
        doubleTree.insert(i * 1.5, i);
    }
    assert(doubleTree.validate());

    std::cout << "✓ Validate different key types test passed" << std::endl;
}

void testValidateSequentialPatterns() {
    BPlusTree<int, int> tree(4);

    // Sequential insert
    for (int i = 1; i <= 50; i++) {
        tree.insert(i, i);
    }
    assert(tree.validate());

    // Sequential delete from start
    for (int i = 1; i <= 25; i++) {
        tree.remove(i);
    }
    assert(tree.validate());

    // Sequential delete from end
    for (int i = 50; i >= 26; i--) {
        tree.remove(i);
    }
    assert(tree.isEmpty());

    std::cout << "✓ Validate sequential patterns test passed" << std::endl;
}

void testValidateAfterComplexMergePattern() {
    BPlusTree<int, std::string> tree(4);

    // Create complex tree structure
    for (int i = 1; i <= 60; i++) {
        tree.insert(i, "value" + std::to_string(i));
    }

    assert(tree.validate());

    // Delete in pattern that causes complex merges
    for (int i = 10; i <= 50; i += 3) {
        tree.remove(i);
        assert(tree.validate());
    }

    std::cout << "✓ Validate after complex merge pattern test passed" << std::endl;
}

void testValidateRepeatedOperations() {
    BPlusTree<int, int> tree(5);

    // Multiple rounds of insert/delete
    for (int round = 0; round < 5; round++) {
        // Insert
        for (int i = 1; i <= 30; i++) {
            tree.insert(i + round * 100, i);
        }
        assert(tree.validate());

        // Delete half
        for (int i = 1; i <= 15; i++) {
            tree.remove(i + round * 100);
        }
        assert(tree.validate());
    }

    std::cout << "✓ Validate repeated operations test passed" << std::endl;
}

void testValidateAfterRangeOperations() {
    BPlusTree<int, int> tree(4);

    // Insert
    for (int i = 1; i <= 100; i++) {
        tree.insert(i, i * 2);
    }

    assert(tree.validate());

    // Perform range queries (shouldn't affect validation)
    auto results1 = tree.rangeQuery(10, 20);
    assert(tree.validate());

    auto results2 = tree.rangeQuery(50, 80);
    assert(tree.validate());

    // Delete some elements
    for (int i = 30; i <= 60; i++) {
        tree.remove(i);
    }

    assert(tree.validate());

    // More range queries
    auto results3 = tree.rangeQuery(1, 100);
    assert(tree.validate());

    std::cout << "✓ Validate after range operations test passed" << std::endl;
}

void testValidateEdgeCases() {
    // Test various edge cases

    // Tree with order just above minimum
    BPlusTree<int, int> tree1(3);
    tree1.insert(1, 1);
    tree1.insert(2, 2);
    tree1.insert(3, 3);
    assert(tree1.validate());
    tree1.remove(2);
    assert(tree1.validate());

    // Tree with many duplicate updates
    BPlusTree<int, std::string> tree2(4);
    for (int round = 0; round < 10; round++) {
        for (int i = 1; i <= 5; i++) {
            tree2.insert(i, "round" + std::to_string(round));
        }
        assert(tree2.validate());
    }

    // Tree built then emptied then rebuilt
    BPlusTree<int, int> tree3(4);
    for (int i = 1; i <= 20; i++) tree3.insert(i, i);
    for (int i = 1; i <= 20; i++) tree3.remove(i);
    for (int i = 21; i <= 40; i++) tree3.insert(i, i);
    assert(tree3.validate());

    std::cout << "✓ Validate edge cases test passed" << std::endl;
}

void testValidateStressWithValidation() {
    BPlusTree<int, int> tree(4);

    std::mt19937 gen(999);
    std::uniform_int_distribution<> keyDist(1, 500);
    std::uniform_int_distribution<> opDist(0, 1);

    // Heavy stress test with frequent validation
    for (int i = 0; i < 1000; i++) {
        int op = opDist(gen);
        int key = keyDist(gen);

        if (op == 0) {
            tree.insert(key, key * 3);
        } else {
            tree.remove(key);
        }

        // Validate every single operation (expensive but thorough)
        assert(tree.validate());
    }

    std::cout << "✓ Validate stress test passed (1000 ops with validation each)" << std::endl;
}

int main() {
    std::cout << "Running validate() function tests..." << std::endl;
    std::cout << "====================================" << std::endl;

    testValidateEmptyTree();
    testValidateSingleElement();
    testValidateAfterEachInsert();
    testValidateAfterEachDelete();
    testValidateAfterSplits();
    testValidateAfterMerges();
    testValidateAfterBorrowing();
    testValidateRandomOperations();
    testValidateMinOrderTree();
    testValidateHighOrderTree();
    testValidateAfterUpdate();
    testValidateAfterRootChange();
    testValidateDifferentKeyTypes();
    testValidateSequentialPatterns();
    testValidateAfterComplexMergePattern();
    testValidateRepeatedOperations();
    testValidateAfterRangeOperations();
    testValidateEdgeCases();
    testValidateStressWithValidation();

    std::cout << "\n✓ All validate() tests passed!" << std::endl;
    std::cout << "  Note: Since the tree maintains invariants, all tests" << std::endl;
    std::cout << "  verify that validate() correctly returns true for valid states." << std::endl;
    return 0;
}
