
#include <iostream>
#include <string>
#include <chrono>
#include <future>
#include <thread>
#include "FoxEngine.hpp"

struct Transform
{
	int x;
};

int main(int argc, char**args)
{
	fox::Application app;
	app.run();
    return 0;
}
