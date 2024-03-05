#ifndef BTREENODE_H
#define BTREENODE_H

#include <cstdint>
#include <vector>
#include <cstring>

#include "Pager.h"
#include "Row.h"

//Node Header
const uint32_t NODE_TYPE_SIZE = sizeof(uint8_t);
const uint32_t NODE_TYPE_OFFSET = 0;
const uint32_t IS_ROOT_SIZE = sizeof(uint8_t);
const uint32_t IS_ROOT_OFFSET = NODE_TYPE_SIZE;
const uint32_t PARENT_POINTER_SIZE = sizeof(uint32_t);
const uint32_t PARENT_POINTER_OFFSET = IS_ROOT_OFFSET + IS_ROOT_SIZE;
const uint8_t COMMON_NODE_HEADER_SIZE = NODE_TYPE_SIZE + IS_ROOT_SIZE + PARENT_POINTER_SIZE;

//Leaf Node Header
const uint32_t LEAF_NODE_NUM_CELLS_SIZE = sizeof(uint32_t);
const uint32_t LEAF_NODE_NUM_CELLS_OFFSET = COMMON_NODE_HEADER_SIZE;
const uint32_t LEAF_NODE_HEADER_SIZE = COMMON_NODE_HEADER_SIZE + LEAF_NODE_NUM_CELLS_SIZE;

//Leaf Node Body
const uint32_t LEAF_NODE_KEY_SIZE = sizeof(uint32_t);
const uint32_t LEAF_NODE_KEY_OFFSET = 0;
const uint32_t LEAF_NODE_VALUE_SIZE = sizeof(Row);
const uint32_t LEAF_NODE_VALUE_OFFSET = LEAF_NODE_KEY_OFFSET + LEAF_NODE_KEY_SIZE;
const uint32_t LEAF_NODE_CELL_SIZE = LEAF_NODE_KEY_SIZE + LEAF_NODE_VALUE_SIZE;
const uint32_t LEAF_NODE_SPACE_FOR_CELLS = PAGE_SIZE - LEAF_NODE_HEADER_SIZE;
const uint32_t LEAF_NODE_MAX_CELLS = LEAF_NODE_SPACE_FOR_CELLS / LEAF_NODE_CELL_SIZE;

enum class NodeType {
    Internal,
    Leaf
};

class BTreeNode {
public:
    BTreeNode(void *node): node(node) {}

    // NodeType getNodeType() const { return nodeType; }
    // bool getIsRoot() const { return isRoot; }
    // void setIsRoot(bool value) { isRoot = value; }

    uint32_t *leaf_node_num_cells(){
        return static_cast<uint32_t*>(node + LEAF_NODE_NUM_CELLS_OFFSET);
    }

    void* leaf_node_cell(uint32_t cell_num){
        return node + LEAF_NODE_HEADER_SIZE + cell_num * LEAF_NODE_CELL_SIZE;
    }

    uint32_t *leaf_node_key(uint32_t cell_num){
        return static_cast<uint32_t*>(leaf_node_cell(cell_num));
    }

    void* leaf_node_value(uint32_t cell_num){
        return leaf_node_cell(cell_num) + LEAF_NODE_KEY_SIZE;
    }

    void initialize_leaf_node(){
        *leaf_node_num_cells() = 0;
    }
    

private:
    void *node;
    // NodeType nodeType;
    // bool isRoot;
};

#endif
