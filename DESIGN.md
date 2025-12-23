# B+ Tree Design Document

## 1. Overview

This document describes the design and implementation of an in-memory B+ tree data structure in C++. The implementation is designed for educational purposes with emphasis on clarity and correctness.

### 1.1 What is a B+ Tree?

A B+ tree is a self-balancing tree data structure that maintains sorted data and allows searches, sequential access, insertions, and deletions in logarithmic time. It is an extension of the B-tree with the following key characteristics:

- All keys are stored in leaf nodes
- Internal nodes only store keys for navigation
- Leaf nodes are linked together forming a linked list for efficient range queries
- All leaf nodes are at the same level (balanced tree)

### 1.2 Advantages

- **Efficient range queries**: Linked leaf nodes allow fast sequential access
- **Balanced structure**: All operations guaranteed O(log n) time
- **Good cache performance**: Nodes contain multiple keys reducing tree height
- **Predictable performance**: No worst-case degradation unlike BST

## 2. Design Parameters

### 2.1 Order (Degree)

The order `m` of a B+ tree defines:
- Maximum number of children per internal node: `m`
- Maximum number of keys per internal node: `m - 1`
- Maximum number of keys per leaf node: `m - 1`
- Minimum number of keys (except root): `⌈m/2⌉ - 1`

**Default**: `m = 4` (suitable for demonstration and debugging)

### 2.2 Key and Value Types

The implementation will use template-based generic types:
- `KeyType`: Must support comparison operators (`<`, `>`, `==`)
- `ValueType`: The data associated with each key (only stored in leaf nodes)

## 3. Data Structures

### 3.1 Node Types

#### Base Node
```cpp
enum class NodeType { INTERNAL, LEAF };

class Node {
    NodeType type;
    int numKeys;
    KeyType keys[MAX_KEYS];
    Node* parent;
};
```

#### Internal Node
```cpp
class InternalNode : public Node {
    Node* children[MAX_CHILDREN];  // size = MAX_KEYS + 1
};
```

Properties:
- Contains `n` keys and `n+1` child pointers
- Key `keys[i]` represents the smallest key in subtree `children[i+1]`
- Keys guide the search but are not the actual data

#### Leaf Node
```cpp
class LeafNode : public Node {
    ValueType values[MAX_KEYS];
    LeafNode* next;  // Pointer to next leaf (for range queries)
    LeafNode* prev;  // Pointer to previous leaf (optional, for bidirectional traversal)
};
```

Properties:
- Contains actual key-value pairs
- Linked to adjacent leaf nodes for sequential access
- No child pointers (always at the bottom level)

### 3.2 Tree Structure

```cpp
class BPlusTree {
private:
    Node* root;
    int order;  // m
    int maxKeys;  // m - 1
    int minKeys;  // ⌈m/2⌉ - 1

public:
    // Core operations
    bool search(const KeyType& key, ValueType& value);
    void insert(const KeyType& key, const ValueType& value);
    bool remove(const KeyType& key);

    // Range query
    std::vector<std::pair<KeyType, ValueType>> rangeQuery(
        const KeyType& start,
        const KeyType& end
    );

    // Utility
    void print();
    int height();
    bool validate();
};
```

## 4. Core Operations

### 4.1 Search

**Algorithm**:
1. Start at root
2. If current node is internal:
   - Find the appropriate child pointer using binary search on keys
   - Recurse down to that child
3. If current node is leaf:
   - Search for the key in the leaf's key array
   - Return the associated value if found

**Time Complexity**: O(log n)

### 4.2 Insertion

**Algorithm**:
1. **Find leaf position**: Search down the tree to find the appropriate leaf node
2. **Insert into leaf**: Insert key-value pair in sorted order
3. **Handle overflow**: If leaf has more than `maxKeys`:
   - Split the leaf into two nodes
   - Distribute keys evenly
   - Update the linked list pointers
   - Promote the middle key to parent
4. **Propagate split**: If parent overflows, recursively split internal nodes
5. **Root split**: If root splits, create a new root (tree height increases)

**Time Complexity**: O(log n)

**Special Cases**:
- Duplicate keys: Either reject, replace value, or allow (design choice)
- Empty tree: Create new root as a leaf node

### 4.3 Deletion

**Algorithm**:
1. **Find and remove**: Locate the key in a leaf node and remove it
2. **Handle underflow**: If leaf has fewer than `minKeys`:
   - **Borrow**: Try to borrow a key from a sibling
   - **Merge**: If borrowing fails, merge with a sibling
3. **Update parent keys**: Adjust keys in internal nodes if necessary
4. **Propagate merge**: Recursively handle underflow in internal nodes
5. **Reduce height**: If root becomes empty, make its only child the new root

**Time Complexity**: O(log n)

**Borrowing Conditions**:
- From right sibling: Sibling has more than `minKeys`
- From left sibling: Sibling has more than `minKeys`

