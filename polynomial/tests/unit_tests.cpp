/**
 * @file unit_tests.cpp
 * @brief Юнит-тесты класса Polynomial на GoogleTest.
 *
 * Запуск:
 *   ctest --test-dir <build-dir> --output-on-failure
 * или напрямую: ./test_polynomial
 */

#include "polynomial.hpp"

#include <gtest/gtest.h>

#include <cmath>
#include <stdexcept>
#include <vector>

namespace {

/// Допуск для сравнения вещественных коэффициентов.
constexpr double kEpsilon = 1e-9;

/// Сравнивает многочлены по степени и коэффициентам.
void ExpectPolynomialEq(const Polynomial& actual, const std::vector<double>& expected) {
    ASSERT_EQ(expected.size() - 1, actual.getDegree());
    for (std::size_t i = 0; i < expected.size(); ++i) {
        EXPECT_NEAR(expected[i], actual[i], kEpsilon) << "коэффициент при x^" << i;
    }
}

/// Сравнивает два многочлена (для проверок коммутативности и т. п.).
void ExpectPolynomialsEq(const Polynomial& actual, const Polynomial& expected) {
    ASSERT_EQ(expected.getDegree(), actual.getDegree());
    for (std::size_t i = 0; i <= expected.getDegree(); ++i) {
        EXPECT_NEAR(expected[i], actual[i], kEpsilon) << "коэффициент при x^" << i;
    }
}

/// Проверяет значения многочлена сразу в нескольких точках.
/// Ссылка не константная: operator() у Polynomial не объявлен const.
void ExpectValueAt(Polynomial& polynomial, double x, double expected) {
    EXPECT_NEAR(expected, polynomial(x), kEpsilon) << "P(" << x << ")";
}

}  // namespace

// ===========================================================================
// Конструкторы
// ===========================================================================
TEST(PolynomialConstructor, DefaultConstructorIsZero) {
    Polynomial p;
    EXPECT_EQ(0u, p.getDegree());
    EXPECT_TRUE(p.isZero());
    EXPECT_NEAR(0.0, p[0], kEpsilon);
}

TEST(PolynomialConstructor, InitializerListKeepsDegree) {
    Polynomial p{1.0, 2.0, 3.0};
    EXPECT_EQ(2u, p.getDegree());
    EXPECT_NEAR(3.0, p[2], kEpsilon);
}

TEST(PolynomialConstructor, InitializerListTrimsTrailingZeros) {
    Polynomial p{1.0, 2.0, 0.0, 0.0};
    EXPECT_EQ(1u, p.getDegree());
}

TEST(PolynomialConstructor, InitializerListOfZerosIsZeroPolynomial) {
    Polynomial p{0.0, 0.0, 0.0};
    EXPECT_TRUE(p.isZero());
    EXPECT_EQ(0u, p.getDegree());
}

TEST(PolynomialConstructor, EmptyInitializerListStaysValid) {
    Polynomial p{};
    EXPECT_EQ(0u, p.getDegree());
    EXPECT_TRUE(p.isZero());
    EXPECT_NEAR(0.0, p[7], kEpsilon);
}

// ===========================================================================
// Доступ к коэффициентам
// ===========================================================================
TEST(PolynomialAccessor, ReadCoefficient) {
    Polynomial p{1.0, 2.0, 3.0};
    EXPECT_NEAR(2.0, p[1], kEpsilon);
}

TEST(PolynomialAccessor, ReadOutOfBoundsReturnsZero) {
    Polynomial p{1.0, 2.0};
    EXPECT_NEAR(0.0, p[5], kEpsilon);
    EXPECT_NEAR(0.0, p[100], kEpsilon);
}

TEST(PolynomialAccessor, WriteCoefficient) {
    Polynomial p{1.0, 2.0};
    p[1] = 5.0;
    EXPECT_NEAR(5.0, p[1], kEpsilon);
}

TEST(PolynomialAccessor, WriteExpandsDegree) {
    Polynomial p{1.0};
    p[3] = 5.0;
    EXPECT_EQ(3u, p.getDegree());
    EXPECT_NEAR(0.0, p[2], kEpsilon);
    EXPECT_NEAR(5.0, p[3], kEpsilon);
}

TEST(PolynomialAccessor, ElementsSizeIsDegreePlusOne) {
    Polynomial p{1.0, 2.0, 3.0};
    EXPECT_EQ(3u, p.getElementsSize());
    EXPECT_EQ(p.getDegree() + 1, p.getElementsSize());
}

TEST(PolynomialAccessor, ResizeElementsChangesStorage) {
    Polynomial p{1.0, 2.0};
    p.resizeElements(5);
    EXPECT_EQ(5u, p.getElementsSize());
    EXPECT_EQ(4u, p.getDegree());
    EXPECT_NEAR(0.0, p[4], kEpsilon);
    EXPECT_NEAR(2.0, p[1], kEpsilon);
}

