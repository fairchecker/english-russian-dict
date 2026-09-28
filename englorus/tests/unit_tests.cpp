/**
 * @file unit_tests.cpp
 * @brief Юнит-тесты словаря DictionaryTree на GoogleTest.
 *
 * Запуск:
 *   ctest --test-dir <build-dir> --output-on-failure
 * или напрямую: ./test_dictionary
 */

#include "dictionary.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <cstdio>
#include <fstream>
#include <random>
#include <string>
#include <utility>
#include <vector>

namespace {

/// Проверяет порядок ключей в поддереве (инвариант бинарного дерева поиска).
void ExpectBstInvariant(DictionaryNode* node) {
    if (node == nullptr) return;
    if (node->getLeft() != nullptr) {
        EXPECT_LT(node->getLeft()->getKey(), node->getKey());
        ExpectBstInvariant(node->getLeft());
    }
    if (node->getRight() != nullptr) {
        EXPECT_GT(node->getRight()->getKey(), node->getKey());
        ExpectBstInvariant(node->getRight());
    }
}

/// Создаёт временный файл словаря, возвращает его имя.
/// Имя уникально (и без путей), поэтому тесты безопасны при ctest -j
/// и одинаково работают на Windows и POSIX.
class TempDictionaryFile {
    public:
    explicit TempDictionaryFile(const std::string& contents)
        : path_("englorus_test_dictionary_" + std::to_string(NextId()) + ".txt") {
        std::ofstream out(path_);
        out << contents;
    }

    ~TempDictionaryFile() { std::remove(path_.c_str()); }

    TempDictionaryFile(const TempDictionaryFile&) = delete;
    TempDictionaryFile& operator=(const TempDictionaryFile&) = delete;

    const std::string& path() const { return path_; }

    private:
    static int NextId() {
        static int counter = 0;
        return counter++;
    }

    std::string path_;
};

}  // namespace

// ===========================================================================
// Пустое дерево
// ===========================================================================
TEST(EmptyTree, GetWordReturnsNull) {
    DictionaryTree dict;
    EXPECT_EQ(dict.GetWord("hello"), nullptr);
}

TEST(EmptyTree, DeleteWordDoesNotCrash) {
    DictionaryTree dict;
    dict.DeleteWord("hello");
    EXPECT_EQ(dict.GetWord("hello"), nullptr);
}

TEST(EmptyTree, MinusEqualsDoesNotCrash) {
    DictionaryTree dict;
    dict -= "hello";
    EXPECT_EQ(dict.GetWord("hello"), nullptr);
}

// ===========================================================================
// Добавление и поиск
// ===========================================================================
TEST(AddWord, SingleKeyIsFound) {
    DictionaryTree dict;
    dict.AddWord("dog", "собака");
    EXPECT_NE(dict.GetWord("dog"), nullptr);
    EXPECT_EQ("собака", dict.GetWord("dog")->getContent());
    EXPECT_EQ("dog", dict.GetWord("dog")->getKey());
}

TEST(AddWord, ManyKeysAreFound) {
    DictionaryTree dict;
    dict.AddWord("cat", "кошка");
    dict.AddWord("dog", "собака");
    dict.AddWord("apple", "яблоко");
    dict.AddWord("zebra", "зебра");

    for (const auto& key : {"cat", "dog", "apple", "zebra"}) {
        ASSERT_NE(dict.GetWord(key), nullptr) << key;
        ExpectBstInvariant(dict.GetWord(key));
    }
    EXPECT_EQ("яблоко", dict.GetWord("apple")->getContent());
}

TEST(AddWord, MissingKeyReturnsNull) {
    DictionaryTree dict;
    dict.AddWord("dog", "собака");
    EXPECT_EQ(dict.GetWord("horse"), nullptr);
    EXPECT_EQ(dict.GetWord("zzz"), nullptr);
}

TEST(AddWord, DuplicateKeyUpdatesContent) {
    DictionaryTree dict;
    dict.AddWord("dog", "собака");
    dict.AddWord("dog", "пёс");
    EXPECT_EQ("пёс", dict.GetWord("dog")->getContent());
    // Лишний узел не появился: после удаления ключа дерево снова пустое.
    dict.DeleteWord("dog");
    EXPECT_EQ(dict.GetWord("dog"), nullptr);
}

