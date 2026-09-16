#include "dictionary.hpp"
#include <fstream>

DictionaryTree::DictionaryTree() = default; 

DictionaryTree::DictionaryTree(std::string filepath){
    std::ifstream in;
    in.open(filepath);
    std::string line, key, content;
    size_t pos;
    if(in.is_open()){
        while (std::getline(in, line))
        {
            pos = line.find(":");
            if (pos == std::string::npos) continue;
            key = line.substr(0, pos);
            content = line.substr(pos + 1);
            this->AddWord(key, content);
        }
    }
    in.close();
}

namespace {
std::unique_ptr<DictionaryNode> copySubtree(const DictionaryNode* node) {
    if (node == nullptr) return nullptr;
    auto copy = std::make_unique<DictionaryNode>(node->getKey(), node->getContent());
    copy->setLeft(copySubtree(node->getLeft()));
    copy->setRight(copySubtree(node->getRight()));
    return copy;
}
}  // namespace

DictionaryTree::DictionaryTree(const DictionaryTree& other)
    : root_(copySubtree(other.root_.get())),
      num_words_(other.num_words_) {}

DictionaryTree& DictionaryTree::operator=(const DictionaryTree& other) {
    if (this != &other) {
        root_ = copySubtree(other.root_.get());
        num_words_ = other.num_words_;
    }
    return *this;
}

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

void DictionaryTree::SetWord(const std::string& key, const std::string& new_content){
    DictionaryNode* cur = root_.get();
    while(true){
        if(cur == nullptr) return;
        if(cur->getKey() < key){
            cur = cur->getRight();
        }
        else if(cur->getKey() > key){
            cur = cur->getLeft();
        }
        else if(cur->getKey() == key){
            cur->setContent(new_content);
            return;
        }
        else return;
    }
}

void DictionaryTree::operator+=(const std::pair<std::string, std::string>& pair){
    AddWord(pair.first, pair.second);
}

void DictionaryTree::operator-=(const std::string& key){
    DeleteWord(key);
}

std::string& DictionaryTree::operator[](const std::string& key){
    DictionaryNode* node = GetWord(key);
    if (node == nullptr) {
        AddWord(key, "");
        node = GetWord(key);
    }
    return node->getContentChangeable();
}