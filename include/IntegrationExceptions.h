#pragma once

#include <stdexcept>
#include <string>

// Базовий клас користувацьких винятків бібліотеки чисельного інтегрування
class IntegrationException : public std::runtime_error
{
public:
    explicit IntegrationException(const std::string& message)
        : std::runtime_error(message)
    {
    }
};

// Виняток для некоректних числових параметрів (NaN, Inf, tolerance <= 0, nullptr)
class InvalidParameterException : public IntegrationException
{
private:
    std::string parameter_;
    double value_;

public:
    InvalidParameterException(const std::string& parameter, double value, const std::string& details = "")
        : IntegrationException(
              "Invalid parameter '" + parameter + "'" +
              (details.empty() ? "" : ": " + details)),
          parameter_(parameter),
          value_(value)
    {
    }

    const std::string& parameter() const noexcept
    {
        return parameter_;
    }

    double value() const noexcept
    {
        return value_;
    }
};

// Виняток для некоректної кількості розбиттів (n <= 0 або непарне n для методу Сімпсона)
class PartitionException : public IntegrationException
{
private:
    std::string method_;
    int subdivisions_;

public:
    PartitionException(const std::string& method, int subdivisions, const std::string& details)
        : IntegrationException("Invalid partition count in '" + method + "': " + details),
          method_(method),
          subdivisions_(subdivisions)
    {
    }

    const std::string& method() const noexcept
    {
        return method_;
    }

    int subdivisions() const noexcept
    {
        return subdivisions_;
    }
};