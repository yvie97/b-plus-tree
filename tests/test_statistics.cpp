#include "../include/BPlusTree.h"
#include <iostream>
#include <cassert>
#include <string>
#include <cmath>

using namespace bptree;

void testInitialStatistics() {
    BPlusTree<int, std::string> tree(4);

    const Statistics& stats = tree.statistics();
    assert(stats.leafNodeCount == 0);
    assert(stats.internalNodeCount == 0);
    assert(stats.insertCount == 0);
    assert(stats.removeCount == 0);
    assert(stats.searchCount == 0);
    assert(stats.searchHitCount == 0);
    assert(stats.leafSplitCount == 0);
    assert(stats.internalSplitCount == 0);
    assert(stats.leafMergeCount == 0);
    assert(stats.internalMergeCount == 0);
    assert(stats.redistributeCount == 0);

    std::cout << "✓ Initial statistics test passed" << std::endl;
}

void testNodeCountAfterInserts() {
    BPlusTree<int, std::string> tree(4);

    // Insert a single element - should create 1 leaf node
    tree.insert(10, "value10");

    const Statistics& stats = tree.statistics();
    assert(stats.leafNodeCount == 1);
    assert(stats.internalNodeCount == 0);
    assert(stats.insertCount == 1);

    std::cout << "✓ Node count after single insert test passed" << std::endl;
}

void testInsertCountTracking() {
    BPlusTree<int, std::string> tree(4);

    for (int i = 1; i <= 20; i++) {
        tree.insert(i, "value" + std::to_string(i));
    }

    const Statistics& stats = tree.statistics();
    assert(stats.insertCount == 20);

    // Updating an existing key should also count as insert
    tree.insert(5, "updated_value5");
    assert(stats.insertCount == 21);

    std::cout << "✓ Insert count tracking test passed" << std::endl;
}

void testSearchCountTracking() {
    BPlusTree<int, std::string> tree(4);

    for (int i = 1; i <= 10; i++) {
        tree.insert(i, "value" + std::to_string(i));
    }

    const Statistics& stats = tree.statistics();
    std::string value;

    // Successful searches
    for (int i = 1; i <= 5; i++) {
        tree.search(i, value);
    }

    assert(stats.searchCount == 5);
    assert(stats.searchHitCount == 5);

    // Failed searches
    tree.search(100, value);
    tree.search(200, value);

    assert(stats.searchCount == 7);
    assert(stats.searchHitCount == 5);

    std::cout << "✓ Search count tracking test passed" << std::endl;
}

void testRemoveCountTracking() {
    BPlusTree<int, std::string> tree(4);

    for (int i = 1; i <= 10; i++) {
        tree.insert(i, "value" + std::to_string(i));
    }

    const Statistics& stats = tree.statistics();

    // Successful removes
    tree.remove(5);
    tree.remove(3);
    tree.remove(7);

    assert(stats.removeCount == 3);

    // Failed removes (key doesn't exist)
    bool removed = tree.remove(100);
    assert(!removed);
    assert(stats.removeCount == 3);  // Should not increment for failed removes

    std::cout << "✓ Remove count tracking test passed" << std::endl;
}

void testSplitCountTracking() {
    BPlusTree<int, std::string> tree(3);  // Small order to trigger splits

    // Insert enough elements to cause splits
    for (int i = 1; i <= 20; i++) {
        tree.insert(i, "value" + std::to_string(i));
    }

    const Statistics& stats = tree.statistics();
    assert(stats.leafSplitCount > 0);
    assert(stats.totalSplitCount() >= stats.leafSplitCount);

    std::cout << "✓ Split count tracking test passed" << std::endl;
}

void testMergeAndRedistributeTracking() {
    BPlusTree<int, std::string> tree(3);  // Small order for more operations

    // Insert elements
    for (int i = 1; i <= 30; i++) {
        tree.insert(i, "value" + std::to_string(i));
    }

    const Statistics& stats = tree.statistics();
    size_t initialMerges = stats.totalMergeCount();
    size_t initialRedistributes = stats.redistributeCount;

    // Remove elements to potentially trigger merges or redistributions
    for (int i = 1; i <= 20; i++) {
        tree.remove(i);
    }

    // Either merges or redistributions should have occurred
    size_t totalRebalancing = (stats.totalMergeCount() - initialMerges) +
                              (stats.redistributeCount - initialRedistributes);
    assert(totalRebalancing > 0);

    std::cout << "✓ Merge and redistribute tracking test passed" << std::endl;
}

