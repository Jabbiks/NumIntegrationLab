#include <gtest/gtest.h>

#include <cmath>
#include <limits>
#include <stdexcept>

#include "IntegrationExceptions.h"
#include "IntegrationTask.h"
#include "NumIntegration.h"

namespace
{
    double squareFunc(double x)
    {
        return x * x; // \int_0^3 x^2 dx = 9.0
    }

    double linearFunc(double x)
    {
        return 2.0 * x + 1.0; // \int_0^4 (2x + 1) dx = 20.0
    }
}

// 1. Перевірка коректності обчислень трьох методів та похибки (EXPECT_NEAR)
TEST(NumIntegrationMethodsTest, CalculatesIntegralsAccurately)
{
    const double exact = 9.0;
    const double rect = NumIntegration::rectangle(squareFunc, 0.0, 3.0, 100);
    const double trap = NumIntegration::trapezoid(squareFunc, 0.0, 3.0, 100);
    const double simp = NumIntegration::simpson(squareFunc, 0.0, 3.0, 100);

    EXPECT_NEAR(rect, exact, 1e-3);
    EXPECT_NEAR(trap, exact, 1e-3);
    EXPECT_NEAR(simp, exact, 1e-12);
    EXPECT_NEAR(NumIntegration::integrationError(exact, simp), 0.0, 1e-12);
}

// 2. Граничні значення: мінімальна допустима кількість розбиттів n = 2 для Сімпсона
TEST(NumIntegrationMethodsTest, HandlesMinimalEvenSubdivisions)
{
    const double exact = 20.0;
    EXPECT_NEAR(NumIntegration::simpson(linearFunc, 0.0, 4.0, 2), exact, 1e-12);
}

// 3. Помилка: недопустимий інтервал (a >= b) -> std::invalid_argument
TEST(NumIntegrationExceptionsTest, RejectsInvalidInterval)
{
    EXPECT_THROW(NumIntegration::rectangle(squareFunc, 3.0, 1.0, 10), std::invalid_argument);
    EXPECT_THROW(NumIntegration::trapezoid(squareFunc, 2.0, 2.0, 10), std::invalid_argument);
    EXPECT_THROW(IntegrationTask(squareFunc, 5.0, 5.0), std::invalid_argument);
}

// 4. Помилка: нульова або від'ємна кількість розбиттів -> PartitionException
TEST(NumIntegrationExceptionsTest, RejectsZeroOrNegativeSubdivisions)
{
    EXPECT_THROW(NumIntegration::rectangle(squareFunc, 0.0, 3.0, 0), PartitionException);
    EXPECT_THROW(NumIntegration::trapezoid(squareFunc, 0.0, 3.0, -5), PartitionException);
    EXPECT_THROW(NumIntegration::simpson(squareFunc, 0.0, 3.0, 0), PartitionException);
}

// 5. Помилка: непарна кількість розбиттів для методу Сімпсона -> PartitionException
TEST(NumIntegrationExceptionsTest, RejectsOddSubdivisionsForSimpson)
{
    EXPECT_THROW(NumIntegration::simpson(squareFunc, 0.0, 3.0, 3), PartitionException);
    EXPECT_THROW(NumIntegration::simpson(squareFunc, 0.0, 3.0, 101), PartitionException);
}

// 6. Помилка: NaN або Inf у межах, точності, функції чи результаті -> InvalidParameterException
TEST(NumIntegrationExceptionsTest, RejectsNaNAndInfinityParameters)
{
    const double inf = std::numeric_limits<double>::infinity();
    const double nan = std::numeric_limits<double>::quiet_NaN();

    EXPECT_THROW(IntegrationTask(nullptr, 0.0, 3.0), InvalidParameterException);
    EXPECT_THROW(IntegrationTask(squareFunc, nan, 3.0), InvalidParameterException);
    EXPECT_THROW(IntegrationTask(squareFunc, 0.0, inf), InvalidParameterException);
    EXPECT_THROW(IntegrationTask(squareFunc, 0.0, 3.0, 0.0), InvalidParameterException);
    EXPECT_THROW(IntegrationTask(squareFunc, 0.0, 3.0, -1e-5), InvalidParameterException);
    EXPECT_THROW(IntegrationTask(squareFunc, 0.0, 3.0, nan), InvalidParameterException);

    auto nanFunc = [](double) { return std::numeric_limits<double>::quiet_NaN(); };
    EXPECT_THROW(NumIntegration::rectangle(nanFunc, 0.0, 3.0, 10), InvalidParameterException);
}