TEST(AddWord, KeysAreCaseSensitive) {
    DictionaryTree dict;
    dict.AddWord("Hello", "привет");
    dict.AddWord("hello", "привет строчными");
    EXPECT_EQ("привет", dict.GetWord("Hello")->getContent());
    EXPECT_EQ("привет строчными", dict.GetWord("hello")->getContent());
    EXPECT_EQ(dict.GetWord("HELLO"), nullptr);
}

// ===========================================================================
// Удаление
// ===========================================================================
TEST(DeleteWord, LeafIsRemoved) {
    DictionaryTree dict;
    dict.AddWord("dog", "собака");
    dict.AddWord("cat", "кошка");
    dict.DeleteWord("cat");
    EXPECT_EQ(dict.GetWord("cat"), nullptr);
    EXPECT_NE(dict.GetWord("dog"), nullptr);
}

TEST(DeleteWord, OtherKeysSurvive) {
    DictionaryTree dict;
    dict.AddWord("dog", "собака");
    dict.AddWord("cat", "кошка");
    dict.AddWord("apple", "яблоко");
    dict.DeleteWord("apple");
    EXPECT_NE(dict.GetWord("dog"), nullptr);
    EXPECT_NE(dict.GetWord("cat"), nullptr);
}

TEST(DeleteWord, NodeWithSingleLeftChild) {
    DictionaryTree dict;
    dict.AddWord("dog", "собака");  // корень
    dict.AddWord("cat", "кошка");   // левый потомок
    dict.DeleteWord("dog");
    EXPECT_EQ(dict.GetWord("dog"), nullptr);
    EXPECT_EQ("кошка", dict.GetWord("cat")->getContent());
}

TEST(DeleteWord, NodeWithSingleRightChild) {
    DictionaryTree dict;
    dict.AddWord("cat", "кошка");   // корень
    dict.AddWord("dog", "собака");  // правый потомок
    dict.DeleteWord("cat");
    EXPECT_EQ(dict.GetWord("cat"), nullptr);
    EXPECT_EQ("собака", dict.GetWord("dog")->getContent());
}

TEST(DeleteWord, NodeWithTwoChildrenReplacesWithLeftMax) {
    // "dog" — корень, у него есть оба потомка; максимум левого поддерева — "cat".
    DictionaryTree dict;
    dict.AddWord("dog", "собака");
    dict.AddWord("cat", "кошка");
    dict.AddWord("elephant", "слон");
    dict.DeleteWord("dog");
    EXPECT_EQ(dict.GetWord("dog"), nullptr);
    EXPECT_EQ("кошка", dict.GetWord("cat")->getContent());
    EXPECT_EQ("слон", dict.GetWord("elephant")->getContent());
}

TEST(DeleteWord, TwoChildrenCaseKeepsWholeSubtree) {
    // Удаляем корень с обоими потомками, где у левого поддерева есть
    // собственные оба потомка: проверяем обход максимума глубже одного уровня.
    DictionaryTree dict;
    dict.AddWord("m", "m");
    dict.AddWord("d", "d");
    dict.AddWord("f", "f");
    dict.AddWord("b", "b");
    dict.AddWord("e", "e");
    dict.AddWord("z", "z");

    dict.DeleteWord("m");
    EXPECT_EQ(dict.GetWord("m"), nullptr);
    for (const auto& key : {"d", "f", "b", "e", "z"}) {
        ASSERT_NE(dict.GetWord(key), nullptr) << key;
        EXPECT_EQ(key, dict.GetWord(key)->getKey());
        EXPECT_EQ(key, dict.GetWord(key)->getContent());
    }
    EXPECT_EQ(dict.GetWord("c"), nullptr);
}

TEST(DeleteWord, OnlyNodeInTree) {
    DictionaryTree dict;
    dict.AddWord("dog", "собака");
    dict.DeleteWord("dog");
    EXPECT_EQ(dict.GetWord("dog"), nullptr);
}

TEST(DeleteWord, MissingKeyKeepsTreeIntact) {
    DictionaryTree dict;
    dict.AddWord("dog", "собака");
    dict.AddWord("cat", "кошка");
    dict.DeleteWord("horse");
    EXPECT_NE(dict.GetWord("dog"), nullptr);
    EXPECT_NE(dict.GetWord("cat"), nullptr);
    EXPECT_EQ(dict.GetWord("horse"), nullptr);
}

