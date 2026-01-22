#include "../include/BPlusTree.h"
#include <iostream>
#include <cassert>
#include <cstdio>
#include <vector>
#include <cstdlib>

using namespace bptree;

// Helper to generate a unique temp filename
std::string getTempFilename() {
    static int counter = 0;
    return "test_bptree_" + std::to_string(counter++) + ".dat";
}

// Helper to clean up temp files
void removeFile(const std::string& filename) {
    std::remove(filename.c_str());
}

void testSaveLoadEmpty() {
    std::string filename = getTempFilename();

    {
        BPlusTree<int, int> tree(4);
        tree.save(filename);
    }

    {
        BPlusTree<int, int> tree(4);
        tree.load(filename);
        assert(tree.isEmpty());
        assert(tree.validate());
    }

    removeFile(filename);
    std::cout << "✓ Save/load empty tree test passed" << std::endl;
}

void testSaveLoadSingleElement() {
    std::string filename = getTempFilename();

    {
        BPlusTree<int, int> tree(4);
        tree.insert(42, 100);
        tree.save(filename);
    }

    {
        BPlusTree<int, int> tree(4);
        tree.load(filename);

        int value;
        assert(tree.search(42, value));
        assert(value == 100);
        assert(tree.validate());
    }

    removeFile(filename);
    std::cout << "✓ Save/load single element test passed" << std::endl;
}

void testSaveLoadSmall() {
    std::string filename = getTempFilename();

    {
        BPlusTree<int, int> tree(4);
        for (int i = 0; i < 10; ++i) {
            tree.insert(i, i * 10);
        }
        tree.save(filename);
    }

    {
        BPlusTree<int, int> tree(4);
        tree.load(filename);

        int value;
        for (int i = 0; i < 10; ++i) {
            assert(tree.search(i, value));
            assert(value == i * 10);
        }
        assert(tree.validate());
    }

    removeFile(filename);
    std::cout << "✓ Save/load small tree test passed" << std::endl;
}

void testSaveLoadMedium() {
    std::string filename = getTempFilename();

    {
        BPlusTree<int, int> tree(5);
        for (int i = 0; i < 1000; ++i) {
            tree.insert(i, i * 2);
        }
        tree.save(filename);
    }

    {
        BPlusTree<int, int> tree(5);
        tree.load(filename);

        int value;
        for (int i = 0; i < 1000; ++i) {
            assert(tree.search(i, value));
            assert(value == i * 2);
        }
        assert(tree.validate());
    }

    removeFile(filename);
    std::cout << "✓ Save/load medium tree test passed" << std::endl;
}

void testSaveLoadLarge() {
    std::string filename = getTempFilename();

    {
        BPlusTree<int, int> tree(10);
        for (int i = 0; i < 50000; ++i) {
            tree.insert(i, i * 3);
        }
        tree.save(filename);
    }

    {
        BPlusTree<int, int> tree(10);
        tree.load(filename);

        int value;
        for (int i = 0; i < 50000; ++i) {
            assert(tree.search(i, value));
            assert(value == i * 3);
        }
        assert(tree.validate());
    }

    removeFile(filename);
    std::cout << "✓ Save/load large tree test passed" << std::endl;
}

void testSaveLoadDoubleValues() {
    std::string filename = getTempFilename();

    {
        BPlusTree<int, double> tree(4);
        tree.insert(1, 1.5);
        tree.insert(2, 2.7);
        tree.insert(3, 3.14159);
        tree.save(filename);
    }

    {
        BPlusTree<int, double> tree(4);
        tree.load(filename);

        double value;
        assert(tree.search(1, value));
        assert(value == 1.5);
        assert(tree.search(2, value));
        assert(value == 2.7);
        assert(tree.search(3, value));
        assert(value == 3.14159);
        assert(tree.validate());
    }

    removeFile(filename);
    std::cout << "✓ Save/load double values test passed" << std::endl;
}

void testSaveLoadDifferentOrders() {
    for (size_t order = 3; order <= 10; ++order) {
        std::string filename = getTempFilename();

        {
            BPlusTree<int, int> tree(order);
            for (int i = 0; i < 200; ++i) {
                tree.insert(i, i);
            }
            tree.save(filename);
        }

        {
            BPlusTree<int, int> tree(order);
            tree.load(filename);

            int value;
            for (int i = 0; i < 200; ++i) {
                assert(tree.search(i, value));
                assert(value == i);
            }
            assert(tree.validate());
        }

        removeFile(filename);
    }

    std::cout << "✓ Save/load different orders test passed" << std::endl;
}

