#include "UnitTest++.h"

#include "dictionary.hpp"

#include <algorithm>
#include <random>
#include <string>
#include <utility>
#include <vector>

namespace {

void CheckInvariant(DictionaryNode* node) {
    if (node == nullptr) return;
    if (node->getLeft()) {
        CHECK(node->getLeft()->getKey() < node->getKey());
        CheckInvariant(node->getLeft());
    }
    if (node->getRight()) {
        CHECK(node->getRight()->getKey() > node->getKey());
        CheckInvariant(node->getRight());
    }
}

}  // namespace

TEST(EmptyTree_GetWord_ReturnsNull) {
    DictionaryTree dict;
    CHECK(dict.GetWord("hello") == nullptr);
}

TEST(EmptyTree_DeleteWord_DoesNotCrash) {
    DictionaryTree dict;
    dict.DeleteWord("hello");
    CHECK(dict.GetWord("hello") == nullptr);
}

TEST(EmptyTree_OperatorMinus_DoesNotCrash) {
    DictionaryTree dict;
    dict -= "hello";
    CHECK(dict.GetWord("hello") == nullptr);
}

TEST(AddSingle_GetWord_FindsKey) {
    DictionaryTree dict;
    dict.AddWord("dog", "собака");
    CHECK(dict.GetWord("dog") != nullptr);
}

TEST(AddSingle_GetWord_ReturnsContent) {
    DictionaryTree dict;
    dict.AddWord("dog", "собака");
    CHECK(dict.GetWord("dog")->getContent() == "собака");
}

TEST(AddSingle_RootKey) {
    DictionaryTree dict;
    dict.AddWord("dog", "собака");
    CHECK(dict.GetWord("dog")->getKey() == "dog");
}

TEST(GetWord_MissingKey_ReturnsNull) {
    DictionaryTree dict;
    dict.AddWord("dog", "собака");
    CHECK(dict.GetWord("horse") == nullptr);
}

TEST(GetWord_KeyNeverInserted_ReturnsNull) {
    DictionaryTree dict;
    dict.AddWord("cat", "кошка");
    CHECK(dict.GetWord("zzz") == nullptr);
}

TEST(AddMany_GetWord_FindsAll) {
    DictionaryTree dict;
    dict.AddWord("cat", "кошка");
    dict.AddWord("dog", "собака");
    dict.AddWord("apple", "яблоко");
    dict.AddWord("zebra", "зебра");
    CHECK(dict.GetWord("cat") != nullptr);
    CHECK(dict.GetWord("dog") != nullptr);
    CHECK(dict.GetWord("apple") != nullptr);
    CHECK(dict.GetWord("zebra") != nullptr);
}

TEST(DuplicateKey_UpdatesContent) {
    DictionaryTree dict;
    dict.AddWord("dog", "собака");
    dict.AddWord("dog", "пёс");
    CHECK(dict.GetWord("dog")->getContent() == "пёс");
}

TEST(DuplicateKey_DoesNotAddExtraNode) {
    DictionaryTree dict;
    dict.AddWord("dog", "собака");
    dict.AddWord("dog", "пёс");
    dict.DeleteWord("dog");
    CHECK(dict.GetWord("dog") == nullptr);
}

TEST(CaseSensitiveKeys_AreDistinct) {
    DictionaryTree dict;
    dict.AddWord("Hello", "привет");
    dict.AddWord("hello", "привет(низ)");
    CHECK(dict.GetWord("Hello") != nullptr);
    CHECK(dict.GetWord("hello") != nullptr);
    CHECK(dict.GetWord("HELLO") == nullptr);
}

TEST(DeleteLeaf_RemovesWord) {
    DictionaryTree dict;
    dict.AddWord("dog", "собака");
    dict.AddWord("cat", "кошка");
    dict.DeleteWord("cat");
    CHECK(dict.GetWord("cat") == nullptr);
}

TEST(DeleteLeaf_KeepsOtherWords) {
    DictionaryTree dict;
    dict.AddWord("dog", "собака");
    dict.AddWord("cat", "кошка");
    dict.AddWord("apple", "яблоко");
    dict.DeleteWord("apple");
    CHECK(dict.GetWord("dog") != nullptr);
    CHECK(dict.GetWord("cat") != nullptr);
}

TEST(Delete_NodeWithOneLeftChild) {
    DictionaryTree dict;
    dict.AddWord("dog", "собака");
    dict.AddWord("cat", "кошка");
    dict.DeleteWord("dog");
    CHECK(dict.GetWord("dog") == nullptr);
    CHECK(dict.GetWord("cat") != nullptr);
}

TEST(Delete_NodeWithOneRightChild) {
    DictionaryTree dict;
    dict.AddWord("cat", "кошка");
    dict.AddWord("dog", "собака");
    dict.DeleteWord("cat");
    CHECK(dict.GetWord("cat") == nullptr);
    CHECK(dict.GetWord("dog") != nullptr);
}

