#include "../include/BPlusTree.h"
#include <iostream>
#include <string>

using namespace bptree;

void printSeparator() {
    std::cout << "\n" << std::string(50, '=') << "\n" << std::endl;
}

void demonstrateBasicOperations() {
    std::cout << "1. Basic Operations Demo" << std::endl;
    printSeparator();

    BPlusTree<int, std::string> tree(4);

    // Insert some data
    std::cout << "Inserting key-value pairs..." << std::endl;
    tree.insert(10, "Apple");
    tree.insert(20, "Banana");
    tree.insert(5, "Cherry");
    tree.insert(15, "Date");
    tree.insert(25, "Elderberry");

    std::cout << "Tree structure:" << std::endl;
    tree.print();

    // Search
    std::cout << "\nSearching for keys..." << std::endl;
    std::string value;
    if (tree.search(15, value)) {
        std::cout << "Key 15 found: " << value << std::endl;
    }
    if (tree.search(100, value)) {
        std::cout << "Key 100 found: " << value << std::endl;
    } else {
        std::cout << "Key 100 not found" << std::endl;
    }

    std::cout << "\nTree height: " << tree.height() << std::endl;
    std::cout << "Tree valid: " << (tree.validate() ? "Yes" : "No") << std::endl;
}

void demonstrateRangeQuery() {
    std::cout << "2. Range Query Demo" << std::endl;
    printSeparator();

    BPlusTree<int, std::string> tree(4);

    // Insert data
    for (int i = 1; i <= 20; i++) {
        tree.insert(i, "Value_" + std::to_string(i));
    }

    std::cout << "Inserted keys 1-20" << std::endl;

    // Range queries
    std::cout << "\nRange query [5, 10]:" << std::endl;
    auto results = tree.rangeQuery(5, 10);
    for (const auto& [key, val] : results) {
        std::cout << "  " << key << ": " << val << std::endl;
    }

    std::cout << "\nRange query [15, 18]:" << std::endl;
    results = tree.rangeQuery(15, 18);
    for (const auto& [key, val] : results) {
        std::cout << "  " << key << ": " << val << std::endl;
    }
}

void demonstrateSplitting() {
    std::cout << "3. Node Splitting Demo" << std::endl;
    printSeparator();

    BPlusTree<int, int> tree(4);  // Order 4: max 3 keys per node

    std::cout << "Inserting 15 sequential values (will cause splits)..." << std::endl;
    for (int i = 1; i <= 15; i++) {
        tree.insert(i, i * 100);
        std::cout << "\nAfter inserting " << i << ":" << std::endl;
        tree.print();
    }

    std::cout << "\nFinal tree height: " << tree.height() << std::endl;
}

void demonstrateDeletion() {
    std::cout << "4. Deletion Demo" << std::endl;
    printSeparator();

    BPlusTree<int, std::string> tree(4);

    // Insert data
    std::cout << "Inserting keys 1-20..." << std::endl;
    for (int i = 1; i <= 20; i++) {
        tree.insert(i, "Item_" + std::to_string(i));
    }

    std::cout << "\nInitial tree:" << std::endl;
    tree.print();
    std::cout << "Height: " << tree.height() << std::endl;

    // Delete some keys
    std::cout << "\nDeleting keys: 5, 10, 15..." << std::endl;
    tree.remove(5);
    tree.remove(10);
    tree.remove(15);

    std::cout << "\nTree after deletions:" << std::endl;
    tree.print();
    std::cout << "Height: " << tree.height() << std::endl;

    // Verify
    std::string value;
    std::cout << "\nVerifying deletions:" << std::endl;
    std::cout << "Key 5 exists: " << (tree.search(5, value) ? "Yes" : "No") << std::endl;
    std::cout << "Key 6 exists: " << (tree.search(6, value) ? "Yes" : "No") << std::endl;
}

void demonstrateUpdate() {
    std::cout << "5. Update Demo" << std::endl;
    printSeparator();

    BPlusTree<int, std::string> tree(4);

    tree.insert(10, "Original_Value");
    std::cout << "Inserted key 10 with value: Original_Value" << std::endl;

    std::string value;
    tree.search(10, value);
    std::cout << "Current value: " << value << std::endl;

    tree.insert(10, "Updated_Value");
    std::cout << "\nUpdated key 10 to: Updated_Value" << std::endl;

    tree.search(10, value);
    std::cout << "New value: " << value << std::endl;
}

void demonstrateLargeDataset() {
    std::cout << "6. Large Dataset Demo" << std::endl;
    printSeparator();

    BPlusTree<int, int> tree(5);

    std::cout << "Inserting 1000 elements..." << std::endl;
    for (int i = 0; i < 1000; i++) {
        tree.insert(i, i * 2);
    }

    std::cout << "Insertion complete!" << std::endl;
    std::cout << "Tree height: " << tree.height() << std::endl;
    std::cout << "Tree valid: " << (tree.validate() ? "Yes" : "No") << std::endl;

    // Test some operations
    int value;
    std::cout << "\nSearching for key 500..." << std::endl;
    if (tree.search(500, value)) {
        std::cout << "Found: " << value << std::endl;
    }

    std::cout << "\nRange query [100, 105]:" << std::endl;
    auto results = tree.rangeQuery(100, 105);
    for (const auto& [key, val] : results) {
        std::cout << "  " << key << ": " << val << std::endl;
    }

    std::cout << "\nDeleting 500 random elements..." << std::endl;
    for (int i = 0; i < 500; i++) {
        tree.remove(i * 2);
    }
    std::cout << "Deletion complete!" << std::endl;
    std::cout << "Tree height: " << tree.height() << std::endl;
    std::cout << "Tree valid: " << (tree.validate() ? "Yes" : "No") << std::endl;
}

int main() {
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════╗\n";
    std::cout << "║        B+ Tree Implementation Demo            ║\n";
    std::cout << "╚════════════════════════════════════════════════╝\n";
    std::cout << std::endl;

    demonstrateBasicOperations();
    printSeparator();

    demonstrateRangeQuery();
    printSeparator();

    demonstrateSplitting();
    printSeparator();

    demonstrateDeletion();
    printSeparator();

    demonstrateUpdate();
    printSeparator();

    demonstrateLargeDataset();

    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════╗\n";
    std::cout << "║             Demo Complete!                     ║\n";
    std::cout << "╚════════════════════════════════════════════════╝\n";
    std::cout << std::endl;

    return 0;
}
