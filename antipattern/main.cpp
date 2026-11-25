#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>

// Антипатерн 1: генерування винятку примітивного типу int
void throwIntError()
{
    throw 404;
}

// Антипатерн 2: генерування об'єкта std::string замість класу ієрархії std::exception
void throwStringError()
{
    throw std::string("Invalid integration limits");
}

// Антипатерн 3: зрізання об'єкта (object slicing) при використанні throw ex;
void demonstrateObjectSlicing()
{
    try
    {
        throw std::invalid_argument("Left limit >= Right limit");
    }
    catch (const std::exception& ex)
    {
        // ПОМИЛКА: копіюється лише базова частина std::exception!
        throw ex;
    }
}

int main()
{
    std::cout << "=== 1. Primitive int exception ===\n";
    try
    {
        throwIntError();
    }
    catch (const std::exception& ex)
    {
        std::cout << "Caught std::exception: " << ex.what() << '\n';
    }
    catch (...)
    {
        std::cout << "Caught via catch (...) because int does not inherit from std::exception!\n";
    }

    std::cout << "\n=== 2. std::string exception ===\n";
    try
    {
        throwStringError();
    }
    catch (const std::exception& ex)
    {
        std::cout << "Caught std::exception: " << ex.what() << '\n';
    }
    catch (...)
    {
        std::cout << "Caught via catch (...) because std::string is not std::exception!\n";
    }

    std::cout << "\n=== 3. Object slicing with 'throw ex;' ===\n";
    try
    {
        demonstrateObjectSlicing();
    }
    catch (const std::invalid_argument& ex)
    {
        std::cout << "Caught exact std::invalid_argument: " << ex.what() << '\n';
    }
    catch (const std::exception& ex)
    {
        std::cout << "Caught sliced base std::exception: " << ex.what() << '\n';
    }

    return 0;
}