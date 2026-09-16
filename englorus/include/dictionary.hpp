#pragma once

#include <string>
#include <memory>
#include <utility>

#include "dictionary_node.hpp"

class DictionaryTree {
    public:
    DictionaryTree(std::string filepath);
    DictionaryTree();

    DictionaryTree(const DictionaryTree& other);
    DictionaryTree& operator=(const DictionaryTree& other);

    void AddWord(std::string key, std::string content);
    void DeleteWord(const std::string& key);
    DictionaryNode* GetWord(const std::string& key);
    void SetWord(const std::string& key, const std::string& new_content);

    void operator+=(const std::pair<std::string, std::string>& pair);
    void operator-=(const std::string& key);
    std::string& operator[](const std::string& key);

    private:
    std::unique_ptr<DictionaryNode> root_;
    std::size_t num_words_ = 0;

};