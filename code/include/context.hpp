#pragma once
#include "arguments.hpp"

struct Context
{
    const char* const APP_NAME = "snmp2otel"; // c++ grammar garbage, LET US USE RUST NEXT TIME!
    const Arguments args;

    static Context& getInstance(const Arguments* args = nullptr)
    {
        static Context ctx = args ? Context(*args) : throw std::runtime_error("Cotext initialized without args");
        return ctx;
    }

private:
    Context(const Arguments& arguments) : args(arguments) { }
};