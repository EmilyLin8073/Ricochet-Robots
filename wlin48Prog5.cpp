/*-------------------------------------------------------------------------------------
 RicoRobotsSample.cpp
 Play the game of Ricochet Robots, similar to that seen online at:
 http://www.robotreboot.com/challenge

 Program 5
 Class: CS 141, Spring 2018
 Lab: Englebert Humberdink, Mon. 5:00 AM
 System: Xcode on an iMac

 @author  Emily Lin
 @version March 22, 2018

 There are a few tricky aspects to keep track of:
 1. A robot cannot move ontop of another robot.
 2. A robot (numbered 1..4) that is not the goal robot can move on top of the destination
 letter.  If it does, then the letter disappears.  Once non-goal-robot moves off,
 then the goal letter must reappear.

 -------------------------------------------------------------------------------------*/


#include <iostream>
#include <cstring>      // for getline
#include <cstdlib>      // for the system command
#include <cctype>       // For the letter checking functions, e.g. toupper( )
#include <fstream>      // For file input and output
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
using namespace std;

// Global data structure for board
const int BOARD_EDGE = 16;
const int BOARD_SIZE = BOARD_EDGE * BOARD_EDGE;
const int NUMBER_OF_ROBOTS = 4;
// declare constants to later use instead of numbers to select values from theBoard
const int NUMBER_OF_ELEMENTS = 5;   // Number of elements for each piece, enumerated below
const int PIECE = 0;    // to reference the character, such as in: theBoard[ i][ PIECE]
const int LEFT = 1;     // to reference the left wall character, such as in: theBoard[ i][ LEFT]
const int ABOVE = 2;    // to reference the above wall character, such as in: theBoard[ i][ ABOVE]
const int RIGHT = 3;    // to reference the right wall character, such as in: theBoard[ i][ RIGHT]
const int BELOW = 4;    // to reference the below wall character, such as in: theBoard[ i][ BELOW]


//All function header
void playGame(char destinationLetter, int goalRobot,
              char theBoard[ BOARD_SIZE][ NUMBER_OF_ELEMENTS],
              int theRobots[ NUMBER_OF_ROBOTS + 1], int letterIndex, char userInput);

void moveUp(char userInputRobot, char theBoard[BOARD_SIZE][NUMBER_OF_ELEMENTS],
            int theRobots[NUMBER_OF_ROBOTS + 1], int robotIndex, char destinationLetter,
            int goalRobot, int trackUserInputRobotIndex, int letterIndex);

void moveDown(char userInputRobot, char theBoard[BOARD_SIZE][NUMBER_OF_ELEMENTS],
             int theRobots[NUMBER_OF_ROBOTS + 1], int robotIndex, char destinationLetter,
              int goalRobot, int trackUserInputRobotIndex, int letterIndex);

void moveRight(char userInputRobot, char theBoard[BOARD_SIZE][NUMBER_OF_ELEMENTS],
              int theRobots[NUMBER_OF_ROBOTS + 1], int robotIndex, char destinationLetter,
               int goalRobot, int trackUserInputRobotIndex, int letterIndex);

void moveLeft(char userInputRobot, char theBoard[BOARD_SIZE][NUMBER_OF_ELEMENTS],
              int theRobots[NUMBER_OF_ROBOTS + 1], int robotIndex, char destinationLetter,
              int goalRobot, int trackUserInputRobotIndex, int letterIndex);


//-------------------------------------------------------------------------------------
void displayIdentifyingInformationAndInstructions()
{
   // Display identifying information
   cout << "Author: Emily Lin               " << endl
   << "Program 5: RicoRobots in C++         " << endl
   << "TA: Billie Joe Armstrong, T 6:00 AM  " << endl
   << "March 22, 2018                       " << endl;

   // Display Instructions
   cout << "Welcome to Ricochet Robots                                " << endl
   << "(\'Ricochet Robot\' is a registered trademark of Hans im Glück " << endl
   << "Munich, Germany, 1999. The game was created by Alex Randolph.) " << endl
   << endl
   << "The object of the game is to get the designated numbered robot " << endl
   << "to the indicated letter position.                              " << endl
   << "The numbered squares are all robots.  They unfortunately have  " << endl
   << "no brakes, and so will continue in whatever direction they are " << endl
   << "moving until they encountar an obstacle.                       " << endl
   << "For each move enter the robot number and the desired direction." << endl
   << "For instance entering                                          " << endl
   << "   1 U " << endl
   << "would move the #1 robot up as far as it can go.                " << endl
   << "The first letter of input is the robot number (1 - 4),         " << endl
   << "and the second letter is the direction:                        " << endl
   << "  (L=left, U=up, R=right, D=down)                              " << endl
   << "Enter x to exit.  Have fun!                                    " << endl
   <<  endl;
}//end displayIdentifyingInformationAndInstructions()


