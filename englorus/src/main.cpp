#include <iostream>
#include <string>

#include "dictionary.hpp"

int main() {
    DictionaryTree dict;

    dict.AddWord("hello", "привет");
    dict.AddWord("world", "мир");
    dict.AddWord("cat", "кот");

    DictionaryNode* node = dict.GetWord("hello");
    if (node != nullptr) {
        std::cout << "hello: " << node->getContent() << "\n";
    }

    dict += std::make_pair("dog", "собака");

    dict["cat"] = "кошка";
    std::cout << "cat: " << dict["cat"] << "\n";

    DictionaryNode* dog = dict.GetWord("dog");
    if (dog != nullptr) {
        std::cout << "dog: " << dog->getContent() << "\n";
    } else {
        std::cout << "dog: not found\n";
    }

    dict -= "world";
    if (dict.GetWord("world") == nullptr) {
        std::cout << "world: deleted\n";
    }

    return 0;
}