// 7. Помилка: переповнення результату -> std::overflow_error
TEST(NumIntegrationExceptionsTest, DetectsResultOverflow)
{
    auto hugeFunc = [](double) { return 1e308; };
    EXPECT_THROW(NumIntegration::rectangle(hugeFunc, 0.0, 100.0, 10), std::overflow_error);
    EXPECT_THROW(NumIntegration::trapezoid(hugeFunc, 0.0, 100.0, 10), std::overflow_error);
    EXPECT_THROW(NumIntegration::simpson(hugeFunc, 0.0, 100.0, 10), std::overflow_error);
}

// 8. Перевірка інваріантів класу IntegrationTask та методу isConverged()
TEST(IntegrationTaskTest, MaintainsInvariantsAndChecksConvergence)
{
    IntegrationTask task(squareFunc, 0.0, 3.0, 1e-3);
    EXPECT_DOUBLE_EQ(task.left(), 0.0);
    EXPECT_DOUBLE_EQ(task.right(), 3.0);
    EXPECT_DOUBLE_EQ(task.tolerance(), 1e-3);
    EXPECT_FALSE(task.hasResults());

    EXPECT_THROW(task.isConverged(), std::logic_error);

    task.computeAll(100);
    EXPECT_TRUE(task.hasResults());
    EXPECT_EQ(task.lastSubdivisions(), 100);
    EXPECT_TRUE(task.isConverged());
}

// 9. Сильна гарантія безпеки (Strong Guarantee) у computeAll()
TEST(IntegrationTaskTest, ComputeAllPreservesStateOnException)
{
    IntegrationTask task(squareFunc, 0.0, 3.0, 1e-4);
    task.computeAll(100);

    const double prevRect = task.rectangleResult();
    const double prevTrap = task.trapezoidResult();
    const double prevSimp = task.simpsonResult();

    // Викликаємо з непарним n = 101: метод Сімпсона генерує PartitionException,
    // але попередні результати для n = 100 повинні залишитися незмінними
    EXPECT_THROW(task.computeAll(101), PartitionException);

    ASSERT_TRUE(task.hasResults());
    EXPECT_EQ(task.lastSubdivisions(), 100);
    EXPECT_DOUBLE_EQ(task.rectangleResult(), prevRect);
    EXPECT_DOUBLE_EQ(task.trapezoidResult(), prevTrap);
    EXPECT_DOUBLE_EQ(task.simpsonResult(), prevSimp);
}

// 10. Сильна гарантія безпеки (Strong Guarantee) у setParameters()
TEST(IntegrationTaskTest, SetParametersPreservesStateOnException)
{
    IntegrationTask task(squareFunc, 0.0, 3.0, 1e-4);
    task.computeAll(50);

    const double prevRect = task.rectangleResult();
    const double prevTrap = task.trapezoidResult();
    const double prevSimp = task.simpsonResult();

    EXPECT_THROW(task.setParameters(10.0, 2.0, 1e-5), std::invalid_argument);

    EXPECT_DOUBLE_EQ(task.left(), 0.0);
    EXPECT_DOUBLE_EQ(task.right(), 3.0);
    EXPECT_DOUBLE_EQ(task.tolerance(), 1e-4);
    ASSERT_TRUE(task.hasResults());
    EXPECT_EQ(task.lastSubdivisions(), 50);
    EXPECT_DOUBLE_EQ(task.rectangleResult(), prevRect);
    EXPECT_DOUBLE_EQ(task.trapezoidResult(), prevTrap);
    EXPECT_DOUBLE_EQ(task.simpsonResult(), prevSimp);
}