//-------------------------------------------------------------------------------------
// createBoard() - Create the board, initializing all the display characters
//
void createBoard(char theBoard[ BOARD_SIZE][ NUMBER_OF_ELEMENTS])
{
   for( int i=0; i<BOARD_SIZE; i++) {
      theBoard[ i][ PIECE]= '.';    // Set each playing piece default
      // set the default wall values to be blank
      theBoard[ i][ LEFT]= ' ';
      theBoard[ i][ ABOVE]= ' ';
      theBoard[ i][ RIGHT]= ' ';
      theBoard[ i][ BELOW]=  ' ';

      // reset the edge pieces
      // reset the left wall if piece is on left edge
      if( (i % BOARD_EDGE) == 0) {
         theBoard[ i][ LEFT]= '|';
      }
      // reset the above wall if piece is on top edge
      if( i < BOARD_EDGE) {
         theBoard[ i][ ABOVE]= '-';
      }
      // reset the right wall if piece is on right edge
      if( ((i+1) % BOARD_EDGE) == 0) {
         theBoard[ i][ RIGHT]= '|';
      }
      // reset the below wall if piece is on bottom edge
      if( i >= (BOARD_SIZE - BOARD_EDGE) ) {
         theBoard[ i][ BELOW]= '-';
      }
   }//end for( int i...

}//end createBoard()


//-------------------------------------------------------------------------------------
// readFromFileAndModifyBoard() - Read from data file, updating board accordingly
//
void readFromFileAndModifyBoard(char theBoard[ BOARD_SIZE][ NUMBER_OF_ELEMENTS],
                                int theRobots[ ], int &numberOfDestinationPieces)
{
   int numberOfSpecialSquares;  // Read from file, denotes number of pieces with some kind of wall
   string inputLine;            // Input line from file
   int pieceNumber;             // pieceNumber for each square defined in data file
   char c;

   ifstream inStream;           // declare an input file stream
   inStream.open("board.txt");  // Open input file, associating the actual file name with "inStream"
   if ( inStream.fail() ) {
      cout << "Input file opening failed.  Exiting...\n\n";
      exit(-1);
   }

   // Read from file one line at a time.  First read the top three data file lines which are documentation.
   getline( inStream, inputLine);
   getline( inStream, inputLine);
   getline( inStream, inputLine);

   inStream >> numberOfDestinationPieces;      // read how many destination pieces there are
   inStream >> numberOfSpecialSquares;         // number of square with walls around them somewhere
   inStream.get( c);                           // Get rid of return character at end of current line
   // to get ready for reading a line at a time below

   // process the special squares, updating the board
   for( int i=0; i<numberOfSpecialSquares; i++) {
      getline( inStream, inputLine);
      char *pInputLine = &inputLine[0];
      // Extract the piece number from the input line array
      sscanf( pInputLine, "%d", &pieceNumber);
      // Advance the inputLine pointer to the space after the number
      pInputLine = strchr( pInputLine, ' ');

      // Read the information on whether or not there is each of the potential 4 walls.
      // The four values read correspond in order to the pieces on all four sides,
      // which are defined as global constants for the values LEFT=1, ABOVE=2, RIGHT=3, BELOW=4
      for( int position=1; position<=4; position++) {
         pInputLine++;   // advance to next character to be read
         sscanf( pInputLine, " %c", &c);         // Read the next potential wall character
         // Convert it to a blank if it was '0'.  '0' was left in input file to make it easier to read.
         if( c=='0') {
            c = ' ';
         }
         theBoard[ pieceNumber][ position] = c;
      }
      pInputLine++;

      // See if there is a piece letter still to be read on this input line.
      // Do this by finding the end of the line, and seeing the length of the string
      // after that.
      // An inputline sample for piece 18 would be:
      // 18 00|- A
      if( strlen( pInputLine) > 0) {
         sscanf( pInputLine, " %c", &theBoard[ pieceNumber][ PIECE]);
      }

   }//end for( int i...

   // At the end of the data file read the information on the location of the robots, updating the board.
   char junk[80];           // used to read and discard input file information
   // Loop starts counting from 1 (rather than 0), since we ignore the 0th position
   // of the robots array, so that robot numbers correspond to what is shown
   // on the board.
   for( int i=1; i<=NUMBER_OF_ROBOTS; i++) {
      inStream >> theRobots[ i];
      inStream.getline( junk, '\n');    // discard rest of line, which is color information, if using graphics
      // set robot on board, converting the integer value to a character
      theBoard[ theRobots[ i]][ PIECE] = (char)('0'+i);     // record the robot number in place on board
   }

   inStream.close();         // close the input file stream
}//end readFromFileAndModifyBoard()


