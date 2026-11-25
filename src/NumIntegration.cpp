#include "NumIntegration.h"
#include "IntegrationExceptions.h"

#include <cmath>
#include <stdexcept>
#include <string>

namespace
{
    void validateCommonInputs(const NumIntegration::MathFunc& f,
                              double a,
                              double b,
                              int n,
                              const std::string& methodName)
    {
        if (!f)
        {
            throw InvalidParameterException("f", 0.0, "Integrand function cannot be null");
        }
        if (!std::isfinite(a))
        {
            throw InvalidParameterException("a", a, "Lower integration limit must be finite");
        }
        if (!std::isfinite(b))
        {
            throw InvalidParameterException("b", b, "Upper integration limit must be finite");
        }
        if (a >= b)
        {
            throw std::invalid_argument("Lower integration limit 'a' must be less than upper limit 'b'");
        }
        if (n <= 0)
        {
            throw PartitionException(methodName, n, "Number of subdivisions must be positive");
        }
    }

    double evaluateFunctionChecked(const NumIntegration::MathFunc& f, double x)
    {
        const double fx = f(x);
        if (!std::isfinite(fx))
        {
            throw InvalidParameterException("f(x)", fx, "Function value must be finite (not NaN or Inf)");
        }
        return fx;
    }
}

namespace NumIntegration
{
    double rectangle(const MathFunc& f, double a, double b, int n)
    {
        validateCommonInputs(f, a, b, n, "rectangle");

        const double h = (b - a) / static_cast<double>(n);
        if (!std::isfinite(h) || h <= 0.0)
        {
            throw std::overflow_error("Step size overflow in rectangle method");
        }

        double sum = 0.0;
        for (int i = 0; i < n; ++i)
        {
            const double x = a + (static_cast<double>(i) + 0.5) * h;
            sum += evaluateFunctionChecked(f, x);
            if (!std::isfinite(sum))
            {
                throw std::overflow_error("Intermediate sum overflow in rectangle method");
            }
        }

        const double result = sum * h;
        if (!std::isfinite(result))
        {
            throw std::overflow_error("Final result overflow in rectangle method");
        }
        return result;
    }

    double trapezoid(const MathFunc& f, double a, double b, int n)
    {
        validateCommonInputs(f, a, b, n, "trapezoid");

        const double h = (b - a) / static_cast<double>(n);
        if (!std::isfinite(h) || h <= 0.0)
        {
            throw std::overflow_error("Step size overflow in trapezoid method");
        }

        double sum = 0.5 * (evaluateFunctionChecked(f, a) + evaluateFunctionChecked(f, b));
        if (!std::isfinite(sum))
        {
            throw std::overflow_error("Boundary sum overflow in trapezoid method");
        }

        for (int i = 1; i < n; ++i)
        {
            const double x = a + static_cast<double>(i) * h;
            sum += evaluateFunctionChecked(f, x);
            if (!std::isfinite(sum))
            {
                throw std::overflow_error("Intermediate sum overflow in trapezoid method");
            }
        }

        const double result = sum * h;
        if (!std::isfinite(result))
        {
            throw std::overflow_error("Final result overflow in trapezoid method");
        }
        return result;
    }

    double simpson(const MathFunc& f, double a, double b, int n)
    {
        validateCommonInputs(f, a, b, n, "simpson");

        if (n % 2 != 0)
        {
            throw PartitionException("simpson", n, "Simpson's rule requires an even number of subdivisions");
        }

        const double h = (b - a) / static_cast<double>(n);
        if (!std::isfinite(h) || h <= 0.0)
        {
            throw std::overflow_error("Step size overflow in Simpson method");
        }

        double sum = evaluateFunctionChecked(f, a) + evaluateFunctionChecked(f, b);
        if (!std::isfinite(sum))
        {
            throw std::overflow_error("Boundary sum overflow in Simpson method");
        }

        for (int i = 1; i < n; ++i)
        {
            const double x = a + static_cast<double>(i) * h;
            const double weight = (i % 2 == 0) ? 2.0 : 4.0;
            sum += weight * evaluateFunctionChecked(f, x);
            if (!std::isfinite(sum))
            {
                throw std::overflow_error("Intermediate sum overflow in Simpson method");
            }
        }

        const double result = sum * (h / 3.0);
        if (!std::isfinite(result))
        {
            throw std::overflow_error("Final result overflow in Simpson method");
        }
        return result;
    }

    double integrationError(double exactValue, double approxValue)
    {
        if (!std::isfinite(exactValue))
        {
            throw InvalidParameterException("exactValue", exactValue, "Exact value must be finite");
        }
        if (!std::isfinite(approxValue))
        {
            throw InvalidParameterException("approxValue", approxValue, "Approximate value must be finite");
        }

        const double diff = std::abs(exactValue - approxValue);
        if (!std::isfinite(diff))
        {
            throw std::overflow_error("Overflow while calculating integration error");
        }
        return diff;
    }
}