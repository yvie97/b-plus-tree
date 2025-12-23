#ifndef BPLUSTREE_CONFIG_H
#define BPLUSTREE_CONFIG_H

namespace bptree {

// Default order for B+ tree (can be overridden at runtime)
constexpr int DEFAULT_ORDER = 4;

// Minimum order allowed
constexpr int MIN_ORDER = 3;

} // namespace bptree

#endif // BPLUSTREE_CONFIG_H
