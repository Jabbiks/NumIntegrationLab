#pragma once

#include "NumIntegration.h"

// Клас задачі чисельного інтегрування з контролем інваріантів:
// 1. func_ != nullptr
// 2. std::isfinite(left_) && std::isfinite(right_) && left_ < right_
// 3. std::isfinite(tolerance_) && tolerance_ > 0.0
// 4. Якщо hasResults_ == true, то lastSubdivisions_ > 0 (парне), а всі 3 результати скінченні
class IntegrationTask
{
private:
    NumIntegration::MathFunc func_;
    double left_;
    double right_;
    double tolerance_;

    bool hasResults_ = false;
    int lastSubdivisions_ = 0;
    double rectangleResult_ = 0.0;
    double trapezoidResult_ = 0.0;
    double simpsonResult_ = 0.0;

    static void validateParameters(const NumIntegration::MathFunc& func,
                                   double left,
                                   double right,
                                   double tolerance);

public:
    IntegrationTask(NumIntegration::MathFunc func,
                    double left,
                    double right,
                    double tolerance = 1e-6);

    // Методи читання стану (no-throw guarantee)
    double left() const noexcept;
    double right() const noexcept;
    double tolerance() const noexcept;
    bool hasResults() const noexcept;
    int lastSubdivisions() const noexcept;
    double rectangleResult() const noexcept;
    double trapezoidResult() const noexcept;
    double simpsonResult() const noexcept;

    // Strong guarantee: оновлює межі та точність лише після успішної перевірки
    void setParameters(double left, double right, double tolerance);

    // Strong guarantee: одночасно обчислює та зберігає результати трьох методів
    // (прямокутників, трапецій та Сімпсона). Якщо хоча б один розрахунок завершиться
    // винятком, попередні збережені результати залишаються незмінними.
    void computeAll(int subdivisions);

    // Перевірка досягнення заданої точності між методами трапецій та Сімпсона
    // (генерує std::logic_error, якщо обчислення ще не проводилися)
    bool isConverged() const;
};