void testSizeMethod() {
    BPlusTree<int, std::string> tree(4);

    assert(tree.size() == 0);

    for (int i = 1; i <= 50; i++) {
        tree.insert(i, "value" + std::to_string(i));
    }

    assert(tree.size() == 50);

    tree.remove(25);
    tree.remove(30);
    assert(tree.size() == 48);

    std::cout << "✓ Size method test passed" << std::endl;
}

void testTotalNodeCount() {
    BPlusTree<int, std::string> tree(3);

    for (int i = 1; i <= 30; i++) {
        tree.insert(i, "value" + std::to_string(i));
    }

    const Statistics& stats = tree.statistics();
    assert(stats.totalNodeCount() == stats.leafNodeCount + stats.internalNodeCount);
    assert(stats.leafNodeCount > 0);

    std::cout << "✓ Total node count test passed" << std::endl;
}

void testFillFactor() {
    BPlusTree<int, std::string> tree(4);

    // Empty tree should return 0.0
    assert(tree.averageLeafFillFactor() == 0.0);
    assert(tree.averageInternalFillFactor() == 0.0);

    // Insert elements
    for (int i = 1; i <= 20; i++) {
        tree.insert(i, "value" + std::to_string(i));
    }

    double leafFill = tree.averageLeafFillFactor();
    double internalFill = tree.averageInternalFillFactor();

    // Fill factors should be between 0 and 1
    assert(leafFill > 0.0 && leafFill <= 1.0);

    // If there are internal nodes, check their fill factor
    if (tree.statistics().internalNodeCount > 0) {
        assert(internalFill > 0.0 && internalFill <= 1.0);
    }

    std::cout << "✓ Fill factor test passed" << std::endl;
}

void testResetStatistics() {
    BPlusTree<int, std::string> tree(4);

    for (int i = 1; i <= 20; i++) {
        tree.insert(i, "value" + std::to_string(i));
    }

    std::string value;
    tree.search(5, value);
    tree.remove(10);

    const Statistics& stats = tree.statistics();
    size_t nodeCount = stats.totalNodeCount();

    // Reset statistics
    tree.resetStatistics();

    // Node counts should be preserved
    assert(stats.leafNodeCount + stats.internalNodeCount == nodeCount);

    // Operation counters should be reset
    assert(stats.insertCount == 0);
    assert(stats.removeCount == 0);
    assert(stats.searchCount == 0);
    assert(stats.searchHitCount == 0);
    assert(stats.leafSplitCount == 0);
    assert(stats.internalSplitCount == 0);
    assert(stats.leafMergeCount == 0);
    assert(stats.internalMergeCount == 0);
    assert(stats.redistributeCount == 0);

    std::cout << "✓ Reset statistics test passed" << std::endl;
}

void testGetStatisticsCopy() {
    BPlusTree<int, std::string> tree(4);

    for (int i = 1; i <= 10; i++) {
        tree.insert(i, "value" + std::to_string(i));
    }

    // Get a copy of statistics
    Statistics statsCopy = tree.getStatistics();
    size_t insertsBefore = statsCopy.insertCount;

    // Do more operations
    tree.insert(100, "value100");

    // The copy should not have been modified
    assert(statsCopy.insertCount == insertsBefore);

    // But the tree's statistics should be updated
    assert(tree.statistics().insertCount == insertsBefore + 1);

    std::cout << "✓ Get statistics copy test passed" << std::endl;
}

void testMoveConstructorStatistics() {
    BPlusTree<int, std::string> tree1(4);

    for (int i = 1; i <= 20; i++) {
        tree1.insert(i, "value" + std::to_string(i));
    }

    Statistics stats1 = tree1.getStatistics();
    size_t leafCount = stats1.leafNodeCount;
    size_t insertCount = stats1.insertCount;

    // Move construct tree2
    BPlusTree<int, std::string> tree2(std::move(tree1));

    // tree2 should have the statistics from tree1
    const Statistics& stats2 = tree2.statistics();
    assert(stats2.leafNodeCount == leafCount);
    assert(stats2.insertCount == insertCount);

    // tree1 should be empty with reset stats
    assert(tree1.isEmpty());
    assert(tree1.statistics().leafNodeCount == 0);
    assert(tree1.statistics().insertCount == 0);

    std::cout << "✓ Move constructor statistics test passed" << std::endl;
}

