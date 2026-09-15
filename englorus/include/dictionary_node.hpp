#pragma once

#include <string>
#include <memory>
#include <utility>

class DictionaryNode{
    public:
    DictionaryNode(std::string key, std::string content);

    const std::string& getContent() const { return content_; }
    const std::string& getKey() const { return key_; }
    std::string& getContentChangeable() { return content_; }

    DictionaryNode* getRight() const { return right_.get(); }
    DictionaryNode* getLeft() const { return left_.get(); }

    std::unique_ptr<DictionaryNode>& getRightPtr() { return right_; }
    std::unique_ptr<DictionaryNode>& getLeftPtr() { return left_; }

    void setRight(std::unique_ptr<DictionaryNode> node) { right_ = std::move(node); }
    void setLeft(std::unique_ptr<DictionaryNode> node) { left_ = std::move(node); }
    void setContent(std::string content) { content_ = std::move(content); }
    void setKey(std::string key) { key_ = std::move(key); }

    private:
    std::string key_;
    std::string content_; 
    std::unique_ptr<DictionaryNode> right_;
    std::unique_ptr<DictionaryNode> left_;
};