**Merging**:
- Combine current node with a sibling
- Remove the separator key from parent
- May cause parent to underflow (propagate)

### 4.4 Range Query

**Algorithm**:
1. Search for the start key to find the starting leaf node
2. Traverse the linked list of leaf nodes
3. Collect all key-value pairs where `start ≤ key ≤ end`
4. Stop when a key exceeds the end value or reach the end of the list

**Time Complexity**: O(log n + k), where k is the number of results

## 5. Implementation Details

### 5.1 Node Splitting

**Leaf Node Split**:
```
Before: [1, 3, 5, 7] (overflow with maxKeys=3)
After:  [1, 3] -> [5, 7]
Promote: 5 to parent
```

**Internal Node Split**:
```
Before: keys=[10, 20, 30], children=[c0, c1, c2, c3]
After:  Left: keys=[10], children=[c0, c1]
        Right: keys=[30], children=[c2, c3]
Promote: 20 to parent
```

### 5.2 Key Management

**Internal Node Keys**:
- Key `keys[i]` is the minimum key in the subtree `children[i+1]`
- After insertions/deletions, internal keys may need updating
- Only leaf nodes contain the authoritative key set

### 5.3 Memory Management

**Allocation**:
- Use `new` to allocate nodes dynamically
- Track node type to properly downcast and deallocate

**Deallocation**:
- Implement recursive destructor to free all nodes
- Clear the tree from bottom-up to avoid memory leaks

### 5.4 Helper Functions

```cpp
// Binary search in node's key array
int findKeyPosition(Node* node, const KeyType& key);

// Split a full node
void splitLeaf(LeafNode* leaf);
void splitInternal(InternalNode* node);

// Merge underflowed nodes
void mergeLeaves(LeafNode* left, LeafNode* right);
void mergeInternal(InternalNode* left, InternalNode* right);

// Borrow from siblings
bool borrowFromLeft(Node* node, Node* leftSibling, int parentIndex);
bool borrowFromRight(Node* node, Node* rightSibling, int parentIndex);

// Get sibling nodes
Node* getLeftSibling(Node* node);
Node* getRightSibling(Node* node);
```

## 6. Project Structure

```
B+tree/
├── DESIGN.md                 # This file
├── include/
│   ├── BPlusTree.h          # Template class declaration
│   ├── Node.h               # Node base class and derived classes
│   └── Config.h             # Configuration constants
├── src/
│   └── BPlusTree.cpp        # Template implementation (or .tpp)
├── tests/
│   ├── test_insert.cpp      # Unit tests for insertion
│   ├── test_delete.cpp      # Unit tests for deletion
│   ├── test_search.cpp      # Unit tests for search
│   └── test_range.cpp       # Unit tests for range queries
├── examples/
│   └── demo.cpp             # Example usage
├── CMakeLists.txt           # Build configuration
└── README.md                # Project overview and usage
```

## 7. Testing Strategy

### 7.1 Unit Tests

- **Insertion Tests**: Sequential, random, duplicate keys
- **Deletion Tests**: From leaf, causing merge, causing borrow
- **Search Tests**: Existing keys, non-existing keys
- **Range Query Tests**: Full range, partial range, empty range

### 7.2 Invariant Validation

Implement a `validate()` function to check:
- All leaves at the same depth
- Key counts within bounds (min/max)
- Keys in sorted order
- Parent-child key relationships correct
- Leaf linked list integrity

### 7.3 Stress Tests

- Large datasets (10,000+ elements)
- Random insertion and deletion sequences
- Edge cases (order=3, single element, etc.)

## 8. Performance Characteristics

| Operation | Time Complexity | Space Complexity |
|-----------|----------------|------------------|
| Search    | O(log n)       | O(1)            |
| Insert    | O(log n)       | O(log n)*       |
| Delete    | O(log n)       | O(log n)*       |
| Range Query | O(log n + k) | O(k)            |
| Height    | O(log n)       | -               |

*Recursive call stack depth

**Space Complexity**: O(n) for storing n key-value pairs

## 9. Extensions and Future Work

### 9.1 Potential Enhancements

- **Bulk loading**: Efficient construction from sorted data
- **Iterators**: STL-style iteration over the tree
- **Persistence**: Serialize/deserialize to disk
- **Concurrency**: Thread-safe operations with locking
- **Variable-length keys**: Support for strings and custom types

### 9.2 Optimizations

- **Lazy deletion**: Mark as deleted rather than immediate removal
- **Prefix compression**: Reduce key storage in internal nodes
- **Sibling pointers**: Faster sibling access without parent traversal
- **Custom allocators**: Reduce allocation overhead

## 10. References

- Comer, D. "The Ubiquitous B-Tree", ACM Computing Surveys, 1979
- Cormen, T. H., et al. "Introduction to Algorithms" (CLRS), Chapter 18
- "Database System Concepts" by Silberschatz, Korth, and Sudarshan
