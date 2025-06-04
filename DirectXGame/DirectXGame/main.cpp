
#include "AppWindow.h"
#include "EngineTime.h"
#include <iostream>
#include <chrono>

int main()
{
	EngineTime::initialize();
	//EngineTime::increaseTime(5.0);  // Increase time by 5 seconds
	//EngineTime::decreaseTime(2.0);  // Decrease time by 2 seconds

	// Retrieve and print the current delta time
	/*double deltaTime = EngineTime::getDeltaTime();
	std::cout << "Current Delta Time: " << deltaTime << " seconds" << std::endl;*/

	AppWindow app;
	if (app.init())
	{
		while (app.isRun())
		{
			app.broadcast();
		}
	}

	return 0;
}