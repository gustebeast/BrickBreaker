# BrickBreaker
A brick breaker game coded in C++ using the SFML graphics library.

Gameplay: https://youtu.be/XGiKX7UlHaE

Note: Although this game is in a working state, not all its features have been implemented. The project has been temporarily suspended until I have more time to work on it.

In the "CompiledGame" directory you will find an app that should work on most computers running OSX. Also provided is an executable compiled on OSX 10.10 along with required support folders "BrickBreakerData" and "Frameworks." You are also free to recompile the program from the .h and .cpp files, although you will need to link it with SFML's libraries.

Features:
  - Carefully simulated physics. Balls react appropriately when hitting other balls or corners of bricks.
  - Paddle rotation, allowing for fine tuned ball angling.
  - Color coded special bricks that can give you an extra ball or ellongate your paddle temporarily.
  - Slick color pallet.
  - Fluid controls.
  - Unique safety brick system. No extra lives, just a set of bricks at the bottom of the screen to reflect back your ball. The more balls you have at once, the more likely you are to deplete your safety bricks, but the faster you can complete the level.
  - Level timer, paired with a high score system that records your best time for each level (press ESC to see).
  - Simple, text based custom level builder that allows you to easily create and share as many levels as you desire.