void testLoadFromFile() {
    std::string filename = getTempFilename();

    {
        BPlusTree<int, int> tree(7);  // Use non-default order
        for (int i = 0; i < 100; ++i) {
            tree.insert(i, i * 5);
        }
        tree.save(filename);
    }

    // Load using static factory method
    auto tree = BPlusTree<int, int>::loadFromFile(filename);

    int value;
    for (int i = 0; i < 100; ++i) {
        assert(tree.search(i, value));
        assert(value == i * 5);
    }
    assert(tree.validate());

    removeFile(filename);
    std::cout << "✓ loadFromFile test passed" << std::endl;
}

void testLoadOverwritesExisting() {
    std::string filename = getTempFilename();

    // Save a tree with certain data
    {
        BPlusTree<int, int> tree(4);
        tree.insert(100, 100);
        tree.insert(200, 200);
        tree.save(filename);
    }

    // Load into a tree that already has data
    {
        BPlusTree<int, int> tree(4);
        tree.insert(1, 1);
        tree.insert(2, 2);
        tree.insert(3, 3);

        tree.load(filename);

        // Old data should be gone
        int value;
        assert(!tree.search(1, value));
        assert(!tree.search(2, value));
        assert(!tree.search(3, value));

        // New data should exist
        assert(tree.search(100, value));
        assert(value == 100);
        assert(tree.search(200, value));
        assert(value == 200);
        assert(tree.validate());
    }

    removeFile(filename);
    std::cout << "✓ Load overwrites existing data test passed" << std::endl;
}

void testSaveLoadRangeQuery() {
    std::string filename = getTempFilename();

    {
        BPlusTree<int, int> tree(4);
        for (int i = 0; i < 100; ++i) {
            tree.insert(i, i);
        }
        tree.save(filename);
    }

    {
        BPlusTree<int, int> tree(4);
        tree.load(filename);

        auto result = tree.rangeQuery(25, 35);
        assert(result.size() == 11);
        for (int i = 0; i < 11; ++i) {
            assert(result[i].first == 25 + i);
            assert(result[i].second == 25 + i);
        }
        assert(tree.validate());
    }

    removeFile(filename);
    std::cout << "✓ Save/load range query test passed" << std::endl;
}

void testSaveLoadIterator() {
    std::string filename = getTempFilename();

    {
        BPlusTree<int, int> tree(4);
        for (int i = 0; i < 50; ++i) {
            tree.insert(i, i * 2);
        }
        tree.save(filename);
    }

    {
        BPlusTree<int, int> tree(4);
        tree.load(filename);

        // Test forward iteration
        int expected = 0;
        for (const auto& pair : tree) {
            assert(pair.first == expected);
            assert(pair.second == expected * 2);
            expected++;
        }
        assert(expected == 50);

        // Test reverse iteration
        expected = 49;
        for (auto it = tree.rbegin(); it != tree.rend(); ++it) {
            assert(it->first == expected);
            assert(it->second == expected * 2);
            expected--;
        }
        assert(expected == -1);
    }

    removeFile(filename);
    std::cout << "✓ Save/load iterator test passed" << std::endl;
}

void testModifyAfterLoad() {
    std::string filename = getTempFilename();

    {
        BPlusTree<int, int> tree(4);
        for (int i = 0; i < 50; i += 2) {
            tree.insert(i, i);  // Even numbers: 0, 2, 4, ..., 48
        }
        tree.save(filename);
    }

    {
        BPlusTree<int, int> tree(4);
        tree.load(filename);

        // Insert odd numbers
        for (int i = 1; i < 50; i += 2) {
            tree.insert(i, i);  // Odd numbers: 1, 3, 5, ..., 49
        }

        // Verify all numbers exist
        int value;
        for (int i = 0; i < 50; ++i) {
            assert(tree.search(i, value));
            assert(value == i);
        }

        // Delete some numbers
        for (int i = 0; i < 25; ++i) {
            assert(tree.remove(i * 2));  // Remove 0, 2, 4, ..., 48
        }

        // Verify only odd numbers remain
        for (int i = 0; i < 50; ++i) {
            if (i % 2 == 0) {
                assert(!tree.search(i, value));
            } else {
                assert(tree.search(i, value));
                assert(value == i);
            }
        }

        assert(tree.validate());
    }

    removeFile(filename);
    std::cout << "✓ Modify after load test passed" << std::endl;
}

void testInvalidFileFormat() {
    std::string filename = getTempFilename();

    // Create an invalid file
    {
        std::ofstream file(filename, std::ios::binary);
        const char* garbage = "This is not a valid B+ tree file";
        file.write(garbage, strlen(garbage));
    }

    {
        BPlusTree<int, int> tree(4);
        bool exceptionThrown = false;
        try {
            tree.load(filename);
        } catch (const std::runtime_error& e) {
            exceptionThrown = true;
            // Check that error message mentions invalid format
            std::string msg = e.what();
            assert(msg.find("Invalid") != std::string::npos ||
                   msg.find("invalid") != std::string::npos ||
                   msg.find("format") != std::string::npos);
        }
        assert(exceptionThrown);
    }

    removeFile(filename);
    std::cout << "✓ Invalid file format test passed" << std::endl;
}

