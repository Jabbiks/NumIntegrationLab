#include <cmath>
#include <exception>
#include <iomanip>
#include <iostream>
#include <limits>

#include "IntegrationExceptions.h"
#include "IntegrationTask.h"
#include "ScopeLogger.h"

double quadraticFunc(double x)
{
    return x * x; // Інтеграл на [0, 3] дорівнює 9.0
}

// Рівень 3 — виникнення винятку
void computeLevel3()
{
    ScopeLogger logger("computeLevel3");
    std::cout << "  Attempting to create task with invalid interval [5.0, 2.0]...\n";
    IntegrationTask badTask(quadraticFunc, 5.0, 2.0, 1e-6);
}

// Рівень 2 — перехоплення, діагностика та повторне генерування через throw;
void computeLevel2()
{
    ScopeLogger logger("computeLevel2");
    try
    {
        computeLevel3();
    }
    catch (const std::exception& ex)
    {
        std::cout << "  [LOG in computeLevel2] Caught exception: " << ex.what() << '\n';
        throw; // Повторне генерування без втрати типу (no object slicing)
    }
}

// Рівень 1 — проміжний виклик
void computeLevel1()
{
    ScopeLogger logger("computeLevel1");
    computeLevel2();
}

void printTaskState(const IntegrationTask& task)
{
    std::cout << "  Interval: [" << task.left() << ", " << task.right() << "], "
              << "Tolerance: " << task.tolerance() << ", "
              << "HasResults: " << std::boolalpha << task.hasResults() << '\n';
    if (task.hasResults())
    {
        std::cout << std::fixed << std::setprecision(6)
                  << "  Subdivisions n = " << task.lastSubdivisions() << ":\n"
                  << "    Rectangle = " << task.rectangleResult() << '\n'
                  << "    Trapezoid = " << task.trapezoidResult() << '\n'
                  << "    Simpson   = " << task.simpsonResult() << '\n';
    }
}

int main()
{
    std::cout << "=== 1. Valid integration & Strong Guarantee demo ===\n";
    try
    {
        IntegrationTask task(quadraticFunc, 0.0, 3.0, 1e-4);
        task.computeAll(100);
        std::cout << "After successful computeAll(100):\n";
        printTaskState(task);

        std::cout << "\nTrying computeAll(101) (odd partitions -> Simpson fails)...\n";
        try
        {
            task.computeAll(101);
        }
        catch (const PartitionException& ex)
        {
            std::cout << "  Caught PartitionException: " << ex.what()
                      << " (method=" << ex.method()
                      << ", n=" << ex.subdivisions() << ")\n";
        }

        std::cout << "State after failed computeAll(101) (Strong Guarantee preserved):\n";
        printTaskState(task);
    }
    catch (const std::exception& ex)
    {
        std::cout << "Unexpected error: " << ex.what() << '\n';
    }

    std::cout << "\n=== 2. Testing other error scenarios ===\n";
    try
    {
        const double nanVal = std::numeric_limits<double>::quiet_NaN();
        IntegrationTask nanTask(quadraticFunc, nanVal, 3.0);
    }
    catch (const InvalidParameterException& ex)
    {
        std::cout << "Caught InvalidParameterException: " << ex.what()
                  << " [param=" << ex.parameter() << ", val=" << ex.value() << "]\n";
    }

    try
    {
        auto hugeFunc = [](double) { return 1e308; };
        double res = NumIntegration::rectangle(hugeFunc, 0.0, 100.0, 10);
        std::cout << res << '\n';
    }
    catch (const std::overflow_error& ex)
    {
        std::cout << "Caught std::overflow_error: " << ex.what() << '\n';
    }

    std::cout << "\n=== 3. RAII, Stack Unwinding & Rethrow demo ===\n";
    try
    {
        computeLevel1();
    }
    catch (const std::invalid_argument& ex)
    {
        std::cout << "[MAIN] Caught std::invalid_argument: " << ex.what() << '\n';
    }
    catch (const std::exception& ex)
    {
        std::cout << "[MAIN] Caught std::exception: " << ex.what() << '\n';
    }

    std::cout << "\nProgram finished cleanly.\n";
    return 0;
}