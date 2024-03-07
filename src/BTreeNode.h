#ifndef BTREENODE_H
#define BTREENODE_H

#include <cstdint>
#include <vector>
#include <cstring>
#include <iostream>

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
const uint32_t LEAF_NODE_NEXT_LEAF_SIZE = sizeof(uint32_t);
const uint32_t LEAF_NODE_NEXT_LEAF_OFFSET = LEAF_NODE_NUM_CELLS_OFFSET + LEAF_NODE_NUM_CELLS_SIZE;
const uint32_t LEAF_NODE_HEADER_SIZE = COMMON_NODE_HEADER_SIZE +
                                       LEAF_NODE_NUM_CELLS_SIZE +
                                       LEAF_NODE_NEXT_LEAF_SIZE;
//Leaf Node Body
const uint32_t LEAF_NODE_KEY_SIZE = sizeof(uint32_t);
const uint32_t LEAF_NODE_KEY_OFFSET = 0;
const uint32_t LEAF_NODE_VALUE_SIZE = sizeof(Row);
const uint32_t LEAF_NODE_VALUE_OFFSET = LEAF_NODE_KEY_OFFSET + LEAF_NODE_KEY_SIZE;
const uint32_t LEAF_NODE_CELL_SIZE = LEAF_NODE_KEY_SIZE + LEAF_NODE_VALUE_SIZE;
const uint32_t LEAF_NODE_SPACE_FOR_CELLS = PAGE_SIZE - LEAF_NODE_HEADER_SIZE;
const uint32_t LEAF_NODE_MAX_CELLS = LEAF_NODE_SPACE_FOR_CELLS / LEAF_NODE_CELL_SIZE;

//Splitting
const uint32_t LEAF_NODE_RIGHT_SPLIT_COUNT = (LEAF_NODE_MAX_CELLS + 1) / 2;
const uint32_t LEAF_NODE_LEFT_SPLIT_COUNT = (LEAF_NODE_MAX_CELLS + 1) - LEAF_NODE_RIGHT_SPLIT_COUNT;

//Internal Node Header
const uint32_t INTERNAL_NODE_NUM_KEYS_SIZE = sizeof(uint32_t);
const uint32_t INTERNAL_NODE_NUM_KEYS_OFFSET = COMMON_NODE_HEADER_SIZE;
const uint32_t INTERNAL_NODE_RIGHT_CHILD_SIZE = sizeof(uint32_t);
const uint32_t INTERNAL_NODE_RIGHT_CHILD_OFFSET = INTERNAL_NODE_NUM_KEYS_OFFSET + INTERNAL_NODE_NUM_KEYS_SIZE;
const uint32_t INTERNAL_NODE_HEADER_SIZE = COMMON_NODE_HEADER_SIZE +
                                           INTERNAL_NODE_NUM_KEYS_SIZE +
                                           INTERNAL_NODE_RIGHT_CHILD_SIZE;

//Internal Node Body
const uint32_t INTERNAL_NODE_KEY_SIZE = sizeof(uint32_t);
const uint32_t INTERNAL_NODE_CHILD_SIZE = sizeof(uint32_t);
const uint32_t INTERNAL_NODE_CELL_SIZE = INTERNAL_NODE_CHILD_SIZE + INTERNAL_NODE_KEY_SIZE;
const uint32_t INTERNAL_NODE_MAX_CELLS = 3;

enum class NodeType {
    Internal,
    Leaf
};

class BTreeNode {
public:
    BTreeNode(void *node): node(node) {}

    NodeType get_node_type() const {
        uint8_t value = *static_cast<uint8_t*>(node + NODE_TYPE_OFFSET);
        return static_cast<NodeType>(value);
    }

    void set_node_type(NodeType nodeType){
        uint8_t value = static_cast<uint8_t>(nodeType);
        *static_cast<uint8_t*>(node + NODE_TYPE_OFFSET) = value;
    }

    bool is_root() const {
        uint8_t value = *static_cast<uint8_t*>(node + IS_ROOT_OFFSET);
        return static_cast<bool>(value);
    }

    void set_root(bool isRoot){
        uint8_t value = static_cast<uint8_t>(isRoot);
        *static_cast<uint8_t*>(node + IS_ROOT_OFFSET) = value;
    }

    uint32_t *node_parent(){
        return static_cast<uint32_t*>(node + PARENT_POINTER_OFFSET);
    }

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

    uint32_t *leaf_node_next_leaf(){
        return static_cast<uint32_t*>(node + LEAF_NODE_NEXT_LEAF_OFFSET);
    }

    void initialize_leaf_node(){
        set_node_type(NodeType::Leaf);
        set_root(false);
        *leaf_node_num_cells() = 0;
        *leaf_node_next_leaf() = 0; //no sibling
    }

    void initialize_internal_node(){
        set_node_type(NodeType::Internal);
        set_root(false);
        *internal_node_num_keys() = 0;
    }

    uint32_t *internal_node_num_keys(){
        return static_cast<uint32_t*>(node + INTERNAL_NODE_NUM_KEYS_OFFSET);
    }

    uint32_t *internal_node_right_child(){
        return static_cast<uint32_t*>(node + INTERNAL_NODE_RIGHT_CHILD_OFFSET);
    }

    uint32_t *internal_node_cell(uint32_t cell_num){
        return static_cast<uint32_t*>(node + INTERNAL_NODE_HEADER_SIZE + cell_num * INTERNAL_NODE_CELL_SIZE);
    }

    uint32_t *internal_node_child(uint32_t child_num){
        uint32_t num_keys = *internal_node_num_keys();
        if (child_num > num_keys){
            std::cerr << "Tried to access child_num " << child_num << " > num_keys " << num_keys << std::endl;
            exit(EXIT_FAILURE);
        }
        else if (child_num == num_keys){
            return internal_node_right_child();
        }
        else {
            return internal_node_cell(child_num);
        }
    }

    uint32_t *internal_node_key(uint32_t key_num){
        return static_cast<uint32_t*>(((void*)internal_node_cell(key_num) + INTERNAL_NODE_CHILD_SIZE));
    }

    void update_internal_node_key(uint32_t old_key, uint32_t new_key){
        uint32_t old_child_index = internal_node_find_child(old_key);
        *internal_node_key(old_child_index) = new_key;
    }

    uint32_t internal_node_find_child(uint32_t key) {
        size_t num_keys = *(this->internal_node_num_keys());

        uint32_t min_index = 0;
        uint32_t max_index = num_keys;

        while (min_index != max_index){
            uint32_t index = (min_index + max_index) / 2;
            uint32_t key_to_right = *(this->internal_node_key(index));
            if (key_to_right >= key){
                max_index = index;
            }
            else {
                min_index = index + 1;
            }
        }

        return min_index;
    }

    uint32_t get_max_key(){
        switch (get_node_type()){
            case NodeType::Internal:
                return *internal_node_key(*internal_node_num_keys() - 1);
            case NodeType::Leaf:
                return *leaf_node_key(*leaf_node_num_cells() - 1);
        }

        return -1;
    }

    void print_leaf_node(){
        uint32_t num_cells = *leaf_node_num_cells();
        std::cout << "leaf (size " << num_cells << "):\n";
        for (uint32_t i = 0; i < num_cells; i++){
            uint32_t key = *leaf_node_key(i);
            std::cout << "  - " << i << " : " << key << std::endl;
        }
    }


private:
    void *node;
};

#endif
