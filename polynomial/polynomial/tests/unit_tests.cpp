#include "polynomial.hpp"
#include <UnitTest++/UnitTest++.h>
#include <cmath>

const double EPSILON = 1e-9;

// ==========================================
// 1. КОНСТРУКТОРЫ (4 теста)
// ==========================================
SUITE(ConstructorTests) {
    TEST(DefaultConstructor) {
        Polynomial p;
        CHECK_EQUAL(0u, p.getDegree());
        CHECK_CLOSE(0.0, p[0], EPSILON);
    }

    TEST(InitializerListBasic) {
        Polynomial p{1.0, 2.0, 3.0};
        CHECK_EQUAL(2u, p.getDegree());
        CHECK_CLOSE(3.0, p[2], EPSILON);
    }
    
    TEST(InitializerListTrimsZeros) {
        Polynomial p{1.0, 2.0, 0.0, 0.0};
        CHECK_EQUAL(1u, p.getDegree()); // Лишние нули обрезаны
    }

    TEST(InitializerListAllZeros) {
        Polynomial p{0.0, 0.0, 0.0};
        CHECK(p.isZero());
        CHECK_EQUAL(0u, p.getDegree());
    }
}

// ==========================================
// 2. ОПЕРАТОРЫ ДОСТУПА И ГЕТТЕРЫ (4 теста)
// ==========================================
SUITE(AccessorTests) {
    TEST(ReadCoefficient) {
        Polynomial p{1.0, 2.0, 3.0};
        CHECK_CLOSE(2.0, p[1], EPSILON);
    }
    
    TEST(ReadOutOfBoundsReturnsZero) {
        Polynomial p{1.0, 2.0};
        CHECK_CLOSE(0.0, p[5], EPSILON);
        CHECK_CLOSE(0.0, p[100], EPSILON);
    }
    
    TEST(WriteCoefficient) {
        Polynomial p{1.0, 2.0};
        p[1] = 5.0;
        CHECK_CLOSE(5.0, p[1], EPSILON);
    }
    
    TEST(ExpandOnWrite) {
        Polynomial p{1.0};
        p[3] = 5.0;
        CHECK_EQUAL(3u, p.getDegree());
        CHECK_CLOSE(0.0, p[2], EPSILON);
        CHECK_CLOSE(5.0, p[3], EPSILON);
    }
}

// ==========================================
// 3. ВЫЧИСЛЕНИЕ ЗНАЧЕНИЯ (4 теста)
// ==========================================
SUITE(EvaluationTests) {
    TEST(EvaluateConstant) {
        Polynomial p{5.0};
        CHECK_CLOSE(5.0, p(0.0), EPSILON);
        CHECK_CLOSE(5.0, p(10.0), EPSILON);
    }
    
    TEST(EvaluateLinear) {
        Polynomial p{1.0, 2.0}; // 1 + 2x
        CHECK_CLOSE(1.0, p(0.0), EPSILON);
        CHECK_CLOSE(5.0, p(2.0), EPSILON);
    }
    
    TEST(EvaluateQuadratic) {
        Polynomial p{1.0, 2.0, 3.0}; // 1 + 2x + 3x^2
        CHECK_CLOSE(6.0, p(1.0), EPSILON);
        CHECK_CLOSE(17.0, p(2.0), EPSILON);
    }

    TEST(EvaluateNegativeX) {
        Polynomial p{1.0, -2.0, 3.0}; // 1 - 2x + 3x^2
        // P(-1) = 1 - 2(-1) + 3(1) = 1 + 2 + 3 = 6
        CHECK_CLOSE(6.0, p(-1.0), EPSILON);
    }
}

// ==========================================
// 4. СЛОЖЕНИЕ (5 тестов)
// ==========================================
SUITE(AdditionTests) {
    TEST(AddSameDegree) {
        Polynomial p1{1.0, 2.0, 3.0};
        Polynomial p2{4.0, 5.0, 6.0};
        Polynomial res = p1 + p2;
        CHECK_CLOSE(5.0, res[0], EPSILON);
        CHECK_CLOSE(7.0, res[1], EPSILON);
        CHECK_CLOSE(9.0, res[2], EPSILON);
    }
    
    TEST(AddDifferentDegree) {
        Polynomial p1{1.0, 2.0};
        Polynomial p2{3.0, 4.0, 5.0};
        Polynomial res = p1 + p2;
        CHECK_EQUAL(2u, res.getDegree());
        CHECK_CLOSE(4.0, res[0], EPSILON);
    }

    TEST(AddZeroPolynomial) {
        Polynomial p1{1.0, 2.0};
        Polynomial p2{0.0};
        Polynomial res = p1 + p2;
        CHECK_CLOSE(1.0, res[0], EPSILON);
        CHECK_CLOSE(2.0, res[1], EPSILON);
    }
    
    TEST(PlusEqualsOperator) {
        Polynomial p1{1.0, 2.0};
        Polynomial p2{3.0, 4.0};
        p1 += p2;
        CHECK_CLOSE(4.0, p1[0], EPSILON);
        CHECK_CLOSE(6.0, p1[1], EPSILON);
    }

    TEST(AddCausesTrim) {
        // x^2 + (-x^2 + x) = x. Старший коэффициент должен обнулиться и обрезаться
        Polynomial p1{0.0, 0.0, 1.0}; 
        Polynomial p2{0.0, 1.0, -1.0};
        Polynomial res = p1 + p2;
        CHECK_EQUAL(1u, res.getDegree()); // Степень упала с 2 до 1
        CHECK_CLOSE(1.0, res[1], EPSILON);
    }
}

