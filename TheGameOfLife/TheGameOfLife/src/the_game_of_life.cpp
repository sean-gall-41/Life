/*
 * File: the_game_of_life.cpp
 * -------------------------------
 * Notes: This file implements John Conway's "Game of Life" through a
 * simple console application. Some extended features include wrapping of
 * the game world upon itself, and random world generation. This file serves
 * as the test bench for adding features. A gui is implemented in
 * graphics_included_game_of_life folder
 * NOTE: this copy, in its current form is an exact copy of the test_bench
 * version, without implementation notes for functions (im lazy) so if you
 * are confused why things are done, check out the test_bench file
 */

#include <iostream>
#include <fstream>
#include <vector>
#include "grid.h"
#include "console.h"
#include "gwindow.h"
#include "simpio.h"  // for getLine
using namespace std;

/*Function prototypes for implementing the game of life*/
void genAnimate(Grid<char>& inGrid, int numFrames, bool& wrap);
void genTick(Grid<char>& inGrid, bool wrap);
void printGameBoard(Grid<char>& inGrid);

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
    Grid<char> gameBoard(rows, columns);
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < columns; j++)
        {
            input >> gameBoard[i][j];
        }
    }
//    input.close();
    /*Now prompt user if they wish for gameboard to wrap*/
    bool wrapping = getYesOrNo("Should the simulation wrap around the grid (y/n)? ");
    bool notDone = true;
    char userResponse = '?';
    /*First, print the current generation configuration*/
    printGameBoard(gameBoard);
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
            genAnimate(gameBoard, getInteger("How may frames? "), wrapping);
        }
        else if (userResponse == 't')
        {
           genTick(gameBoard, wrapping);
        }
        else if (userResponse == 'q') notDone = false;
    }
    //Once user has finished, we print our final statement
    cout << "Have a nice Life!" << endl;
    getLine("Press [Enter] to close the window... ");
    return 0;
}

void printGameBoard(Grid<char>& inGrid)
{
    for (int i = 0; i < inGrid.numRows(); i++)
    {
        for (int j = 0; j < inGrid.numCols(); j++)
        {
            cout << inGrid[i][j];
        }
        cout << endl;
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
    printGameBoard(inGrid);
}

void genAnimate(Grid<char>& inGrid, int numFrames, bool& wrap)
{
    cout << "(" + integerToString(numFrames) + " new generations are shown, "
         << "with screen clear and 50ms pause before each)" << endl;
    for (int i = 0; i < numFrames; i++)
    {
        clearConsole();
        cout << "==================== (console cleared) ====================" << endl;
        genTick(inGrid, wrap);
        pause(50);
    }
}