TEST(DeleteWord, EverythingRemoved) {
    DictionaryTree dict;
    dict.AddWord("dog", "собака");
    dict.AddWord("cat", "кошка");
    dict.DeleteWord("dog");
    dict.DeleteWord("cat");
    EXPECT_EQ(dict.GetWord("dog"), nullptr);
    EXPECT_EQ(dict.GetWord("cat"), nullptr);
    // Дерево снова пустое: добавление работает как в самом начале.
    dict.AddWord("fox", "лиса");
    EXPECT_EQ("лиса", dict.GetWord("fox")->getContent());
}

// ===========================================================================
// Операторы
// ===========================================================================
TEST(Operators, SubscriptInsertsNewKey) {
    DictionaryTree dict;
    dict["newkey"] = "значение";
    EXPECT_EQ("значение", dict.GetWord("newkey")->getContent());
}

TEST(Operators, SubscriptReturnsReferenceToStoredValue) {
    DictionaryTree dict;
    dict.AddWord("dog", "собака");
    std::string& value = dict["dog"];
    value = "пёс";
    EXPECT_EQ("пёс", dict.GetWord("dog")->getContent());
}

TEST(Operators, SubscriptInPlaceModification) {
    DictionaryTree dict;
    dict["count"] = "1";
    dict["count"] += "!";
    EXPECT_EQ("1!", dict.GetWord("count")->getContent());
}

TEST(Operators, SubscriptOnMissingKeyCreatesEmptyValue) {
    DictionaryTree dict;
    std::string& value = dict["brand-new"];
    EXPECT_EQ("", value);
    value = "заполнено";
    EXPECT_EQ("заполнено", dict.GetWord("brand-new")->getContent());
}

TEST(Operators, PlusEqualsAddsPair) {
    DictionaryTree dict;
    dict += std::make_pair("dog", "собака");
    EXPECT_EQ("собака", dict.GetWord("dog")->getContent());
}

TEST(Operators, MinusEqualsRemovesWord) {
    DictionaryTree dict;
    dict.AddWord("dog", "собака");
    dict -= "dog";
    EXPECT_EQ(dict.GetWord("dog"), nullptr);
}

TEST(SetWord, UpdatesExistingKey) {
    DictionaryTree dict;
    dict.AddWord("dog", "собака");
    dict.SetWord("dog", "пёс");
    EXPECT_EQ("пёс", dict.GetWord("dog")->getContent());
}

TEST(SetWord, MissingKeyIsNotCreated) {
    DictionaryTree dict;
    dict.AddWord("dog", "собака");
    dict.SetWord("cat", "кошка");
    EXPECT_EQ(dict.GetWord("cat"), nullptr);
    EXPECT_EQ("собака", dict.GetWord("dog")->getContent());
}

TEST(SetWord, WorksOnEmptyTree) {
    DictionaryTree dict;
    dict.SetWord("dog", "собака");
    EXPECT_EQ(dict.GetWord("dog"), nullptr);
}

// ===========================================================================
// Копирование
// ===========================================================================
TEST(CopySemantics, CopyConstructorKeepsOriginalIntact) {
    DictionaryTree original;
    original.AddWord("dog", "собака");
    original.AddWord("cat", "кошка");

    DictionaryTree copy(original);
    copy.DeleteWord("dog");
    copy.AddWord("fox", "лиса");

    EXPECT_NE(original.GetWord("dog"), nullptr);
    EXPECT_NE(original.GetWord("cat"), nullptr);
    EXPECT_EQ(original.GetWord("fox"), nullptr);
}

TEST(CopySemantics, CopyConstructorDeepCopiesContent) {
    DictionaryTree original;
    original.AddWord("dog", "собака");

    DictionaryTree copy(original);
    copy["dog"] = "пёс";

    EXPECT_EQ("собака", original.GetWord("dog")->getContent());
    EXPECT_EQ("пёс", copy.GetWord("dog")->getContent());
}

TEST(CopySemantics, AssignmentOperatorDeepCopies) {
    DictionaryTree left;
    left.AddWord("dog", "собака");

    DictionaryTree right;
    right.AddWord("cat", "кошка");
    right = left;
    right.AddWord("fox", "лиса");

    EXPECT_EQ("собака", right.GetWord("dog")->getContent());
    EXPECT_EQ(right.GetWord("cat"), nullptr);
    EXPECT_EQ(left.GetWord("fox"), nullptr);
    EXPECT_NE(right.GetWord("fox"), nullptr);
}

