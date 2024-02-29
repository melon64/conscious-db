#include <cstdint>
#include <vector>
#include <cstring>

#include "Pager.h"
#include "Row.h"

enum class NodeType {
    Internal,
    Leaf
};

class BTreeNode {
public:
    BTreeNode(NodeType type, bool isRoot = false): nodeType(type), isRoot(isRoot), parentPointer(0), numCells(0) {}

    NodeType getNodeType() const { return nodeType; }
    bool getIsRoot() const { return isRoot; }
    void setIsRoot(bool value) { isRoot = value; }
    size_t getParentPointer() const { return parentPointer; }
    void setParentPointer(size_t pointer) { parentPointer = pointer; }
    size_t getNumCells() const { return numCells; }

    void initializeLeafNode() {
        numCells = 0;
    }

    size_t* leafNodeKey(size_t cellNum) {
        char* cellPtr = reinterpret_cast<char*>(leafNodeCell(cellNum));
        return reinterpret_cast<size_t*>(cellPtr);
    }

    char* leafNodeValue(size_t cellNum) {
        char* cellPtr = reinterpret_cast<char*>(leafNodeCell(cellNum));
        return cellPtr + LEAF_NODE_KEY_SIZE;
    }

    void insertLeafNode(size_t key, const char* value) {
        // Simple insertion logic for demonstration
        // In a real implementation, you would need to find the correct position
        // and possibly split the leaf node if it's full
        if (numCells >= LEAF_NODE_MAX_CELLS) {
            // Node full, handle splitting or return error
            return;
        }
        char* cellPtr = reinterpret_cast<char*>(leafNodeCell(numCells));
        std::memcpy(cellPtr, &key, sizeof(key));
        std::memcpy(cellPtr + LEAF_NODE_KEY_SIZE, value, sizeof(Row));
        numCells++;
    }

private:
    NodeType nodeType;
    bool isRoot;
    size_t parentPointer;
    size_t numCells; // For leaf nodes

    static constexpr size_t LEAF_NODE_KEY_SIZE = sizeof(size_t);
    static constexpr size_t LEAF_NODE_VALUE_SIZE = sizeof(Row);
    static constexpr size_t LEAF_NODE_CELL_SIZE = LEAF_NODE_KEY_SIZE + LEAF_NODE_VALUE_SIZE;
    static constexpr size_t LEAF_NODE_HEADER_SIZE = sizeof(nodeType) + sizeof(isRoot) + sizeof(parentPointer) + sizeof(numCells);
    static constexpr size_t LEAF_NODE_SPACE_FOR_CELLS = PAGE_SIZE - LEAF_NODE_HEADER_SIZE;
    static constexpr size_t LEAF_NODE_MAX_CELLS = LEAF_NODE_SPACE_FOR_CELLS / LEAF_NODE_CELL_SIZE;

    // Memory layout simulation
    char data[PAGE_SIZE]; // Represents the node's data in memory

    void* leafNodeCell(size_t cellNum) {
        return data + LEAF_NODE_HEADER_SIZE + cellNum * LEAF_NODE_CELL_SIZE;
    }
};
