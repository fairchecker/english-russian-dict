#pragma once

using <string>;

class DictionaryTree {
    public:
    void AddWord(std::string key, std::string content);
    void DeleteWord(std::string key);
    void GetWord(std::string key);

    private:
    DictionaryNode root_;
    int num_words_;

}