//-------------------------------------------------------------------------------------
// Display the Board
//
void displayTheBoard( char theBoard[ BOARD_SIZE][ NUMBER_OF_ELEMENTS])
{
   // display the top edge
   cout << "  ---------------------------------------------------------------- " << endl;

   // display the "body" of the board
   for( int i=0; i<BOARD_SIZE; i++) {
      // Figure out what character should be displayed to the left.  It will
      // be a wall if the current spot has a left wall, or if the spot to the
      // left has a right wall.

      char leftCharacter = theBoard[ i][ LEFT];   // set left display char
      // See if the piece to the left has a right wall.  Don't do this for the
      // first piece on the board, since it has no left neighbor.
      if ( (i>0) && (theBoard[ i-1][ RIGHT] != ' ')) {
         leftCharacter = theBoard[ i-1][ RIGHT];
      }
      cout << " " << leftCharacter << " " << theBoard[ i][ PIECE];
      // see if we're at the end of a row
      if( ((i+1) % BOARD_EDGE) == 0) {
         // we are at the end of a row, so display right wall and go to next line
         cout << "  " << theBoard[ i][ RIGHT] << endl;
         // Now display any walls immediately below the line of pieces just displayed
         // Backup our index counter j to the beginning of this line again, to find any
         // walls displayed below this line.  Don't do it for the bottom row though.
         if( i < BOARD_SIZE - BOARD_EDGE) {

            cout << " |";       // display the left boundary
            for( int j=i-BOARD_EDGE+1; j<(i+1); j++) {
               // Set the character to be displayed.  This is a wall if the
               // current spot has a wall below, or if the spot below has a wall
               // above.
               char belowCharacter = theBoard[ j][ BELOW];
               // Only check the square below if we're NOT on the bottom row
               if ( (j<(BOARD_SIZE - BOARD_EDGE)) &&      // verify not on bottom row
                   (theBoard[ j+16][ ABOVE] != ' ')) {    // piece below has wall above
                  belowCharacter = theBoard[ j+16][ ABOVE];
               }
               for( int i=0; i<3; i++) {
                  cout << belowCharacter;        // display the character
               }
               // display extra spaces, if we're not at the end of a row
               if( ((j+1) % BOARD_EDGE) != 0) {
                  cout << " ";
               }
            }//end for( int j...
            cout << " |" << endl;       // display the right boundary
         }//end if( i< BOARD_SIZE...

      }//end if( ((i+1...

   }//end for( int i=0;...

   // display the bottom edge
   cout << "  ---------------------------------------------------------------- " << endl;
   cout << endl;
}//end displayTheBoard()


void readInUserInputAndPlay(char theBoard[ BOARD_SIZE][ NUMBER_OF_ELEMENTS],
                            int theRobots[ NUMBER_OF_ROBOTS + 1])
{
   char userInput = ' ';
   int letterIndex = 0;

   char destinationLetter = ' ';
   int goalRobot = 0;

   while(true)
   {
      cout << "Enter 'r' for random robot and goal, 'd' for default or 's' to select: ";
      cin >> userInput;

      userInput = toupper(userInput);

      if(userInput == 'D')
      {
         destinationLetter = 'M';
         goalRobot = 2;

         break;
      }
      else if(userInput == 'S')
      {
         displayTheBoard( theBoard);
         cout << "Enter the goal robot number and the destination letter: ";
         cin >> goalRobot >> destinationLetter;

         if(goalRobot > 4 || goalRobot < 1 || destinationLetter > 111)
         {
            cout << "*** Error, invalid input. Retry" << endl;
            continue;
         }
         break;
      }
      else if(userInput == 'R')
      {
         char letters[] = "ABCDEFGHIJKLMNO";
         destinationLetter = letters[rand() % 15];

         goalRobot = (rand() % 4) + 1;
         break;
      }
      else
      {
         cout << "Thank you for playing" << endl;
         break;
      }
   }

   destinationLetter = toupper(destinationLetter);

   for(int i = 0; i < BOARD_SIZE; i++)
   {
      if(theBoard[i][PIECE] != destinationLetter &&
         theBoard[i][PIECE] != ('0' + 1) &&
         theBoard[i][PIECE] != ('0' + 2) &&
         theBoard[i][PIECE] != ('0' + 3) &&
         theBoard[i][PIECE] != ('0' + 4))
      {
         theBoard[i][PIECE] = '.';
      }
   }

   for(int i = 0;i < BOARD_SIZE; i++)
   {
      if(theBoard[i][PIECE] != destinationLetter)
         letterIndex++;

      else
         break;
   }

   playGame(destinationLetter, goalRobot, theBoard, theRobots, letterIndex, userInput);
}


