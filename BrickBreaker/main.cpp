#include <SFML/Graphics.hpp>
#include <iostream>
#include <unistd.h>
#include "GraphicsRunner.h"

using namespace sf;

int main( int numArgs, char *args[]) {
    // This changes the working directory to the location of the executable //

    // Get the last position of '/'
    std::string aux(args[0]);

    // get '/' or '\\' depending on unix/mac or windows.
    #if defined(_WIN32) || defined(WIN32)
        int pos = aux.rfind('\\');
    #else
        unsigned long pos = aux.rfind('/');
    #endif

    // Get the path and change the directory
    chdir(aux.substr(0,pos+1).c_str());
    // // // // // // // // // // // // // // // // // // // // // // // // //

    // Initialize the window with antialiasing and a fixed frame rate
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Brick Breaker", Style::Default, settings);
    window.setFramerateLimit(60);

    // Create a graphics runner to hold all the game's objects and handle the clear draw display loop
    GraphicsRunner game = GraphicsRunner(window);

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            game.handleEvent(event);
        }

        game.update();
    }
    return 0;
}

//int main() {
//    main2();
//    std::this_thread::sleep_for(std::chrono::seconds(5));
//    return 0;
//}