#include "include/dictionary.hpp"

void DictionaryTree::AddWord(std::string key, std::string content){
    if (root_ == nullptr) {
    root_ = new DictionaryNode(key, content);
    return;
    }
    DictionaryNode* cur = root_;
    while(true){
        if(cur->getKey > key){
            if(cur->getLeft){
                cur = cur->getLeft;
            }
            else {
                cur->setLeft(new DictionaryNode(key, content));
                return;
            }
        }
        else if(cur->getKey < key){
            if(cur->getRight){
                cur = cur->getRight;
            }
            else {
                cur->setRight(new DictionaryNode(key, content));
                return;
            }
        }
        else {
            cur->setContent(content);
            return;
        }
    }
}