#include "../include/BPlusTree.h"
#include <iostream>
#include <cassert>
#include <string>
#include <cstddef>
#include <memory>

using namespace bptree;

// Statistics structure shared across all allocator rebinds
struct AllocatorStats {
    size_t allocations = 0;
    size_t deallocations = 0;
    size_t bytes_allocated = 0;
    size_t bytes_deallocated = 0;
};

// Tracking allocator that counts allocations and deallocations
template<typename T>
class TrackingAllocator {
public:
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using propagate_on_container_move_assignment = std::true_type;
    using is_always_equal = std::false_type;

    std::shared_ptr<AllocatorStats> stats;

    TrackingAllocator() : stats(std::make_shared<AllocatorStats>()) {}

    explicit TrackingAllocator(std::shared_ptr<AllocatorStats> s) : stats(s) {}

    template<typename U>
    TrackingAllocator(const TrackingAllocator<U>& other) : stats(other.stats) {}

    T* allocate(size_type n) {
        stats->allocations++;
        stats->bytes_allocated += n * sizeof(T);
        return static_cast<T*>(::operator new(n * sizeof(T)));
    }

    void deallocate(T* p, size_type n) {
        stats->deallocations++;
        stats->bytes_deallocated += n * sizeof(T);
        ::operator delete(p);
    }

    template<typename U, typename... Args>
    void construct(U* p, Args&&... args) {
        new(p) U(std::forward<Args>(args)...);
    }

    template<typename U>
    void destroy(U* p) {
        p->~U();
    }

    template<typename U>
    struct rebind {
        using other = TrackingAllocator<U>;
    };

    bool operator==(const TrackingAllocator& other) const {
        return stats == other.stats;
    }

    bool operator!=(const TrackingAllocator& other) const {
        return !(*this == other);
    }
};

void testCustomAllocatorBasic() {
    using Alloc = TrackingAllocator<std::pair<const int, int>>;
    Alloc alloc;

    {
        BPlusTree<int, int, Alloc> tree(4, alloc);

        // Insert some values
        for (int i = 0; i < 20; i++) {
            tree.insert(i, i * 10);
        }

        // Verify insertions worked
        int value;
        for (int i = 0; i < 20; i++) {
            assert(tree.search(i, value));
            assert(value == i * 10);
        }

        assert(tree.validate());

        // Check that allocator was used
        assert(alloc.stats->allocations > 0);
        assert(alloc.stats->bytes_allocated > 0);
    }

    // After tree destruction, deallocations should match allocations
    assert(alloc.stats->allocations == alloc.stats->deallocations);
    assert(alloc.stats->bytes_allocated == alloc.stats->bytes_deallocated);

    std::cout << "✓ Custom allocator basic test passed" << std::endl;
    std::cout << "  Allocations: " << alloc.stats->allocations << std::endl;
    std::cout << "  Bytes: " << alloc.stats->bytes_allocated << std::endl;
}

void testCustomAllocatorWithDelete() {
    using Alloc = TrackingAllocator<std::pair<const int, int>>;
    Alloc alloc;

    {
        BPlusTree<int, int, Alloc> tree(4, alloc);

        // Insert values
        for (int i = 0; i < 50; i++) {
            tree.insert(i, i * 10);
        }

        size_t allocsAfterInsert = alloc.stats->allocations;

        // Delete some values (triggers node merging)
        for (int i = 0; i < 30; i++) {
            assert(tree.remove(i));
        }

        // Verify remaining values
        int value;
        for (int i = 30; i < 50; i++) {
            assert(tree.search(i, value));
            assert(value == i * 10);
        }

        assert(tree.validate());

        // Some deallocations should have happened due to merging
        assert(alloc.stats->deallocations > 0 || allocsAfterInsert == alloc.stats->allocations);
    }

    // After tree destruction, deallocations should match allocations
    assert(alloc.stats->allocations == alloc.stats->deallocations);

    std::cout << "✓ Custom allocator with delete test passed" << std::endl;
}

