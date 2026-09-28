/**
 * @file polynomial.cpp
 * @brief Реализация класса Polynomial — многочлена с вещественными коэффициентами.
 */

#include "polynomial.hpp"
#include <stdexcept>

/**
 * @brief Конструктор из списка коэффициентов по возрастанию степени.
 * @param coeffs коэффициенты: coeffs[i] — коэффициент при x^i.
 *
 * После копирования коэффициентов вызывается Trim() для отбрасывания
 * лидирующих нулей.
 */
Polynomial::Polynomial(std::initializer_list<double> coeffs) : elements_(coeffs) {
    Trim();
}

/**
 * @brief Отбрасывание лидирующих (старших) нулевых коэффициентов.
 *
 * Пока в векторе больше одного элемента и последний коэффициент
 * пренебрежимо мал (по модулю меньше 1e-9), он удаляется. Если вектор
 * оказался пустым, в него добавляется единственный нулевой коэффициент,
 * чтобы гарантировать представление нулевого многочлена как {0.0}.
 */
void Polynomial::Trim() {
    while (elements_.size() > 1 && std::abs(elements_.back()) < 1e-9) {
        elements_.pop_back();
    }
    if (elements_.empty()) {
        elements_.push_back(0.0);
    }
}

/**
 * @brief Сложение с присваиванием: *this = *this + other.
 * @param other второй операнд (слагаемое).
 * @return ссылка на *this после сложения.
 *
 * При необходимости внутренний вектор расширяется до размера other,
 * после чего коэффициенты складываются поэлементно. В конце вызывается
 * Trim().
 */
Polynomial& Polynomial::operator+=(const Polynomial& other){
    if(other.getElementsSize() > elements_.size()){
        elements_.resize(other.getElementsSize(), 0.0);
    }
    const size_t otherSize = other.getElementsSize();
    for(size_t i = 0; i < otherSize; i++){
        elements_[i] += other[i];
    }
    Trim();
    return *this;
}

/**
 * @brief Бинарное сложение двух многочленов.
 * @param left первое слагаемое (передаётся по значению и изменяется на месте).
 * @param right второе слагаемое.
 * @return новый многочлен, равный left + right.
 */
Polynomial operator+(Polynomial left, const Polynomial& right) {
    left += right;
    return left;
}

/**
 * @brief Вычитание с присваиванием: *this = *this - other.
 * @param other второй операнд (вычитаемое).
 * @return ссылка на *this после вычитания.
 *
 * При необходимости внутренний вектор расширяется до размера other,
 * после чего коэффициенты вычитаются поэлементно. В конце вызывается
 * Trim().
 */
Polynomial& Polynomial::operator-=(const Polynomial& other) {
    if (other.getElementsSize() > elements_.size()) {
        elements_.resize(other.getElementsSize(), 0.0);
    }
    const size_t otherSize = other.getElementsSize();
    for (size_t i = 0; i < otherSize; ++i) {
        elements_[i] -= other[i];
    }
    Trim();
    return *this;
}

/**
 * @brief Бинарное вычитание двух многочленов.
 * @param left уменьшаемое (передаётся по значению и изменяется на месте).
 * @param right вычитаемое.
 * @return новый многочлен, равный left - right.
 */
Polynomial operator-(Polynomial left, const Polynomial& right) {
    left -= right;
    return left;
}

/**
 * @brief Умножение с присваиванием: *this = *this * other.
 * @param other второй операнд (множитель).
 * @return ссылка на *this после умножения.
 *
 * Реализовано «в лоб» через двойной цикл: коэффициент при x^(i+j)
 * результата равен сумме произведений elements_[j] * other[i].
 * Размер результата — сумма размеров операндов минус один. В конце
 * вызывается Trim().
 */
Polynomial& Polynomial::operator*=(const Polynomial& other){
    std::vector<double> newElements(elements_.size() + other.getElementsSize() - 1, 0.0);

    size_t otherSize = other.getElementsSize();
    for(size_t i = 0; i < otherSize; i++){
        for(size_t j = 0; j < elements_.size(); j++){
            newElements[i+j] += elements_[j] * other[i];
        }
    }
    elements_ = std::move(newElements);
    Trim();
    return *this;
}

/**
 * @brief Бинарное умножение двух многочленов.
 * @param left первый множитель (передаётся по значению и изменяется на месте).
 * @param right второй множитель.
 * @return новый многочлен, равный left * right.
 */
Polynomial operator*(Polynomial left, const Polynomial& right){
    left *= right;
    return left;
}

/**
 * @brief Деление с присваиванием: *this = *this / other.
 * @param other делитель.
 * @return ссылка на *this после деления (на частное).
 * @throws std::invalid_argument если other является нулевым многочленом.
 *
 * Выполняется деление с остатком «уголком»: на каждом шаге старший член
 * остатка делится на старший член делителя, полученный коэффициент
 * добавляется к частному, а из остатка вычитается соответствующее
 * произведение. Возвращается только частное, остаток отбрасывается.
 */
Polynomial& Polynomial::operator/=(const Polynomial& other) {
    if(other.isZero()) {
        throw std::invalid_argument("division by zero");
    }

    Polynomial quotient;
    Polynomial remainder = *this;

    while(!remainder.isZero() && remainder.getDegree() >= other.getDegree()){
        double coef = remainder.elements_.back() / other.elements_.back();
        size_t num = remainder.getDegree() - other.getDegree();

        quotient[num] += coef;

        for(size_t i = 0; i < other.getElementsSize(); ++i){
            remainder[i + num] -= coef * other[i];
        }
        remainder.Trim();
    }

    *this = quotient;
    return *this;
}

/**
 * @brief Бинарное деление двух многочленов.
 * @param left делимое (передаётся по значению и изменяется на месте).
 * @param right делитель.
 * @return частное от деления left на right.
 * @throws std::invalid_argument если right является нулевым многочленом.
 */
Polynomial operator/(Polynomial left, const Polynomial& right){
    left /= right;
    return left;
}

/**
 * @brief Вычисление значения многочлена в точке x (схема Горнера).
 * @param x точка, в которой вычисляется значение.
 * @return значение многочлена P(x).
 *
 * Коэффициенты обходятся от старшего к младшему: result = result * x + a_i.
 */
double Polynomial::operator()(double x) {
    double result = 0.0;
    for (int i = static_cast<int>(elements_.size()) - 1; i >= 0; --i) {
        result = result * x + elements_[i];
    }
    return result;
}