void playGame(char destinationLetter, int goalRobot, char theBoard[ BOARD_SIZE][ NUMBER_OF_ELEMENTS],
              int theRobots[ NUMBER_OF_ROBOTS + 1], int letterIndex, char userInput)
{
   char userInputRobot;
   char userInputDireciton;
   int robotIndex = 0;
   int playCount = 1;
   int trackGoalRobotIndex = 0;
   int trackUserInputRobotIndex = 0;

   cout << letterIndex << endl << endl;;

   while(userInput != 'X')
   {
      cout << "Move robot " << goalRobot<< " to square " << destinationLetter << endl;
      displayTheBoard( theBoard);

      cout << playCount << ". Please enter the robot to move and the direction (e.g. 2 r): ";
      cin >> userInputRobot;
      userInputRobot = toupper(userInputRobot);

      if(userInputRobot == 'X')
      {
         cout << "Thank you for playing" << endl;
         break;
      }
      else
      {
         cin >> userInputDireciton;
         userInputDireciton = toupper(userInputDireciton);
      }

      if(userInputRobot > 52 || userInputRobot < 49)
      {
         cout << "*** Error, invalid robot. Retry" << endl;
         continue;
      }

      //      if(userInputDireciton != 'U' || userInputDireciton != 'D' || userInputDireciton != 'R' || userInputDireciton != 'L')
      //      {
      //         cout << "*** Error, invalid direction. Retry" << endl;
      //         continue;
      //      }

      switch(userInputRobot)
      {
         case '1' :
            userInputRobot = (int)1;

            switch(userInputDireciton)
         {
            case 'U' :
               moveUp(userInputRobot, theBoard,theRobots, robotIndex, destinationLetter,
                      goalRobot, trackUserInputRobotIndex, letterIndex);
               break;

            case 'D' :
               moveDown(userInputRobot, theBoard,theRobots, robotIndex, destinationLetter,
                        goalRobot, trackUserInputRobotIndex, letterIndex);
               break;

            case 'R' :
               moveRight(userInputRobot, theBoard,theRobots, robotIndex, destinationLetter,
                         goalRobot, trackUserInputRobotIndex, letterIndex);
               break;

            case 'L' :
               moveLeft(userInputRobot, theBoard,theRobots, robotIndex, destinationLetter,
                        goalRobot, trackUserInputRobotIndex, letterIndex);
               break;
         }
            break;

         case '2' :
            userInputRobot = (int)2;

            switch(userInputDireciton)
         {
            case 'U' :
               moveUp(userInputRobot, theBoard,theRobots, robotIndex, destinationLetter,
                      goalRobot, trackUserInputRobotIndex, letterIndex);
               break;

            case 'D' :
               moveDown(userInputRobot, theBoard,theRobots, robotIndex, destinationLetter,
                        goalRobot, trackUserInputRobotIndex, letterIndex);
               break;

            case 'R' :
               moveRight(userInputRobot, theBoard,theRobots, robotIndex, destinationLetter,
                         goalRobot, trackUserInputRobotIndex, letterIndex);
               break;

            case 'L' :
               moveLeft(userInputRobot, theBoard,theRobots, robotIndex, destinationLetter,
                        goalRobot, trackUserInputRobotIndex, letterIndex);
               break;
         }
            break;

         case '3' :
            userInputRobot = (int)3;

            switch(userInputDireciton)
         {
            case 'U' :
               moveUp(userInputRobot, theBoard,theRobots, robotIndex, destinationLetter,
                      goalRobot, trackUserInputRobotIndex, letterIndex);
               break;

            case 'D' :
               moveDown(userInputRobot, theBoard,theRobots, robotIndex, destinationLetter,
                        goalRobot, trackUserInputRobotIndex, letterIndex);
               break;

            case 'R' :
               moveRight(userInputRobot, theBoard,theRobots, robotIndex, destinationLetter,
                         goalRobot, trackUserInputRobotIndex, letterIndex);
               break;

            case 'L' :
               moveLeft(userInputRobot, theBoard,theRobots, robotIndex, destinationLetter,
                        goalRobot, trackUserInputRobotIndex, letterIndex);
               break;
         }
            break;

         case '4' :
            userInputRobot = 4;

            switch(userInputDireciton)
         {
            case 'U' :
               moveUp(userInputRobot, theBoard,theRobots, robotIndex, destinationLetter,
                      goalRobot, trackUserInputRobotIndex, letterIndex);
               break;

            case 'D' :
               moveDown(userInputRobot, theBoard,theRobots, robotIndex, destinationLetter,
                        goalRobot, trackUserInputRobotIndex, letterIndex);
               break;

            case 'R' :
               moveRight(userInputRobot, theBoard,theRobots, robotIndex, destinationLetter,
                         goalRobot, trackUserInputRobotIndex, letterIndex);
               break;

            case 'L' :
               moveLeft(userInputRobot, theBoard,theRobots, robotIndex, destinationLetter,
                        goalRobot, trackUserInputRobotIndex, letterIndex);
               break;
         }
            break;
      }

      playCount++;

      displayTheBoard(theBoard);
      
      trackGoalRobotIndex = theRobots[goalRobot];
      trackUserInputRobotIndex = theRobots[userInputRobot];
      
      if(trackGoalRobotIndex == letterIndex)
      {
         cout << "Congras, you did it in only " << playCount << " move" << endl;
         break;
      }
   }
}