TEST(CopySemantics, SelfAssignmentDoesNotCorruptTree) {
    DictionaryTree dict;
    dict.AddWord("dog", "собака");
    dict.AddWord("cat", "кошка");

    // Через псевдоним: компилятор не считает это самоприсваиванием
    // (иначе ругается -Wself-assign-overloaded), а проверить надо именно его.
    DictionaryTree& alias = dict;
    dict = alias;

    EXPECT_EQ("собака", dict.GetWord("dog")->getContent());
    EXPECT_EQ("кошка", dict.GetWord("cat")->getContent());
}

TEST(CopySemantics, AssigningEmptyTreeClearsTarget) {
    DictionaryTree source;
    DictionaryTree target;
    target.AddWord("dog", "собака");

    target = source;
    EXPECT_EQ(target.GetWord("dog"), nullptr);
    target.AddWord("fox", "лиса");
    EXPECT_EQ("лиса", target.GetWord("fox")->getContent());
    EXPECT_EQ(source.GetWord("fox"), nullptr);
}

// ===========================================================================
// Загрузка из файла
// ===========================================================================
TEST(FileLoading, ReadsKeyValuePairs) {
    TempDictionaryFile file("dog:собака\ncat:кошка\n");
    DictionaryTree dict(file.path());

    EXPECT_EQ("собака", dict.GetWord("dog")->getContent());
    EXPECT_EQ("кошка", dict.GetWord("cat")->getContent());
    EXPECT_EQ(dict.GetWord("horse"), nullptr);
}

TEST(FileLoading, SkipsLinesWithoutSeparator) {
    TempDictionaryFile file("dog:собака\nпросто строка без двоеточия\ncat:кошка\n");
    DictionaryTree dict(file.path());

    EXPECT_EQ("собака", dict.GetWord("dog")->getContent());
    EXPECT_EQ("кошка", dict.GetWord("cat")->getContent());
    EXPECT_EQ(dict.GetWord("просто строка без двоеточия"), nullptr);
}

TEST(FileLoading, ValueMayContainColons) {
    TempDictionaryFile file("time:12:30:00\n");
    DictionaryTree dict(file.path());
    EXPECT_EQ("12:30:00", dict.GetWord("time")->getContent());
}

TEST(FileLoading, MissingFileGivesEmptyDictionary) {
    DictionaryTree dict("englorus_no_such_file.txt");
    EXPECT_EQ(dict.GetWord("dog"), nullptr);
    dict.AddWord("dog", "собака");
    EXPECT_EQ("собака", dict.GetWord("dog")->getContent());
}

// ===========================================================================
// Нагрузочный тест
// ===========================================================================
TEST(Stress, RandomInsertKeepsAllWordsAndInvariant) {
    DictionaryTree dict;
    std::mt19937 rng(12345);
    std::vector<std::string> keys;
    for (int i = 0; i < 100; ++i) {
        keys.push_back("k" + std::to_string(i));
    }
    std::shuffle(keys.begin(), keys.end(), rng);
    for (const auto& key : keys) {
        dict.AddWord(key, "v" + key);
    }

    int found = 0;
    for (const auto& key : keys) {
        DictionaryNode* node = dict.GetWord(key);
        ASSERT_NE(node, nullptr) << key;
        EXPECT_EQ("v" + key, node->getContent());
        ++found;
    }
    EXPECT_EQ(static_cast<int>(keys.size()), found);
    EXPECT_EQ(dict.GetWord("missing"), nullptr);

    // Повторное добавление тех же ключей не ломает структуру.
    for (const auto& key : keys) {
        dict.AddWord(key, "w" + key);
    }
    for (const auto& key : keys) {
        ASSERT_NE(dict.GetWord(key), nullptr) << key;
        EXPECT_EQ("w" + key, dict.GetWord(key)->getContent());
        ExpectBstInvariant(dict.GetWord(key));
    }
}

TEST(Stress, InterleavedInsertAndDelete) {
    DictionaryTree dict;
    for (int i = 0; i < 200; ++i) {
        const std::string key = "k" + std::to_string(i);
        dict.AddWord(key, "v" + key);
    }
    for (int i = 0; i < 200; i += 2) {
        dict.DeleteWord("k" + std::to_string(i));
    }
    for (int i = 0; i < 200; ++i) {
        const std::string key = "k" + std::to_string(i);
        DictionaryNode* node = dict.GetWord(key);
        if (i % 2 == 0) {
            EXPECT_EQ(node, nullptr) << key;
        } else {
            ASSERT_NE(node, nullptr) << key;
            EXPECT_EQ(key, node->getKey());
            EXPECT_EQ("v" + key, node->getContent());
        }
        ExpectBstInvariant(node);
    }
}
