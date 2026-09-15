#include <iostream>
#include <string>

#include "include/dictionary.hpp"

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

    std::cout << "dog: " << dict.GetWord("dog")->getContent() << "\n";

    dict -= "world";
    if (dict.GetWord("world") == nullptr) {
        std::cout << "world: deleted\n";
    }

    return 0;
}