void moveUp(char userInputRobot, char theBoard[BOARD_SIZE][NUMBER_OF_ELEMENTS],
            int theRobots[NUMBER_OF_ROBOTS + 1], int robotIndex, char destinationLetter,
            int goalRobot, int trackUserInputRobotIndex, int letterIndex)
{
   robotIndex = theRobots[userInputRobot];

   theBoard[robotIndex][PIECE] = '.';

   if((userInputRobot != goalRobot) && trackUserInputRobotIndex == letterIndex)
   {
      theBoard[letterIndex][PIECE] = destinationLetter;
   }

   for(int i = robotIndex; i > 0; i = i - 16)
   {
      if(theBoard[i][ABOVE] != '-' && theBoard[i - 16][BELOW] != '-')
      {
         theRobots[userInputRobot] = i - 16;
      }
      else
      {
         theRobots[userInputRobot] = i;
         break;
      }

      if(theBoard[i - 16][PIECE] != ('0' + 1) &&
         theBoard[i - 16][PIECE] != ('0' + 2) &&
         theBoard[i - 16][PIECE] != ('0' + 3) &&
         theBoard[i - 16][PIECE] != ('0' + 4))
      {
         theRobots[userInputRobot] = i + 16;
      }
      else
      {
         theRobots[userInputRobot] = i;
         break;
      }
   }

   robotIndex = theRobots[userInputRobot];

   theBoard[robotIndex][PIECE] = '0' + userInputRobot;
}


void moveDown(char userInputRobot, char theBoard[BOARD_SIZE][NUMBER_OF_ELEMENTS],
              int theRobots[NUMBER_OF_ROBOTS + 1], int robotIndex, char destinationLetter,
              int goalRobot, int trackUserInputRobotIndex, int letterIndex)
{
   robotIndex = theRobots[userInputRobot];

   theBoard[ robotIndex][ PIECE] = '.';

   if((userInputRobot != goalRobot) && trackUserInputRobotIndex == letterIndex)
   {
      theBoard[letterIndex][PIECE] = destinationLetter;
   }

   for(int i = robotIndex; i > 0; i = i + 16)
   {
      if(theBoard[i][BELOW] != '-' && theBoard[i + 16][ABOVE] != '-')
      {
         theRobots[userInputRobot] = i + 16;
      }
      else
      {
         theRobots[userInputRobot] = i;
         break;
      }

      if(theBoard[i + 16][PIECE] != ('0' + 1) &&
         theBoard[i + 16][PIECE] != ('0' + 2) &&
         theBoard[i + 16][PIECE] != ('0' + 3) &&
         theBoard[i + 16][PIECE] != ('0' + 4))
      {
         theRobots[userInputRobot] = i + 16;
      }
      else
      {
         theRobots[userInputRobot] = i;
         break;
      }
   }

   robotIndex = theRobots[userInputRobot];

   theBoard[robotIndex][PIECE] = '0' + userInputRobot;
}