TEST(Delete_NodeWithTwoChildren) {
    DictionaryTree dict;
    dict.AddWord("dog", "собака");
    dict.AddWord("cat", "кошка");
    dict.AddWord("elephant", "слон");
    dict.DeleteWord("dog");
    CHECK(dict.GetWord("dog") == nullptr);
    CHECK(dict.GetWord("cat") != nullptr);
    CHECK(dict.GetWord("elephant") != nullptr);
}

TEST(Delete_RootOnlyNode) {
    DictionaryTree dict;
    dict.AddWord("dog", "собака");
    dict.DeleteWord("dog");
    CHECK(dict.GetWord("dog") == nullptr);
}

TEST(Delete_MissingKey_TreeIntact) {
    DictionaryTree dict;
    dict.AddWord("dog", "собака");
    dict.AddWord("cat", "кошка");
    dict.DeleteWord("horse");
    CHECK(dict.GetWord("dog") != nullptr);
    CHECK(dict.GetWord("cat") != nullptr);
    CHECK(dict.GetWord("horse") == nullptr);
}

TEST(DeleteAll_ThenGetWordNull) {
    DictionaryTree dict;
    dict.AddWord("dog", "собака");
    dict.AddWord("cat", "кошка");
    dict.DeleteWord("dog");
    dict.DeleteWord("cat");
    CHECK(dict.GetWord("dog") == nullptr);
    CHECK(dict.GetWord("cat") == nullptr);
}

TEST(OperatorSubscript_InsertsNewKey) {
    DictionaryTree dict;
    dict["newkey"] = "значение";
    CHECK(dict.GetWord("newkey") != nullptr);
}

TEST(OperatorSubscript_ModifiesExistingNodeContent) {
    DictionaryTree dict;
    dict.AddWord("dog", "собака");
    auto& ref = dict["dog"];
    ref = "пёс";
    CHECK(dict.GetWord("dog")->getContent() == "пёс");
}

TEST(OperatorSubscript_ChainedModification) {
    DictionaryTree dict;
    dict["count"] = "1";
    dict["count"] += "!";  // modifies the stored string in place
    CHECK(dict.GetWord("count")->getContent() == "1!");
}

TEST(OperatorPlusEq_AddsPair) {
    DictionaryTree dict;
    dict += std::make_pair("dog", "собака");
    CHECK(dict.GetWord("dog") != nullptr);
    CHECK(dict.GetWord("dog")->getContent() == "собака");
}

TEST(OperatorMinusEq_RemovesWord) {
    DictionaryTree dict;
    dict.AddWord("dog", "собака");
    dict -= "dog";
    CHECK(dict.GetWord("dog") == nullptr);
}

TEST(CopyConstructor_DeepCopyKeepsOriginal) {
    DictionaryTree original;
    original.AddWord("dog", "собака");
    original.AddWord("cat", "кошка");
    DictionaryTree copy(original);
    copy.DeleteWord("dog");
    CHECK(original.GetWord("dog") != nullptr);
}

TEST(CopyConstructor_ContentIndependent) {
    DictionaryTree original;
    original.AddWord("dog", "собака");
    DictionaryTree copy(original);
    copy["dog"] = "пёс";
    CHECK(original.GetWord("dog")->getContent() == "собака");
    CHECK(copy.GetWord("dog")->getContent() == "пёс");
}

TEST(CopyAssignment_DeepCopy) {
    DictionaryTree left;
    left.AddWord("dog", "собака");
    DictionaryTree right;
    right.AddWord("cat", "кошка");
    right = left;
    CHECK(right.GetWord("dog") != nullptr);
    right.DeleteWord("dog");
    CHECK(left.GetWord("dog") != nullptr);
}

TEST(SelfAssignment_DoesNotCorrupt) {
    DictionaryTree dict;
    dict.AddWord("dog", "собака");
    dict = dict;
    CHECK(dict.GetWord("dog") != nullptr);
}

TEST(LargeRandomInsert_AllFoundAndInvariantHolds) {
    DictionaryTree dict;
    std::mt19937 rng(12345);
    std::vector<std::string> keys;
    for (int i = 0; i < 100; ++i) keys.push_back("k" + std::to_string(i));
    std::shuffle(keys.begin(), keys.end(), rng);
    for (const auto& k : keys) dict.AddWord(k, "v" + k);
    int found = 0;
    for (const auto& k : keys) {
        DictionaryNode* n = dict.GetWord(k);
        if (n != nullptr && n->getContent() == "v" + k) ++found;
        CheckInvariant(n);
    }
    CHECK(found == 100);
    CHECK(dict.GetWord("missing") == nullptr);
}

int main() {
    return UnitTest::RunAllTests();
}
