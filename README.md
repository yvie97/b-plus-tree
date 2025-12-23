# B+ Tree Implementation

An educational in-memory B+ tree implementation in C++ with a focus on clarity and correctness.

## Features

- Generic template-based implementation supporting custom key/value types
- Efficient search, insertion, and deletion operations (O(log n))
- Range query support with linked leaf nodes
- Comprehensive validation and testing
- Clean, well-documented code for learning

## Building

```bash
mkdir build
cd build
cmake ..
make
```

## Running Tests

```bash
cd build
ctest --verbose
```

## Running Demo

```bash
cd build
./demo
```

## Usage Example

```cpp
#include "BPlusTree.h"

int main() {
    BPlusTree<int, std::string> tree(4);  // Order 4

    tree.insert(10, "value1");
    tree.insert(20, "value2");
    tree.insert(5, "value3");

    std::string value;
    if (tree.search(10, value)) {
        std::cout << "Found: " << value << std::endl;
    }

    auto results = tree.rangeQuery(5, 20);
    for (const auto& [key, val] : results) {
        std::cout << key << ": " << val << std::endl;
    }

    return 0;
}
```

## Design

See [DESIGN.md](DESIGN.md) for detailed design documentation.

## Project Structure

- `include/` - Header files
- `tests/` - Unit tests
- `examples/` - Example programs
- `DESIGN.md` - Design documentation