void moveRight(char userInputRobot, char theBoard[BOARD_SIZE][NUMBER_OF_ELEMENTS],
               int theRobots[NUMBER_OF_ROBOTS + 1], int robotIndex, char destinationLetter,
               int goalRobot, int trackUserInputRobotIndex, int letterIndex)
{
   robotIndex = theRobots[userInputRobot];

   theBoard[ robotIndex][ PIECE] = '.';

   if((userInputRobot != goalRobot) && trackUserInputRobotIndex == letterIndex)
   {
      theBoard[letterIndex][PIECE] = destinationLetter;
   }

   for(int i = robotIndex; i > 0; i = i + 1)
   {
      if(theBoard[i][RIGHT] != '|' && theBoard[i + 1][LEFT] != '|')
      {
         theRobots[userInputRobot] = i + 1;
      }
      else
      {
         theRobots[userInputRobot] = i;
         break;
      }

      if(theBoard[i + 1][PIECE] != ('0' + 1) &&
         theBoard[i + 1][PIECE] != ('0' + 2) &&
         theBoard[i + 1][PIECE] != ('0' + 3) &&
         theBoard[i + 1][PIECE] != ('0' + 4))
      {
         theRobots[userInputRobot] = i + 16;
      }
      else
      {
         theRobots[userInputRobot] = i;
         break;
      }
   }

   robotIndex = theRobots[userInputRobot];

   theBoard[robotIndex][PIECE] = '0' + userInputRobot;

}


void moveLeft(char userInputRobot, char theBoard[BOARD_SIZE][NUMBER_OF_ELEMENTS],
              int theRobots[NUMBER_OF_ROBOTS + 1], int robotIndex, char destinationLetter,
              int goalRobot, int trackUserInputRobotIndex, int letterIndex)
{
   robotIndex = theRobots[userInputRobot];

   theBoard[ robotIndex][ PIECE] = '.';

   if((userInputRobot != goalRobot) && trackUserInputRobotIndex == letterIndex)
   {
      theBoard[letterIndex][PIECE] = destinationLetter;
   }

   for(int i = robotIndex; i > 0; i = i - 1)
   {
      if(theBoard[i][LEFT] != '|' && theBoard[i - 1][RIGHT] != '|')
      {
         theRobots[userInputRobot] = i - 1;
      }
      else
      {
         theRobots[userInputRobot] = i;
         break;
      }

      if(theBoard[i - 1][PIECE] != ('0' + 1) &&
         theBoard[i - 1][PIECE] != ('0' + 2) &&
         theBoard[i - 1][PIECE] != ('0' + 3) &&
         theBoard[i - 1][PIECE] != ('0' + 4))
      {
         theRobots[userInputRobot] = i + 16;
      }
      else
      {
         theRobots[userInputRobot] = i;
         break;
      }
   }

   robotIndex = theRobots[userInputRobot];

   theBoard[robotIndex][PIECE] = '0' + userInputRobot;

}


//-------------------------------------------------------------------------------------
// main() - main part of program, that drives everything else
//
int main()
{
   // theBoard itself is represented by a one-dimensional array.
   // The other entries are used to keep track of walls.  Each row
   // in theBoard array will contain the character to be displayed,
   // followed by any "walls" to be displayed around the displayed
   // character.  For instance the first couple of entries are be:
   //
   //       PIECE   LEFT ABOVE RIGHT BELOW
   //      ---0---  --1-  --2-  --3-  --4-
   //    0    .       |     _
   //    1    .             _
   //   ...
   //   255   .                   |     _
   //
   char theBoard[ BOARD_SIZE][ NUMBER_OF_ELEMENTS];
   // Declare the 4 robots, which will store the board position of where they are found
   // Robots are displayed using values 1-4.  We declare 5 robots, though we will never use
   // the 0th robot in the array, so that we can use the robot digit itself as the index.
   int theRobots[ NUMBER_OF_ROBOTS + 1] = {0,0,0,0,0};

//   int goalRobot;                  // the robot that needs to reach the goal
//   char destinationLetter;         // the goal letter
   int numberOfDestinationPieces;  // Number of destination pieces.  Value comes from the data file.

   srand (time(NULL));

   displayIdentifyingInformationAndInstructions();
   createBoard(theBoard);   // create the board, initializing all pieces to their default settings
   readFromFileAndModifyBoard(theBoard, theRobots, numberOfDestinationPieces); // read from the datafile, updating board

   readInUserInputAndPlay(theBoard, theRobots);


}//end main()






