#pragma once

#include <string>
#include <memory>
#include <utility>

/**
 * @file dictionary_node.hpp
 * @brief Узел бинарного дерева поиска, хранящий одну пару «ключ — значение».
 */

/**
 * @class DictionaryNode
 * @brief Узел словаря, реализованного как бинарное дерево поиска.
 *
 * Хранит слово-ключ (английское слово) и связанный с ним контент
 * (перевод/значение), а также указатели на левого и правого потомков.
 * Потомки владеются через std::unique_ptr, поэтому память освобождается
 * автоматически при уничтожении узла.
 */
class DictionaryNode{
    public:
    /**
     * @brief Конструктор узла.
     * @param key слово-ключ.
     * @param content значение (перевод), связанное с ключом.
     */
    DictionaryNode(std::string key, std::string content);

    /// @brief Возвращает неизменяемую ссылку на хранимое значение.
    const std::string& getContent() const { return content_; }
    /// @brief Возвращает неизменяемую ссылку на ключ.
    const std::string& getKey() const { return key_; }
    /// @brief Возвращает изменяемую ссылку на значение (для правки на месте).
    std::string& getContentChangeable() { return content_; }

    /// @brief Возвращает указатель на правого потомка (или nullptr).
    DictionaryNode* getRight() const { return right_.get(); }
    /// @brief Возвращает указатель на левого потомка (или nullptr).
    DictionaryNode* getLeft() const { return left_.get(); }

    /// @brief Возвращает ссылку на unique_ptr правого потомка.
    std::unique_ptr<DictionaryNode>& getRightPtr() { return right_; }
    /// @brief Возвращает ссылку на unique_ptr левого потомка.
    std::unique_ptr<DictionaryNode>& getLeftPtr() { return left_; }

    /// @brief Устанавливает правого потомка.
    void setRight(std::unique_ptr<DictionaryNode> node) { right_ = std::move(node); }
    /// @brief Устанавливает левого потомка.
    void setLeft(std::unique_ptr<DictionaryNode> node) { left_ = std::move(node); }
    /// @brief Заменяет значение узла.
    void setContent(std::string content) { content_ = std::move(content); }
    /// @brief Заменяет ключ узла.
    void setKey(std::string key) { key_ = std::move(key); }

    private:
    /// Ключ узла (английское слово).
    std::string key_;
    /// Значение узла (перевод/контент).
    std::string content_; 
    /// Правый потомок.
    std::unique_ptr<DictionaryNode> right_;
    /// Левый потомок.
    std::unique_ptr<DictionaryNode> left_;
};