// ==========================================
// 5. ВЫЧИТАНИЕ (5 тестов)
// ==========================================
SUITE(SubtractionTests) {
    TEST(SubtractSameDegree) {
        Polynomial p1{5.0, 6.0, 7.0};
        Polynomial p2{1.0, 2.0, 3.0};
        Polynomial res = p1 - p2;
        CHECK_CLOSE(4.0, res[0], EPSILON);
        CHECK_CLOSE(4.0, res[1], EPSILON);
        CHECK_CLOSE(4.0, res[2], EPSILON);
    }
    
    TEST(SubtractToZero) {
        Polynomial p1{1.0, 2.0, 3.0};
        Polynomial p2{1.0, 2.0, 3.0};
        Polynomial res = p1 - p2;
        CHECK(res.isZero());
    }

    TEST(SubtractLargerFromSmaller) {
        // 1 - (1 + x + x^2) = -x - x^2
        Polynomial p1{1.0};
        Polynomial p2{1.0, 1.0, 1.0};
        Polynomial res = p1 - p2;
        CHECK_EQUAL(2u, res.getDegree());
        CHECK_CLOSE(0.0, res[0], EPSILON);
        CHECK_CLOSE(-1.0, res[1], EPSILON);
        CHECK_CLOSE(-1.0, res[2], EPSILON);
    }
    
    TEST(MinusEqualsOperator) {
        Polynomial p1{5.0, 5.0};
        Polynomial p2{2.0, 3.0};
        p1 -= p2;
        CHECK_CLOSE(3.0, p1[0], EPSILON);
        CHECK_CLOSE(2.0, p1[1], EPSILON);
    }

    TEST(SubtractZeroPolynomial) {
        Polynomial p1{1.0, 2.0};
        Polynomial p2{0.0};
        Polynomial res = p1 - p2;
        CHECK_CLOSE(1.0, res[0], EPSILON);
        CHECK_CLOSE(2.0, res[1], EPSILON);
    }
}

// ==========================================
// 6. УМНОЖЕНИЕ (4 теста)
// ==========================================
SUITE(MultiplicationTests) {
    TEST(MultiplyLinear) {
        // (1 + 2x) * (3 + 4x) = 3 + 10x + 8x^2
        Polynomial p1{1.0, 2.0};
        Polynomial p2{3.0, 4.0};
        Polynomial res = p1 * p2;
        CHECK_EQUAL(2u, res.getDegree());
        CHECK_CLOSE(3.0, res[0], EPSILON);
        CHECK_CLOSE(10.0, res[1], EPSILON);
        CHECK_CLOSE(8.0, res[2], EPSILON);
    }
    
    TEST(MultiplyByConstant) {
        Polynomial p1{1.0, 2.0, 3.0};
        Polynomial p2{5.0};
        Polynomial res = p1 * p2;
        CHECK_CLOSE(5.0, res[0], EPSILON);
        CHECK_CLOSE(10.0, res[1], EPSILON);
        CHECK_CLOSE(15.0, res[2], EPSILON);
    }

    TEST(MultiplyByZero) {
        Polynomial p1{1.0, 2.0, 3.0};
        Polynomial p2{0.0};
        Polynomial res = p1 * p2;
        CHECK(res.isZero());
    }

    TEST(MultiplyCommutativity) {
        // A * B == B * A
        Polynomial p1{1.0, 2.0};
        Polynomial p2{3.0, 4.0, 5.0};
        Polynomial res1 = p1 * p2;
        Polynomial res2 = p2 * p1;
        
        CHECK_EQUAL(res1.getDegree(), res2.getDegree());
        for(size_t i = 0; i <= res1.getDegree(); ++i) {
            CHECK_CLOSE(res1[i], res2[i], EPSILON);
        }
    }
}

// ==========================================
// 7. ДЕЛЕНИЕ (4 теста)
// ==========================================
SUITE(DivisionTests) {
    TEST(DivideEvenly) {
        // (3 + 10x + 8x^2) / (1 + 2x) = 3 + 4x
        Polynomial p1{3.0, 10.0, 8.0};
        Polynomial p2{1.0, 2.0};
        Polynomial res = p1 / p2;
        CHECK_EQUAL(1u, res.getDegree());
        CHECK_CLOSE(3.0, res[0], EPSILON);
        CHECK_CLOSE(4.0, res[1], EPSILON);
    }
    
    TEST(DivideWithRemainder) {
        // (1 + x^2) / (1 + x) = -1 + x (остаток 2, но оператор / возвращает только частное)
        Polynomial p1{1.0, 0.0, 1.0};
        Polynomial p2{1.0, 1.0};
        Polynomial res = p1 / p2;
        CHECK_EQUAL(1u, res.getDegree());
        CHECK_CLOSE(-1.0, res[0], EPSILON);
        CHECK_CLOSE(1.0, res[1], EPSILON);
    }

    TEST(DivideByZeroThrows) {
        Polynomial p1{1.0, 2.0};
        Polynomial p2{0.0};
        CHECK_THROW(p1 / p2, std::invalid_argument);
    }

    TEST(DivideByItself) {
        // P / P = 1
        Polynomial p1{2.0, 4.0, 6.0};
        Polynomial p2{2.0, 4.0, 6.0};
        Polynomial res = p1 / p2;
        CHECK_EQUAL(0u, res.getDegree());
        CHECK_CLOSE(1.0, res[0], EPSILON);
    }
}

// Главная функция
int main(int, char*[]) {
    return UnitTest::RunAllTests();
}