// ===========================================================================
// Вычисление значения
// ===========================================================================
TEST(PolynomialEvaluation, Constant) {
    Polynomial p{5.0};
    ExpectValueAt(p, 0.0, 5.0);
    ExpectValueAt(p, 10.0, 5.0);
    ExpectValueAt(p, -3.5, 5.0);
}

TEST(PolynomialEvaluation, Linear) {
    Polynomial p{1.0, 2.0};  // 1 + 2x
    ExpectValueAt(p, 0.0, 1.0);
    ExpectValueAt(p, 2.0, 5.0);
}

TEST(PolynomialEvaluation, Quadratic) {
    Polynomial p{1.0, 2.0, 3.0};  // 1 + 2x + 3x^2
    ExpectValueAt(p, 1.0, 6.0);
    ExpectValueAt(p, 2.0, 17.0);
}

TEST(PolynomialEvaluation, NegativeArgument) {
    Polynomial p{1.0, -2.0, 3.0};  // P(-1) = 1 + 2 + 3 = 6
    ExpectValueAt(p, -1.0, 6.0);
}

TEST(PolynomialEvaluation, ZeroPolynomialIsZeroEverywhere) {
    Polynomial p{0.0};
    ExpectValueAt(p, 0.0, 0.0);
    ExpectValueAt(p, 42.0, 0.0);
}

TEST(PolynomialEvaluation, HornerMatchesNaiveSum) {
    Polynomial p{2.0, -3.0, 0.5, 4.0};
    for (double x : {-2.0, -0.5, 0.0, 1.5, 3.0}) {
        const double naive = 2.0 + -3.0 * x + 0.5 * x * x + 4.0 * x * x * x;
        EXPECT_NEAR(naive, p(x), kEpsilon) << "P(" << x << ")";
    }
}

// ===========================================================================
// Сложение
// ===========================================================================
TEST(PolynomialAddition, SameDegree) {
    Polynomial result = Polynomial{1.0, 2.0, 3.0} + Polynomial{4.0, 5.0, 6.0};
    ExpectPolynomialEq(result, {5.0, 7.0, 9.0});
}

TEST(PolynomialAddition, DifferentDegree) {
    Polynomial result = Polynomial{1.0, 2.0} + Polynomial{3.0, 4.0, 5.0};
    ExpectPolynomialEq(result, {4.0, 6.0, 5.0});
}

TEST(PolynomialAddition, ZeroPolynomialChangesNothing) {
    Polynomial result = Polynomial{1.0, 2.0} + Polynomial{0.0};
    ExpectPolynomialEq(result, {1.0, 2.0});
}

TEST(PolynomialAddition, PlusEquals) {
    Polynomial p{1.0, 2.0};
    p += Polynomial{3.0, 4.0};
    ExpectPolynomialEq(p, {4.0, 6.0});
}

TEST(PolynomialAddition, CancellationTrimsDegree) {
    // x^2 + (x - x^2) = x: старший коэффициент обнуляется и обрезается.
    Polynomial result = Polynomial{0.0, 0.0, 1.0} + Polynomial{0.0, 1.0, -1.0};
    EXPECT_EQ(1u, result.getDegree());
    ExpectPolynomialEq(result, {0.0, 1.0});
}

TEST(PolynomialAddition, Commutativity) {
    const Polynomial a{1.0, 2.0, 3.0};
    const Polynomial b{4.0, 5.0};
    ExpectPolynomialsEq(a + b, b + a);
}

// ===========================================================================
// Вычитание
// ===========================================================================
TEST(PolynomialSubtraction, SameDegree) {
    Polynomial result = Polynomial{5.0, 6.0, 7.0} - Polynomial{1.0, 2.0, 3.0};
    ExpectPolynomialEq(result, {4.0, 4.0, 4.0});
}

TEST(PolynomialSubtraction, SubtractItselfGivesZero) {
    Polynomial result = Polynomial{1.0, 2.0, 3.0} - Polynomial{1.0, 2.0, 3.0};
    EXPECT_TRUE(result.isZero());
    EXPECT_EQ(0u, result.getDegree());
}

TEST(PolynomialSubtraction, SmallerFromLarger) {
    // 1 - (1 + x + x^2) = -x - x^2
    Polynomial result = Polynomial{1.0} - Polynomial{1.0, 1.0, 1.0};
    ExpectPolynomialEq(result, {0.0, -1.0, -1.0});
}

TEST(PolynomialSubtraction, MinusEquals) {
    Polynomial p{5.0, 5.0};
    p -= Polynomial{2.0, 3.0};
    ExpectPolynomialEq(p, {3.0, 2.0});
}

TEST(PolynomialSubtraction, ZeroSubtrahend) {
    Polynomial result = Polynomial{1.0, 2.0} - Polynomial{0.0};
    ExpectPolynomialEq(result, {1.0, 2.0});
}

TEST(PolynomialSubtraction, HigherDegreeSubtrahendExpands) {
    Polynomial result = Polynomial{1.0} - Polynomial{0.0, 0.0, 1.0};
    ExpectPolynomialEq(result, {1.0, 0.0, -1.0});
}

