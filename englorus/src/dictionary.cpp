#include "include/dictionary.hpp"
#include "dictionary_node.hpp"

void DictionaryTree::AddWord(std::string key, std::string content){
    if (root_ == nullptr) {
    root_ = std::make_unique<DictionaryNode>(std::move(key), std::move(content));
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
    std::unique_ptr<DictionaryNode>* cur = &root_;
    
    while(true){
        if((*cur) == nullptr) return;
        if((*cur)->getKey() > key){
            cur = &((*cur)->getLeftPtr());
        }
        else if((*cur)->getKey() < key){
            cur = &((*cur)->getRightPtr());
        }
        else break;
    }
    if((*cur)->getLeft() == nullptr && (*cur)->getRight() == nullptr){
        (*cur).reset();
        num_words_--;
        return;
    }
    if((*cur)->getLeft() && (*cur)->getRight()){
        std::unique_ptr<DictionaryNode>* add = cur;
        cur = &((*cur)->getLeftPtr());
        while((*cur)->getRight()){
            cur = &((*cur)->getRightPtr());
        }
        (*add)->setKey((*cur)->getKey());
        (*add)->setContent((*cur)->getContent());
        *cur = std::move((*cur)->getLeftPtr());
        num_words_--;
        return;
    }
    if((*cur)->getLeft()){
        *cur = std::move((*cur)->getLeftPtr());
        num_words_--;
        return;
    }
    else {
        *cur = std::move((*cur)->getRightPtr());
        num_words_--;
        return;
    }
}

DictionaryNode* DictionaryTree::GetWord(const std::string& key){
    DictionaryNode* cur = root_.get();
    while(true){
        if(cur == nullptr) return nullptr;
        if(cur->getKey() < key){
            cur = cur->getRight();
        }
        else if(cur->getKey() > key){
            cur = cur->getLeft();
        }
        else if(cur->getKey() == key){
            return cur;
        }
        else return nullptr;
    }
}