void testMoveAssignmentStatistics() {
    BPlusTree<int, std::string> tree1(4);
    BPlusTree<int, std::string> tree2(4);

    for (int i = 1; i <= 20; i++) {
        tree1.insert(i, "value" + std::to_string(i));
    }

    for (int i = 100; i <= 110; i++) {
        tree2.insert(i, "value" + std::to_string(i));
    }

    Statistics stats1 = tree1.getStatistics();
    size_t leafCount = stats1.leafNodeCount;
    size_t insertCount = stats1.insertCount;

    // Move assign tree1 to tree2
    tree2 = std::move(tree1);

    // tree2 should now have the statistics from tree1
    const Statistics& stats2 = tree2.statistics();
    assert(stats2.leafNodeCount == leafCount);
    assert(stats2.insertCount == insertCount);

    std::cout << "✓ Move assignment statistics test passed" << std::endl;
}

void testStatisticsAfterBulkLoad() {
    BPlusTree<int, std::string> tree(4);

    // First do some regular inserts
    for (int i = 1; i <= 5; i++) {
        tree.insert(i, "value" + std::to_string(i));
    }

    // Now bulk load (should clear and rebuild)
    std::vector<std::pair<int, std::string>> data;
    for (int i = 1; i <= 30; i++) {
        data.emplace_back(i, "value" + std::to_string(i));
    }
    tree.bulkLoad(data);

    const Statistics& stats = tree.statistics();
    assert(stats.leafNodeCount > 0);
    assert(tree.size() == 30);

    std::cout << "✓ Statistics after bulk load test passed" << std::endl;
}

void testStatisticsHelperMethods() {
    Statistics stats;
    stats.leafNodeCount = 10;
    stats.internalNodeCount = 5;
    stats.leafSplitCount = 8;
    stats.internalSplitCount = 3;
    stats.leafMergeCount = 2;
    stats.internalMergeCount = 1;

    assert(stats.totalNodeCount() == 15);
    assert(stats.totalSplitCount() == 11);
    assert(stats.totalMergeCount() == 3);

    stats.reset();
    assert(stats.totalNodeCount() == 0);
    assert(stats.totalSplitCount() == 0);
    assert(stats.totalMergeCount() == 0);

    std::cout << "✓ Statistics helper methods test passed" << std::endl;
}

void testNodeCountConsistency() {
    BPlusTree<int, std::string> tree(3);

    // Insert many elements
    for (int i = 1; i <= 100; i++) {
        tree.insert(i, "value" + std::to_string(i));
    }

    const Statistics& stats = tree.statistics();
    size_t leafCountBefore = stats.leafNodeCount;
    size_t internalCountBefore = stats.internalNodeCount;

    // Remove many elements
    for (int i = 1; i <= 80; i++) {
        tree.remove(i);
    }

    // Node counts should have decreased (or stayed same if just keys removed)
    assert(stats.leafNodeCount <= leafCountBefore);
    assert(tree.size() == 20);
    assert(tree.validate());

    std::cout << "✓ Node count consistency test passed" << std::endl;
}

int main() {
    std::cout << "Running statistics tests..." << std::endl;
    std::cout << std::endl;

    testInitialStatistics();
    testNodeCountAfterInserts();
    testInsertCountTracking();
    testSearchCountTracking();
    testRemoveCountTracking();
    testSplitCountTracking();
    testMergeAndRedistributeTracking();
    testSizeMethod();
    testTotalNodeCount();
    testFillFactor();
    testResetStatistics();
    testGetStatisticsCopy();
    testMoveConstructorStatistics();
    testMoveAssignmentStatistics();
    testStatisticsAfterBulkLoad();
    testStatisticsHelperMethods();
    testNodeCountConsistency();

    std::cout << std::endl;
    std::cout << "All statistics tests passed!" << std::endl;
    return 0;
}