// ===========================================================================
// Умножение
// ===========================================================================
TEST(PolynomialMultiplication, TwoLinears) {
    // (1 + 2x)(3 + 4x) = 3 + 10x + 8x^2
    Polynomial result = Polynomial{1.0, 2.0} * Polynomial{3.0, 4.0};
    ExpectPolynomialEq(result, {3.0, 10.0, 8.0});
}

TEST(PolynomialMultiplication, ByConstant) {
    Polynomial result = Polynomial{1.0, 2.0, 3.0} * Polynomial{5.0};
    ExpectPolynomialEq(result, {5.0, 10.0, 15.0});
}

TEST(PolynomialMultiplication, ByZeroGivesZero) {
    Polynomial result = Polynomial{1.0, 2.0, 3.0} * Polynomial{0.0};
    EXPECT_TRUE(result.isZero());
}

TEST(PolynomialMultiplication, StarEquals) {
    Polynomial p{1.0, 1.0};
    p *= Polynomial{1.0, -1.0};  // (1 + x)(1 - x) = 1 - x^2
    ExpectPolynomialEq(p, {1.0, 0.0, -1.0});
}

TEST(PolynomialMultiplication, Commutativity) {
    const Polynomial a{1.0, 2.0};
    const Polynomial b{3.0, 4.0, 5.0};
    ExpectPolynomialsEq(a * b, b * a);
}

TEST(PolynomialMultiplication, Distributivity) {
    const Polynomial a{1.0, 2.0};
    const Polynomial b{3.0, 4.0};
    const Polynomial c{5.0, 6.0, 7.0};
    ExpectPolynomialsEq(a * (b + c), (a * b) + (a * c));
}

// ===========================================================================
// Деление
// ===========================================================================
TEST(PolynomialDivision, Evenly) {
    // (3 + 10x + 8x^2) / (1 + 2x) = 3 + 4x
    Polynomial result = Polynomial{3.0, 10.0, 8.0} / Polynomial{1.0, 2.0};
    ExpectPolynomialEq(result, {3.0, 4.0});
}

TEST(PolynomialDivision, WithRemainderKeepsQuotientOnly) {
    // (1 + x^2) / (1 + x) = -1 + x, остаток отбрасывается.
    Polynomial result = Polynomial{1.0, 0.0, 1.0} / Polynomial{1.0, 1.0};
    ExpectPolynomialEq(result, {-1.0, 1.0});
}

TEST(PolynomialDivision, ByZeroThrows) {
    const Polynomial dividend{1.0, 2.0};
    const Polynomial zero{0.0};
    EXPECT_THROW(dividend / zero, std::invalid_argument);
    EXPECT_THROW((Polynomial{0.0, 0.0, 0.0} / zero), std::invalid_argument);
}

TEST(PolynomialDivision, SlashEqualsByZeroThrows) {
    Polynomial p{1.0, 2.0};
    const Polynomial zero{0.0};
    EXPECT_THROW(p /= zero, std::invalid_argument);
}

TEST(PolynomialDivision, ByItselfGivesOne) {
    Polynomial result = Polynomial{2.0, 4.0, 6.0} / Polynomial{2.0, 4.0, 6.0};
    ExpectPolynomialEq(result, {1.0});
}

TEST(PolynomialDivision, SlashEquals) {
    Polynomial p{3.0, 10.0, 8.0};
    p /= Polynomial{1.0, 2.0};
    ExpectPolynomialEq(p, {3.0, 4.0});
}

TEST(PolynomialDivision, ByConstant) {
    Polynomial result = Polynomial{3.0, 6.0, 9.0} / Polynomial{3.0};
    ExpectPolynomialEq(result, {1.0, 2.0, 3.0});
}

TEST(PolynomialDivision, ByHigherDegreeGivesZero) {
    Polynomial result = Polynomial{1.0, 2.0} / Polynomial{1.0, 0.0, 1.0};
    EXPECT_TRUE(result.isZero());
}

TEST(PolynomialDivision, ExactMultiplicationRoundTrip) {
    const Polynomial divisor{1.0, 2.0};
    const Polynomial quotient{5.0, -3.0, 2.0};
    const Polynomial dividend = divisor * quotient;
    ExpectPolynomialEq(dividend / divisor, {5.0, -3.0, 2.0});
}

TEST(PolynomialDivision, EvaluatesLikeTheOriginal) {
    // Частное и исходный многочлен совпадают в точках, где деление точное.
    Polynomial dividend{2.0, 0.0, -2.0};   // 2 - 2x^2
    Polynomial divisor{1.0, 1.0};         // 1 + x
    Polynomial quotient{2.0, -2.0};       // (2 - 2x^2) / (1 + x) = 2 - 2x
    Polynomial computed = dividend / divisor;
    ExpectPolynomialEq(computed, {2.0, -2.0});
    EXPECT_NEAR(quotient(0.5), computed(0.5), kEpsilon);
}
