#include "dictionary_node.hpp"

DictionaryNode::DictionaryNode(std::string key, std::string content){
    key_ = std::move(key);
    content_ = std::move(content);
}