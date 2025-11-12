/// ISA ZS 2025
/// xzluva01

#pragma once

#include <vector>

#include "arguments.hpp"

struct Context
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