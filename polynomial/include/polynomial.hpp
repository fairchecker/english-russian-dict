/**
 * @file polynomial.hpp
 * @brief Класс Polynomial — многочлен с вещественными коэффициентами.
 *
 * Многочлен хранится как вектор коэффициентов по возрастанию степени:
 * элементы_[i] — коэффициент при x^i.
 */
#pragma once

#include <vector>
#include <cmath>
#include <initializer_list>

/**
 * @class Polynomial
 * @brief Представление многочлена с коэффициентами типа double.
 *
 * Тривиальные (нулевые) старшие коэффициенты отбрасываются (см. Trim()),
 * поэтому getDegree() всегда возвращает степень многочлена,
 * а нулевой многочлен имеет степень 0 и коэффициент 0.0.
 */
class Polynomial {
    public:
    /**
     * @brief Конструктор по умолчанию: создаёт нулевой многочлен.
     */
    Polynomial() : elements_{0.0} {}

    /**
     * @brief Конструктор из списка коэффициентов по возрастанию степени.
     * @param coeffs коэффициенты: coeffs[i] — коэффициент при x^i.
     *
     * Лидирующие нули отбрасываются, поэтому Polynomial{0.0, 0.0, 1.0}
     * представляет x^2.
     */
    Polynomial(std::initializer_list<double> coeffs);

    /**
     * @brief Сложение с присваиванием: *this = *this + other.
     * @param other второй операнд (слагаемое).
     * @return ссылка на *this после сложения.
     */
    Polynomial& operator+=(const Polynomial& other);
    /**
     * @brief Вычитание с присваиванием: *this = *this - other.
     * @param other второй операнд (вычитаемое).
     * @return ссылка на *this после вычитания.
     */
    Polynomial& operator-=(const Polynomial& other);
    /**
     * @brief Умножение с присваиванием: *this = *this * other.
     * @param other второй операнд (множитель).
     * @return ссылка на *this после умножения.
     */
    Polynomial& operator*=(const Polynomial& other);
    /**
     * @brief Деление с присваиванием: *this = *this / other.
     * @param other делитель.
     * @return ссылка на *this после деления.
     * @throws std::invalid_argument если other является нулевым многочленом.
     *
     * Возвращается только частное от деления с остатком.
     */
    Polynomial& operator/=(const Polynomial& other);

    /**
     * @brief Бинарное сложение двух многочленов.
     * @param left первое слагаемое.
     * @param right второе слагаемое.
     * @return новый многочлен, равный left + right.
     */
    friend Polynomial operator+(Polynomial left, const Polynomial& right);
    /**
     * @brief Бинарное вычитание двух многочленов.
     * @param left уменьшаемое.
     * @param right вычитаемое.
     * @return новый многочлен, равный left - right.
     */
    friend Polynomial operator-(Polynomial left, const Polynomial& right);
    /**
     * @brief Бинарное умножение двух многочленов.
     * @param left первый множитель.
     * @param right второй множитель.
     * @return новый многочлен, равный left * right.
     */
    friend Polynomial operator*(Polynomial left, const Polynomial& right);
    /**
     * @brief Бинарное деление двух многочленов.
     * @param left делимое.
     * @param right делитель.
     * @return частное от деления left на right.
     * @throws std::invalid_argument если right является нулевым многочленом.
     */
    friend Polynomial operator/(Polynomial left, const Polynomial& right);

    /**
     * @brief Вычисление значения многочлена в точке x (схема Горнера).
     * @param x точка, в которой вычисляется значение.
     * @return значение многочлена P(x).
     */
    double operator()(double x);

    /**
     * @brief Чтение коэффициента при x^number (без изменения многочлена).
     * @param number степень, коэффициент которой запрашивается.
     * @return коэффициент при x^number; 0, если number больше степени многочлена.
     */
    double operator[](size_t number) const {
        if(number >= elements_.size()) return 0;
        return elements_[number];
    }

    /**
     * @brief Чтение/запись коэффициента при x^number (с расширением степени).
     * @param number степень, коэффициент которой запрашивается.
     * @return ссылка на коэффициент; при необходимости вектор расширяется,
     *         недостающие коэффициенты заполняются нулями.
     */
    double& operator[](size_t number) {
        if (number >= elements_.size()) {
            elements_.resize(number + 1, 0.0); 
        }
        return elements_[number];
    }

    /**
     * @brief Степень многочлена.
     * @return старшая степень с ненулевым коэффициентом (0 для нулевого многочлена).
     */
    size_t getDegree() const {
        return elements_.size() - 1;
    }

    /**
     * @brief Проверка, является ли многочлен нулевым.
     * @return true, если все коэффициенты (пренебрежимо малы) равны нулю.
     */
    bool isZero() const {
        return elements_.size() == 1 && std::abs(elements_[0]) < 1e-9;
    }

    /**
     * @brief Количество хранимых коэффициентов.
     * @return размер внутреннего вектора коэффициентов (степень + 1).
     */
    const size_t getElementsSize() const { return elements_.size(); }

    /**
     * @brief Изменение размера внутреннего массива коэффициентов.
     * @param newSize новый размер вектора.
     */
    void resizeElements(size_t newSize) { elements_.resize(newSize); }

    private:
    /**
     * @brief Отбрасывание лидирующих (старших) нулевых коэффициентов.
     *
     * Гарантирует, что последний элемент вектора не равен нулю,
     * а вектор никогда не пуст (минимум {0.0}).
     */
    void Trim();

    /// Коэффициенты многочлена по возрастанию степени: elements_[i] — при x^i.
    std::vector<double> elements_;
};