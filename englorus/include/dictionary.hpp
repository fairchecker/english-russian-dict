#pragma once

#include <string>
#include <memory>

class DictionaryTree {
    public:
    void AddWord(std::string key, std::string content);
    void DeleteWord(const std::string& key);
    void GetWord(std::string key);

    private:
    std::unique_ptr<DictionaryNode> root_;
    std::size_t num_words_ = 0;

};