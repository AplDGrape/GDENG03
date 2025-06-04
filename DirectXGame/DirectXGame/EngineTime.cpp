#include "EngineTime.h"
#include <chrono>
#include <iostream>

EngineTime* EngineTime::sharedInstance = nullptr;

void EngineTime::initialize()
{
    if (sharedInstance == nullptr)
    {
        sharedInstance = new EngineTime();
    }
}

double EngineTime::getDeltaTime()
{
    if (sharedInstance == nullptr)
    {
        std::cerr << "EngineTime not initialized!" << std::endl;
        return 0.0;
    }

    return sharedInstance->deltaTime;
}

void EngineTime::increaseTime(double seconds)
{
    if (sharedInstance == nullptr)
    {
        std::cerr << "EngineTime not initialized!" << std::endl;
        return;
    }

    sharedInstance->deltaTime += seconds;
    std::cout << "Increase Time\n";
}

void EngineTime::decreaseTime(double seconds)
{
    if (sharedInstance == nullptr)
    {
        std::cerr << "EngineTime not initialized!" << std::endl;
        return;
    }

    sharedInstance->deltaTime -= seconds;
    std::cout << "Decrease Time\n";
}

EngineTime::EngineTime()
{
    start = std::chrono::system_clock::now();
}

EngineTime::~EngineTime()
{
}

void EngineTime::LogFrameStart()
{
    if (sharedInstance == nullptr)
    {
        std::cerr << "EngineTime not initialized!" << std::endl;
        return;
    }

    sharedInstance->start = std::chrono::system_clock::now();
}

void EngineTime::LogFrameEnd()
{
    if (sharedInstance == nullptr)
    {
        std::cerr << "EngineTime not initialized!" << std::endl;
        return;
    }

    sharedInstance->end = std::chrono::system_clock::now();
    std::chrono::duration<double> duration = sharedInstance->end - sharedInstance->start;
    sharedInstance->deltaTime = duration.count();
}
