/// ISA ZS 2025
/// xzluva01 Adam Žluva

#pragma once

#include <vector>

#include "arguments.hpp"

/// @brief Singleton struct that holds things needed globally
struct Context // well this ended up obsolete... xd
{
    const Arguments args;

    static Context& getInstance(const Arguments* args = nullptr)
    {
        static Context ctx = args ? Context(*args) : throw std::runtime_error("Cotext initialized without args");
        return ctx;
    }

private:
    Context(const Arguments& arguments) : args(arguments) { }
};