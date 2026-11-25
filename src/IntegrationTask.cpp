#include "IntegrationTask.h"
#include "IntegrationExceptions.h"

#include <cmath>
#include <stdexcept>
#include <utility>

void IntegrationTask::validateParameters(const NumIntegration::MathFunc& func,
                                         double left,
                                         double right,
                                         double tolerance)
{
    if (!func)
    {
        throw InvalidParameterException("func", 0.0, "Integrand function cannot be null");
    }
    if (!std::isfinite(left))
    {
        throw InvalidParameterException("left", left, "Left boundary must be finite");
    }
    if (!std::isfinite(right))
    {
        throw InvalidParameterException("right", right, "Right boundary must be finite");
    }
    if (left >= right)
    {
        throw std::invalid_argument("Left boundary must be strictly less than right boundary");
    }
    if (!std::isfinite(right - left))
    {
        throw std::overflow_error("Integration interval length overflow");
    }
    if (!std::isfinite(tolerance) || tolerance <= 0.0)
    {
        throw InvalidParameterException("tolerance", tolerance, "Tolerance must be finite and positive");
    }
}

IntegrationTask::IntegrationTask(NumIntegration::MathFunc func,
                                 double left,
                                 double right,
                                 double tolerance)
    : func_(std::move(func)),
      left_(left),
      right_(right),
      tolerance_(tolerance)
{
    validateParameters(func_, left_, right_, tolerance_);
}

double IntegrationTask::left() const noexcept
{
    return left_;
}

double IntegrationTask::right() const noexcept
{
    return right_;
}

double IntegrationTask::tolerance() const noexcept
{
    return tolerance_;
}

bool IntegrationTask::hasResults() const noexcept
{
    return hasResults_;
}

int IntegrationTask::lastSubdivisions() const noexcept
{
    return lastSubdivisions_;
}

double IntegrationTask::rectangleResult() const noexcept
{
    return rectangleResult_;
}

double IntegrationTask::trapezoidResult() const noexcept
{
    return trapezoidResult_;
}

double IntegrationTask::simpsonResult() const noexcept
{
    return simpsonResult_;
}

void IntegrationTask::setParameters(double left, double right, double tolerance)
{
    // 1. Попередня перевірка (якщо виникне виняток, об'єкт не зміниться)
    validateParameters(func_, left, right, tolerance);

    // 2. Фіксація нового стану (noexcept операції присвоєння)
    left_ = left;
    right_ = right;
    tolerance_ = tolerance;

    hasResults_ = false;
    lastSubdivisions_ = 0;
    rectangleResult_ = 0.0;
    trapezoidResult_ = 0.0;
    simpsonResult_ = 0.0;
}

void IntegrationTask::computeAll(int subdivisions)
{
    // 1. Підготовка нового стану: обчислюємо всі три методи в тимчасові змінні.
    // Якщо будь-який із трьох викликів згенерує виняток (наприклад, непарне subdivisions
    // для методу Сімпсона, NaN/Inf або overflow), поля класу залишаться недоторканими.
    const double rect = NumIntegration::rectangle(func_, left_, right_, subdivisions);
    const double trap = NumIntegration::trapezoid(func_, left_, right_, subdivisions);
    const double simp = NumIntegration::simpson(func_, left_, right_, subdivisions);

    // 2. Атомарна фіксація нового стану (операції не генерують винятків)
    rectangleResult_ = rect;
    trapezoidResult_ = trap;
    simpsonResult_ = simp;
    lastSubdivisions_ = subdivisions;
    hasResults_ = true;
}

bool IntegrationTask::isConverged() const
{
    if (!hasResults_)
    {
        throw std::logic_error("Cannot check convergence before computing integration results");
    }
    return std::abs(simpsonResult_ - trapezoidResult_) <= tolerance_;
}