void testNonexistentFile() {
    BPlusTree<int, int> tree(4);
    bool exceptionThrown = false;
    try {
        tree.load("nonexistent_file_12345.dat");
    } catch (const std::runtime_error& e) {
        exceptionThrown = true;
    }
    assert(exceptionThrown);

    std::cout << "✓ Nonexistent file test passed" << std::endl;
}

void testOrderMismatch() {
    std::string filename = getTempFilename();

    {
        BPlusTree<int, int> tree(5);  // Order 5
        tree.insert(1, 1);
        tree.save(filename);
    }

    {
        BPlusTree<int, int> tree(7);  // Order 7 - different!
        bool exceptionThrown = false;
        try {
            tree.load(filename);
        } catch (const std::logic_error& e) {
            exceptionThrown = true;
            std::string msg = e.what();
            assert(msg.find("order") != std::string::npos ||
                   msg.find("Order") != std::string::npos);
        }
        assert(exceptionThrown);
    }

    removeFile(filename);
    std::cout << "✓ Order mismatch test passed" << std::endl;
}

void testSaveLoadStructValues() {
    // Test with a simple POD struct
    struct Point {
        int x;
        int y;

        bool operator==(const Point& other) const {
            return x == other.x && y == other.y;
        }
    };

    std::string filename = getTempFilename();

    {
        BPlusTree<int, Point> tree(4);
        tree.insert(1, {10, 20});
        tree.insert(2, {30, 40});
        tree.insert(3, {50, 60});
        tree.save(filename);
    }

    {
        BPlusTree<int, Point> tree(4);
        tree.load(filename);

        Point value;
        assert(tree.search(1, value));
        assert((value == Point{10, 20}));
        assert(tree.search(2, value));
        assert((value == Point{30, 40}));
        assert(tree.search(3, value));
        assert((value == Point{50, 60}));
        assert(tree.validate());
    }

    removeFile(filename);
    std::cout << "✓ Save/load struct values test passed" << std::endl;
}

void testSaveLoadLongLongKeys() {
    std::string filename = getTempFilename();

    {
        BPlusTree<long long, long long> tree(4);
        tree.insert(1LL << 40, 100LL);
        tree.insert(1LL << 50, 200LL);
        tree.insert(1LL << 60, 300LL);
        tree.save(filename);
    }

    {
        BPlusTree<long long, long long> tree(4);
        tree.load(filename);

        long long value;
        assert(tree.search(1LL << 40, value));
        assert(value == 100LL);
        assert(tree.search(1LL << 50, value));
        assert(value == 200LL);
        assert(tree.search(1LL << 60, value));
        assert(value == 300LL);
        assert(tree.validate());
    }

    removeFile(filename);
    std::cout << "✓ Save/load long long keys test passed" << std::endl;
}

void testMultipleSaveLoad() {
    std::string filename = getTempFilename();

    BPlusTree<int, int> tree(4);

    // Save and load multiple times
    for (int round = 0; round < 5; ++round) {
        // Add some data
        for (int i = round * 10; i < (round + 1) * 10; ++i) {
            tree.insert(i, i * 100);
        }

        tree.save(filename);

        // Reload
        tree.load(filename);

        // Verify all data from all rounds
        int value;
        for (int i = 0; i < (round + 1) * 10; ++i) {
            assert(tree.search(i, value));
            assert(value == i * 100);
        }

        assert(tree.validate());
    }

    removeFile(filename);
    std::cout << "✓ Multiple save/load cycles test passed" << std::endl;
}

int main() {
    std::cout << "Running persistence tests..." << std::endl;

    testSaveLoadEmpty();
    testSaveLoadSingleElement();
    testSaveLoadSmall();
    testSaveLoadMedium();
    testSaveLoadLarge();
    testSaveLoadDoubleValues();
    testSaveLoadDifferentOrders();
    testLoadFromFile();
    testLoadOverwritesExisting();
    testSaveLoadRangeQuery();
    testSaveLoadIterator();
    testModifyAfterLoad();
    testInvalidFileFormat();
    testNonexistentFile();
    testOrderMismatch();
    testSaveLoadStructValues();
    testSaveLoadLongLongKeys();
    testMultipleSaveLoad();

    std::cout << "\n✓ All persistence tests passed!" << std::endl;
    return 0;
}
