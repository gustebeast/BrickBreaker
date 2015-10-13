/**
 * \file StageBuilder.cpp
 *
 * \author Gus Callaway
 *
 * \date 6/18/15
 *
 * \brief Implements the game's stage builder
 */
#include <sys/stat.h>
#include <fstream>
#include "Brick.h"
#include "StageBuilder.h"

StageBuilder::StageBuilder(std::vector<Object*>& objects, sf::Vector2f stageSize,
                           sf::Vector2f origin, float brickHeight, float separation)
        : objects_(objects),
          stageSize_(stageSize),
          origin_(origin),
          brickHeight_(brickHeight),
          separation_(separation)
{
    checkDataFile();
}

bool StageBuilder::getLevel(int level) {
    if (level == 1) {
        // Subtract separation from the stage width to account for the separation to the left of the first brick. Then
        // just divide up the remaining room into the number of bricks needed.
        // The actual drawn width of the bricks will be this minus the separation amount, but this number is needed
        // for placing the bricks.
        float brickWidth = ((stageSize_.x - separation_) / NUM_BRICKS_PER_LINE);

        // See the random number generator for use in the next step
        srand(time(NULL));

        // We want 6 random bricks to be special bricks, so make a vector of 6 brick indices that will be special
        int specials[NUM_SPECIAL_BRICKS];
        for (int i = 0; i < NUM_SPECIAL_BRICKS; ++i) {
            specials[i] = rand() % (NUM_BRICKS_PER_LINE * NUM_BRICK_ROWS);
        }

        for (int row = 0; row < NUM_BRICK_ROWS; ++row) {
            for (int col = 0; col < NUM_BRICKS_PER_LINE; ++col) {

                // Check if this brick is special
                int k = 0;
                for (; k < NUM_SPECIAL_BRICKS; k++) {
                    if (specials[k] == row * NUM_BRICKS_PER_LINE + col) {
                        // This brick is in the list of special brick indexes, so mark it as such
                        objects_.push_back(
                                new Brick(origin_.x + separation_ + brickWidth * col,
                                          origin_.y + separation_ + (NUM_EMPTY_ROWS + row) *
                                                                            (brickHeight_ + separation_),
                                          brickWidth - separation_,
                                          brickHeight_,
                                          SPECIALS[rand() % (sizeof(SPECIALS)/sizeof(char))] // Random special character
                                )
                        );
                        break;
                    }
                }
                // If the brick was not special
                if (k == NUM_SPECIAL_BRICKS) {
                    objects_.push_back(
                            new Brick(origin_.x + separation_ + brickWidth * col,
                                      origin_.y + separation_ + (NUM_EMPTY_ROWS + row) *
                                                                (brickHeight_ + separation_),
                                      brickWidth - separation_,
                                      brickHeight_
                            )
                    );
                }
            }
        }
    }

    // Any level other than level 1 is loaded from file
    else {
        return loadLevelFromFile(level);
    }

    return true;
}

void StageBuilder::addSafetyBricks(int numBricks) {
    float brickWidth = (stageSize_.x - 2 * separation_) / numBricks;

    // Fill the bottom with safety bricks (they have twice the separation and half the height of normal bricks)
    for (int col = 0; col < numBricks; ++col) {
        objects_.push_back(
                new Brick(2 * separation_ + origin_.x + brickWidth * col,
                          origin_.y + stageSize_.y - brickHeight_*.5f - separation_,
                          brickWidth - 2 * separation_,
                          brickHeight_*.5f,
                          's'
                )
        );
    }
}

void StageBuilder::checkDataFile() {
    // Make sure the data folder exists
    mkdir("BrickBreakerData", ACCESSPERMS);

    if (mkdir("BrickBreakerData/levels", ACCESSPERMS) == 0) {
        // If mkdir returns 0, the directory didn't exist and the second level's data file needs to be created
        std::ofstream levelTwoFile("BrickBreakerData/levels/2.txt");
        levelTwoFile << "~                  ~\n";
        levelTwoFile << "~                  ~\n";
        levelTwoFile << "~                  ~\n";
        levelTwoFile << "--------------------\n";
        levelTwoFile << "--------------------\n";

        // Also a readme file teaching people how to create a level
        std::ofstream readMeFile("BrickBreakerData/levels/README.txt");
        readMeFile << "To create your own level, create a file called \"<level #>.txt\". The stage builder will read "
                              "spaces as empty slots, dashes as regular bricks, and tildas as special bricks. "
                              "See \"2.txt\" for an example and make sure to put spaces at the end of lines if you want"
                              " empty space there.";

        // And close both files
        levelTwoFile.close();
        readMeFile.close();
    }
}

bool StageBuilder::loadLevelFromFile(int level) {
    // Load the specified level file, and if it isn't found return false
    std::ifstream levelFile("BrickBreakerData/levels/" + std::to_string(level) + ".txt");
    if (!levelFile.is_open()) {
        return false;
    }

    // Various variables used in the while loop below
    std::string line;
    char special = '\0';
    unsigned long numBricksPerLines = 0;
    float brickWidth = 0;
    int row = 0;
    int col = 0;

    while (getline(levelFile, line)) {
        col = 0;
        // If on the first line, store its length to determine how wide each brick should be
        if (numBricksPerLines == 0) {
            numBricksPerLines = line.size();
            brickWidth = ((stageSize_.x - separation_) / numBricksPerLines);
        }

        // If the line is too long, shorten it. If it is too short, add spaces.
        if (line.size() > numBricksPerLines) {
            line.resize(numBricksPerLines, ' ');
        }

        for (char c : line) {
            // Get the appropriate special character based on the character read from file
            switch (c) {
                case '-': special = '\0';
                          break;

                case '~': special = SPECIALS[rand() % (sizeof(SPECIALS)/sizeof(char))]; // Random special character
                          break;

                default: c = ' '; // No brick in the default case
            }

            // If the character in the file is a space, don't make a brick
            if (c != ' ') {
                objects_.push_back(
                        new Brick(origin_.x + separation_ + brickWidth * col,
                                  origin_.y + separation_ + row * (brickHeight_ + separation_),
                                  brickWidth - separation_,
                                  brickHeight_,
                                  special
                        )
                );
            }
            ++col;
        }
        ++row;
    }

    // Close the file and return true to mark the load successful
    levelFile.close();
    return true;
}
