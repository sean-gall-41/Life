/*
 * File: the_game_of_life_test.cpp
 * -------------------------------
 * Notes: This file implements John Conway's "Game of Life" through a
 * simple console application. Some extended features include wrapping of
 * the game world upon itself, random world generation, as well as a gui.
 * This file is essentially the same as the_game_of_life.cpp except we have
 * added gui capabilities. See the_game_of_life_test.cpp for be detailed
 * comments on everything but the gui
 * TODO: combine the random functionality with the gui (though decrease world
 * size)
 */

#include <cctype>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include "console.h"
#include "filelib.h"
#include "grid.h"
#include "gwindow.h" //seems redundant knowing also included in lifegui.h
#include "simpio.h"
#include "strlib.h"
#include "lifegui.h"
using namespace std;

//NOTE: the function prototypes below take one more argument here, the lifeGUI
//      object

/*Function prototypes for implementing the game of life*/
void genAnimate(Grid<char>& inGrid, LifeGUI& inBoard, int numFrames, bool& wrap);
void genTick(Grid<char>& inGrid, bool wrap);
void printGameBoard(Grid<char>& inGrid, LifeGUI& inBoard);

int main() {
    cout << "Welcome to the Game Of Life,\na simulation of the lifecycle of "
         << "a bacteria colony.\nCells (X) live and die by the following "
         << "rules:\n- A cell with 1 or fewer neighbors dies.\n- Locations "
         << "with 2 neighbors remain stable.\n- Locations with 3 neighbors "
         << "will create life.\n- A cell with 4 or more neighbors dies.\n"
         << endl;
    bool badName = true;
    ifstream input;
    /*Try to open the ifilestream from user input. Continue until stream
    doesn't break */
    while (badName)
    {
        string fileName = getLine("Grid input file name? ");
        input.open(fileName.c_str());
        if (!input.is_open())
        {
            cerr << "Unable to open that file. Try again." << endl;
        }
        else badName = false;
    }
    /*Now we have to read two integers from the file*/
    int rows = 0, columns = 0;
    input >> rows >> columns; //Maybe try getline(input, someString)
                              // then int rows = StringToInteger(someString)?

    /*Now have to read contents of starting configuration into grid*/
    Grid<char> gameGrid(rows, columns);
    LifeGUI gameBoard; //Declare a gameboard window object for gui capability
    gameBoard.resize(rows, columns); //Initialize and draw the board
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < columns; j++)
        {
            input >> gameGrid[i][j];
        }
    }
//    input.close();
    /*Now prompt user if they wish for gameGrid to wrap*/
    bool wrapping = getYesOrNo("Should the simulation wrap around the grid (y/n)? ");
    bool notDone = true;
    char userResponse = '?';
    /*First, print the current generation configuration*/
    printGameBoard(gameGrid, gameBoard);
    while (notDone) //loop until user wishes to quit
    {
        /*Some input validation for user response*/
        bool badInput = true;
        while (badInput)
        {
            string userLine = getLine("a)nimate, t)ick, q)uit? ");
            userResponse = tolower(userLine[0]);
            if (userResponse == 'a' || userResponse == 't' || userResponse == 'q')
            {
                badInput = false;
            }
            else
            {
                cerr << "Answer not recognized. Try again." << endl;
            }
        }
        /*Now implement the game of life*/
        if (userResponse == 'a')
        {
            genAnimate(gameGrid, gameBoard, getInteger("How may frames? "), wrapping);
        }
        else if (userResponse == 't')
        {
           genTick(gameGrid, wrapping);
           printGameBoard(gameGrid, gameBoard);
        }
        else if (userResponse == 'q') notDone = false;
    }
    //Once user has finished, we print our final statement
    cout << "Have a nice Life!" << endl;
    getLine("Press [Enter] to close the window... ");
    return 0;
}

void printGameBoard(Grid<char>& inGrid, LifeGUI& inBoard)
{
    for (int i = 0; i < inGrid.numRows(); i++)
    {
        for (int j = 0; j < inGrid.numCols(); j++)
        {
            if (inGrid[i][j] == 'X') //this cell is alive
            {
                inBoard.drawCell(i,j, true); //draw a cell at inGrid_ij
            }
            else
            {
                inBoard.drawCell(i,j, false);
            }
//            cout << inGrid[i][j];
        }
//        cout << endl;
    }
}

void genTick(Grid<char>& inGrid, bool wrap)
{
    /*First create a vector object for saving contents of next generation
      without altering current generation. This way we do not compare entries
      that have changed (are part of the next generation) with unchanged
      entries (as part of the current generation). Using a vector because
      I think it is computationally cheaper than using another grid?*/
    Vector<char> nextGeneration;

    /*indices i & j are for looping through each entry in our grid*/
    for (int i = 0; i < inGrid.numRows(); i++)
    {
        for (int j = 0; j < inGrid.numCols(); j++)
        {
            int numAlive = 0; //tracks the number of cells that are alive
                              //around entry inGrid_ij
            /*indices m and n count all the possible values around inGrid_ij*/
            for (int m = -1; m <= 1; m++)
            {
                for (int n = -1; n <= 1; n++)
                {
                    if (!(m == 0 && n == 0)) //Do not wish to count inGrid_ij
                    {
                        if (wrap) //If user chose to wrap
                        {
                            if (inGrid[(i+m+inGrid.numRows())%inGrid.numRows()][(j+n+inGrid.numCols())%inGrid.numCols()] == 'X') ++numAlive;
                        }
                        else
                        {
                           if (inGrid.inBounds(i+m,j+n)) //For no wrap, must check this is true
                           {
                               if (inGrid[i+m][j+n] == 'X') ++numAlive;
                           }

                        }
                    }
                }
            }
            /*Now implement the game rules*/
            /*Rules 1 and 4: location with 0, 1 or >3 neigbors dies*/
            if (numAlive < 2 || numAlive > 3) nextGeneration.push_back('-');
            /*Rule 2: location with two neighbors is stable*/
            else if (numAlive == 2) nextGeneration.push_back(inGrid[i][j]);
            /*Rule 3: location with 3 neighbors creates life*/
            else if (numAlive == 3) nextGeneration.push_back('X');
        }
    }
    /*Once we scan the entire current generation and place the new generation
      into our vector, it is time to replace the values of our grid*/
    int vecIt = 0;
    for (int i = 0; i < inGrid.numRows(); i++)
    {
        for (int j = 0; j < inGrid.numCols(); j++)
        {
            inGrid[i][j] = nextGeneration[vecIt];
            vecIt++;
        }
    }
}

void genAnimate(Grid<char>& inGrid, LifeGUI &inBoard, int numFrames, bool& wrap)
{
//    cout << "(" + integerToString(numFrames) + " new generations are shown, "
//         << "with screen clear and 50ms pause before each)" << endl;
    cout << "(" + integerToString(numFrames) + " new generations are shown)"
         << endl;
    for (int i = 0; i < numFrames; i++)
    {
//        clearConsole();
//        cout << "==================== (console cleared) ====================" << endl;
        genTick(inGrid, wrap);
        printGameBoard(inGrid, inBoard);
        //pause(10);
    }
}

