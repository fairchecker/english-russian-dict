#include "dictionary_node.hpp"

/**
 * @file dictionary_node.cpp
 * @brief Реализация класса DictionaryNode.
 */

/**
 * @brief Конструктор узла.
 * @param key слово-ключ.
 * @param content значение (перевод), связанное с ключом.
 */
DictionaryNode::DictionaryNode(std::string key, std::string content){
    key_ = std::move(key);
    content_ = std::move(content);
}