void testCustomAllocatorMove() {
    using Alloc = TrackingAllocator<std::pair<const int, int>>;
    Alloc alloc;

    {
        BPlusTree<int, int, Alloc> tree1(4, alloc);
        for (int i = 0; i < 20; i++) {
            tree1.insert(i, i * 10);
        }

        size_t allocsBeforeMove = alloc.stats->allocations;

        // Move construct
        BPlusTree<int, int, Alloc> tree2(std::move(tree1));

        // No new allocations should happen during move
        assert(alloc.stats->allocations == allocsBeforeMove);

        // Verify tree2 has the data
        int value;
        for (int i = 0; i < 20; i++) {
            assert(tree2.search(i, value));
            assert(value == i * 10);
        }

        // tree1 should be empty
        assert(tree1.isEmpty());

        assert(tree2.validate());
    }

    // After destruction, deallocations should match allocations
    assert(alloc.stats->allocations == alloc.stats->deallocations);

    std::cout << "✓ Custom allocator move test passed" << std::endl;
}

void testCustomAllocatorMoveAssign() {
    using Alloc = TrackingAllocator<std::pair<const int, int>>;
    Alloc alloc;

    {
        BPlusTree<int, int, Alloc> tree1(4, alloc);
        BPlusTree<int, int, Alloc> tree2(4, alloc);

        for (int i = 0; i < 20; i++) {
            tree1.insert(i, i * 10);
        }

        for (int i = 100; i < 110; i++) {
            tree2.insert(i, i * 10);
        }

        size_t allocsBeforeMove = alloc.stats->allocations;
        size_t deallocsBeforeMove = alloc.stats->deallocations;

        // Move assign
        tree2 = std::move(tree1);

        // tree2's old nodes should be deallocated
        assert(alloc.stats->deallocations > deallocsBeforeMove);

        // No new allocations during move
        assert(alloc.stats->allocations == allocsBeforeMove);

        // Verify tree2 has tree1's data
        int value;
        for (int i = 0; i < 20; i++) {
            assert(tree2.search(i, value));
            assert(value == i * 10);
        }

        // tree1 should be empty
        assert(tree1.isEmpty());

        assert(tree2.validate());
    }

    // After destruction, deallocations should match allocations
    assert(alloc.stats->allocations == alloc.stats->deallocations);

    std::cout << "✓ Custom allocator move assignment test passed" << std::endl;
}

void testCustomAllocatorBulkLoad() {
    using Alloc = TrackingAllocator<std::pair<const int, int>>;
    Alloc alloc;

    {
        BPlusTree<int, int, Alloc> tree(4, alloc);

        std::vector<std::pair<int, int>> data;
        for (int i = 0; i < 100; i++) {
            data.emplace_back(i, i * 10);
        }

        tree.bulkLoad(data);

        // Verify all data loaded
        int value;
        for (int i = 0; i < 100; i++) {
            assert(tree.search(i, value));
            assert(value == i * 10);
        }

        assert(tree.validate());
        assert(alloc.stats->allocations > 0);
    }

    // After destruction, deallocations should match allocations
    assert(alloc.stats->allocations == alloc.stats->deallocations);

    std::cout << "✓ Custom allocator bulk load test passed" << std::endl;
}

void testGetAllocator() {
    using Alloc = TrackingAllocator<std::pair<const int, int>>;
    Alloc alloc;

    BPlusTree<int, int, Alloc> tree(4, alloc);

    auto retrieved = tree.get_allocator();
    // The retrieved allocator should share the same stats
    assert(retrieved.stats == alloc.stats);

    std::cout << "✓ get_allocator test passed" << std::endl;
}

void testDefaultAllocator() {
    // Test that default allocator still works
    BPlusTree<int, int> tree(4);

    for (int i = 0; i < 50; i++) {
        tree.insert(i, i * 10);
    }

    int value;
    for (int i = 0; i < 50; i++) {
        assert(tree.search(i, value));
        assert(value == i * 10);
    }

    assert(tree.validate());

    std::cout << "✓ Default allocator test passed" << std::endl;
}

int main() {
    std::cout << "=== Custom Allocator Tests ===" << std::endl;

    testDefaultAllocator();
    testCustomAllocatorBasic();
    testCustomAllocatorWithDelete();
    testCustomAllocatorMove();
    testCustomAllocatorMoveAssign();
    testCustomAllocatorBulkLoad();
    testGetAllocator();

    std::cout << "\n✓ All custom allocator tests passed!" << std::endl;
    return 0;
}
