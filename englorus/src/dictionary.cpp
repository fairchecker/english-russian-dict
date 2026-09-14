#include "include/dictionary.hpp"
#include "dictionary_node.hpp"

void DictionaryTree::AddWord(std::string key, std::string content){
    if (root_ == nullptr) {
    root_ = std::make_unique<DictionaryNode>(std::move(key), std::move(content);
    ++num_words_;
    return;
    }
    DictionaryNode* cur = root_.get();
    while(true){
        if(cur->getKey() > key){
            if(cur->getLeft()){
                cur = cur->getLeft();
            }
            else {
                cur->setLeft(std::make_unique<DictionaryNode>(std::move(key), std::move(content)));
                num_words_++;
                return;
            }
        }
        else if(cur->getKey() < key){
            if(cur->getRight()){
                cur = cur->getRight();
            }
            else {
                cur->setRight(std::make_unique<DictionaryNode>(std::move(key), std::move(content)));
                num_words_++;
                return;
            }
        }
        else {
            cur->setContent(std::move(content));
            return;
        }
    }
}

void DictionaryTree::DeleteWord(const std::string& key){
    if(root_ == nullptr){
        return;
    }
    DictionaryNode* cur = root_.get();
    
    while(true){
        if(cur == nullptr) return;
        if(cur->getKey() > key){
            cur = cur->getLeft();
        }
        else if(cur->getKey() < key){
            cur = cur->getRight();
        }
        else break;
    }
    if(cur->getLeft() == nullptr && cur->getRight() == nullptr){

    }
}