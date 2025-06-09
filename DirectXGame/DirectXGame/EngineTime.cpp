#include "EngineTime.h"
#include <chrono>
#include <iostream>

EngineTime* EngineTime::sharedInstance = NULL;

void EngineTime::initialize()
{
    /*if (sharedInstance == nullptr)
    {
        sharedInstance = new EngineTime();
    }*/
    sharedInstance = new EngineTime();
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

void EngineTime::update()
{
    if (!sharedInstance) return;

    sharedInstance->end = std::chrono::system_clock::now();
    std::chrono::duration<double> frameDuration = sharedInstance->end - sharedInstance->start;
    sharedInstance->deltaTime = frameDuration.count();
    sharedInstance->simulatedTime += sharedInstance->deltaTime;
    sharedInstance->start = sharedInstance->end;
}

double EngineTime::getTime()
{
    return sharedInstance ? sharedInstance->simulatedTime : 0.0;
}

void EngineTime::increaseTime(double seconds)
{
    if (sharedInstance)
    {
        sharedInstance->simulatedTime += seconds;
        std::cout << "Increase Time: " << sharedInstance->simulatedTime << "s\n";
    }
}

void EngineTime::decreaseTime(double seconds)
{
    if (sharedInstance)
    {
        sharedInstance->simulatedTime -= seconds;
        std::cout << "Decrease Time: " << sharedInstance->simulatedTime << "s\n";
    }
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
