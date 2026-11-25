#pragma once

#include <functional>

namespace NumIntegration
{
    using MathFunc = std::function<double(double)>;

    // Обчислення визначеного інтеграла методом середніх прямокутників
    double rectangle(const MathFunc& f, double a, double b, int n);

    // Обчислення визначеного інтеграла методом трапецій
    double trapezoid(const MathFunc& f, double a, double b, int n);

    // Обчислення визначеного інтеграла методом Сімпсона (потребує парного n > 0)
    double simpson(const MathFunc& f, double a, double b, int n);

    // Обчислення абсолютної похибки інтегрування
    double integrationError(double exactValue, double approxValue);
}