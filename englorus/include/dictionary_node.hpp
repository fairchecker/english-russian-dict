#pragma once

using <string>;

class DictionaryNode{
    public:
    DictionaryNode(std::string key, std::string content);

    std::string getContent { return content_; }
    std::string getKey { return key_; }
    DictionaryNode getRight { return right_; }
    DictionaryNode getLeft { return left_; }

    private:
    std::string key_;
    std::string content_; 
    DictionaryNode* right_;
    DictionaryNode* left_;
}