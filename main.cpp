//  prog5_1024.cpp
//     Text and graphical version of the game 1024, a variant of 2048 available online at:
//        http://gabrielecirulli.github.io/2048/
//     Object of game is to combine number tiles with the same value, accumulating
//     points as the game progresses, to try and create the tile with the value 1024.
//     Each move consists of sliding pieces to the left, up, right or down.
// 
// Instructions:
//    To run the graphical version, first select the "Build and Run" option at the top of the window. You
//    can ignore the following error messages that will appear:
//         Failed to use the XRandR extension while trying to get the desktop video modes
//         Failed to use the XRandR extension while trying to get the desktop video modes
//         Failed to initialize inotify, joystick connections and disconnections won't be notified
//    To see the graphical output then select the "Viewer" option at the top of the window.
//    
// For more information about SFML graphics, see: https://www.sfml-dev.org/tutorials
// Be sure to close the old window each time you rebuild and rerun, to ensure you
// are seeing the latest output.
//
//
/*  Running the program (for the textual output) looks like:

*/
  
#include <SFML/Graphics.hpp> // Needed to access all the SFML graphics libraries
#include <iostream>          // Since we are using multiple libraries, now use std::
                             // in front of every cin, cout, endl, setw, and string 
#include <iomanip>           // used for setting output field size using setw
#include <cstdio>            // For sprintf, "printing" to a string
#include <cstring>           // For c-string functions such as strlen()  
#include <chrono>            // Used in pausing for some milliseconds using sleep_for(...)
#include <thread>            // Used in pausing for some milliseconds using sleep_for(...)

const int WindowXSize = 400;
const int WindowYSize = 500;
const int MaxBoardSize = 12;  // Max number of squares per side


//---------------------------------------------------------------------------------------
class Square {
	public:
		// Default Constructor 
		Square()
		{
			size = 0;
			xPosition = 0;
			yPosition = 0;
			color = sf::Color::Black;
			isVisible = false;
			isCaptured = false;
			text = "";		
		}
	
		// Fully-qualified constructor, used to set all fields
		Square( int theSize, int theXPosition, int theYPosition, 
			     const sf::Color &theColor, bool theVisibility, std::string theText)
		{
			// Set the class fields
			size = theSize;
			xPosition = theXPosition;
			yPosition = theYPosition;
			color = theColor;
			isVisible = theVisibility;
			isCaptured = false;   // By default squares have not been captured
			text = theText;
			// Use the values to set the display characteristics of theSquare
			theSquare.setSize( sf::Vector2f(theSize, theSize));
			theSquare.setPosition( theXPosition, theYPosition);   // Set the position of the square
			theSquare.setFillColor( theColor);
		}
			
		// Get (accessor) functions
		sf::RectangleShape getTheSquare() { return theSquare; }
		int getSize() { return size; }
		int getXPosition() { return xPosition; }
		int getYPosition() { return yPosition; }
		sf::Color& getColor() { return color; }
		bool getIsVisible() { return isVisible; }
		bool getIsCaptured() { return isCaptured; }
		std::string getText() { return text; }
	
		// Set (mutator) functions
		void setSize( int theSize) { 
			size = theSize; 
			theSquare.setSize( sf::Vector2f(theSize, theSize));
		}
		void setXPosition( int theXPosition) { 
			xPosition = theXPosition; 
			theSquare.setPosition( theXPosition, yPosition);   // Set the position of the square
		}
		void setYPosition( int theYPosition) { 
			yPosition = theYPosition; 
			theSquare.setPosition( xPosition, theYPosition);   // Set the position of the square
		}
		void setColor( sf::Color & theColor) { 
			color = theColor; 
			theSquare.setFillColor( theColor);    // Also update the color on the square itself
		}
		void setColor( int R, int G, int B) {
			sf::Color theNewColor( R, G, B);
			color = theNewColor;
			theSquare.setFillColor( theNewColor);
		}
		void setVisibility( bool theVisibility) { isVisible = theVisibility; }
		void setIsCaptured( bool isCaptured) { this->isCaptured = isCaptured; }
		void setText( std::string theText) { text = theText; }

		// Utility functions
		void displayText( sf::RenderWindow *pWindow, sf::Font theFont, sf::Color theColor, int textSize);
	
	private:
		int size;
		int xPosition;
		int yPosition;
		sf::Color color;
		bool isVisible;
		bool isCaptured;   // Indicates whether or not it is part of the captured area
		std::string text;
		sf::RectangleShape theSquare;

}; //end class Square


//---------------------------------------------------------------------------------------
// Square class utility function to create a sf::Text object to store and display text 
// associated with this Square.
// 
// Assuming we display output in sf::RenderWindow window(...), then call this function using: 
//    aSquare.displayTest( &window);
// or when using an array of Square pointers declared as:  Square *squaresArray[ 4];
// then call it using:  squaresArray[i]->displayText( &window);
void Square::displayText( 
		sf::RenderWindow *pWindow,   // The window into which we draw everything
		sf::Font theFont,            // Font to be used in displaying text   
		sf::Color theColor,          // Color of the font
		int textSize)                // Size of the text to be displayed
{	
	// Create a sf::Text object to draw the text, using a sf::Text constructor
	sf::Text theText( text,        // text is a class data member
					  theFont,     // font from a font file, passed as a parameter
					  textSize);   // this is the size of text to be displayed

	// Text color is the designated one, unless the background is Yellow, in which case the text
	// color gets changed to blue so we can see it, since we can't see white-on-yellow very well
	if( this->getColor() == sf::Color::Yellow) {
		theColor = sf::Color::Blue;
	}
	theText.setColor( theColor);

	// Place text in the corresponding square, centered in both x (horizontally) and y (vertically)
	// For horizontal center, find the center of the square and subtract half the width of the text 
	int theXPosition = xPosition + (size / 2) - ((strlen(text.c_str()) * theText.getCharacterSize()) / 2);
	// For the vertical center, from the top of the square go down the amount: (square size - text size) / 2
	int theYPosition = yPosition + (size - theText.getCharacterSize()) / 2;
	// Use an additional offset to get it centered
	int offset = 5;
	theText.setPosition( theXPosition + offset, theYPosition - offset);

	// Finally draw the Text object in the RenderWindow
	pWindow->draw( theText);
}


//---------------------------------------------------------------------------------------
// Initialize the font
void initializeFont( sf::Font &theFont)
{
	// Create the global font object from the font file
	if (!theFont.loadFromFile("arial.ttf"))
	{
		std::cout << "Unable to load font. " << std::endl;
		exit( -1);
	}	
}


//--------------------------------------------------------------------
// Display Instructions
void displayInstructions()
{
    std::cout << "Welcome to 1024. \n"
			  << "  \n"
			  << "For each move enter a direction as a letter key, as follows: \n"
			  << "    W    \n"
			  << "  A S D  \n"
			  << "where A=left,W=up, D=right and S=down. \n"
			  << "  \n"
			  << "After a move, when two identical valued tiles come together they    \n"
			  << "join to become a new single tile with the value of the sum of the   \n"
			  << "two originals. This value gets added to the score.  On each move    \n"
			  << "one new randomly chosen value of 2 or 4 is placed in a random open  \n"
			  << "square.  User input of x exits the game.                            \n"
			  << "  \n";
}//end displayInstructions()


//--------------------------------------------------------------------
// Place a randomly selected 2 or 4 into a random open square on
// the board.
void placeRandomPiece( int board[], int squaresPerSide)
{
    // Randomly choose a piece to be placed (2 or 4)
    int pieceToPlace = 2;
    if( rand()%2 == 1) {
        pieceToPlace = 4;
    }
    
    // Find an unoccupied square that currently has a 0
    int index;
    do {
        index = rand() % (squaresPerSide*squaresPerSide);
    }while( board[ index] != 0);
    
    // board at position index is blank, so place piece there
    board[ index] = pieceToPlace;
}//end placeRandomPiece()

//create the Node
struct Node{
    int board[MaxBoardSize*MaxBoardSize];
    int score;
    int move;
    Node *pNext;
};

//add the node for the next 
void addNode(int squaresPerSide, int board[], int move, int score, Node* &pHead){
    Node *pTemp = new Node;
    pTemp->score = score;
    pTemp->move = move;
    for ( int i = 0; i < squaresPerSide*squaresPerSide; i++){
        pTemp->board[i] = board[i];
    }
    pTemp->pNext = pHead;
    pHead = pTemp;
}

//this function is undo the board, score, and move.
void undoList(int squaresPerSide, int board[], int &move, int &score, Node* &pHead){
    Node *pTemp = pHead;   
	// Next line should be:  if( pTemp->pNext != NULL)
    if(pTemp->pNext != NULL){
        std::cout << "* Undoing move *" << std::endl;
        score = pTemp->pNext->score;
        move = pTemp->pNext->move;
        for (int i = 0; i < squaresPerSide*squaresPerSide; i ++){
            board[i] = pTemp->pNext->board[i];
        }
        pHead = pTemp->pNext;
        delete pTemp;
    }
    else{
        std::cout << "*** You cannot undo past the beginning of the game.  Please retry. ***" << std::endl;
    }
}


//this function is make the display list.
void displayList(Node *pTemp,int counter = 0){
    std::cout << "List: ";
    while (pTemp != NULL){
        std::cout << pTemp->move;
        if (pTemp->pNext != NULL){
            std::cout << "->";
        }
        pTemp = pTemp -> pNext;
    }
}

//get the index of the board;
int getIndex(int row, int col, int squaresPerSide){
    return row * squaresPerSide + col;
}

//reset the board and return every element into 0.
int resetBoard(int board[], int squaresPerSide){
    for (int i = 0 ; i < squaresPerSide*squaresPerSide; i++){
        board[i] = 0;
    }
}

//display the board to play game.
void displayBoardSize(int squaresPerSide, int board[], int score, Node* &pHead,int &counter){
    Node *pTemp = pHead;
    std::cout << std::endl;
    std::cout << "Score: " << score << std::endl;
    
    //print the board
    for( int i = 0; i < squaresPerSide*squaresPerSide; i++){
        if( i % squaresPerSide == 0){
            std:: cout << "\n";
            std:: cout << "\n";
        }
        if(board[i] ==0){
            std::cout << "\t" << ".";
        }
        else{
            std::cout << "\t" << board[i];
        }
    }
    std::cout << std::endl;
    displayList(pTemp,counter);        
    std::cout << std::endl;
}

//this function is copy anything in the board and put it in previousBoard.
void copyBoard(int board[], int previousBoard[], int squaresPerSide, int score){
    for ( int i = 0; i < squaresPerSide*squaresPerSide; i++){
        previousBoard[i] = board[i];
    }
}

//this funcion is checking if the board is change or not.
bool boardChanged(int board[],int previousBoard[],int squaresPerSide,int score){
    for ( int i = 0; i < squaresPerSide*squaresPerSide; i++){
        if( board[i] != previousBoard[i]) {
            return true;
        }
    }
    return false;
}

// make the goal for the board ( still not working)
int boardGoal(int squaresPerSide){
    int fomular;
    return fomular = 1024*pow(2,abs(squaresPerSide-4));    
}

//this function is checking if the board got max goal or not.
bool maxGoal(int board[],int squaresPerSide){
    int fomular;
    fomular = 1024*pow(2,abs(squaresPerSide-4));
    for ( int i = 0 ; i < squaresPerSide*squaresPerSide; i++){
        if( board[i] == fomular){
            return true;
        }
    }    
    return false;
}

//make the move in the board
void movePieces(int board[], char userInput,int &squaresPerSide, int &score, int &move, Node* &pHead){
    char destination;
    int number;
    int position;
    int row;
    int col;
    switch (userInput){
        //when user press u to undo what the make.
        case 'U':
            std::cout << std::endl;
            undoList( squaresPerSide, board, move, score, pHead);            
            break;
        //when user press X to quit and see the magic happend.
        case 'X':
            std::cout << std::endl;
            std::cout << "Thanks for playing. Exiting program... \n\n";
            exit( 0);
            break;
        //when user press P to put any value in the index on board.
        case 'P': 
            std::cin >> position;
            std::cin >> number;
            board[position] = number;
            break;
        //when user press R to reset the board.
        case 'R':
            std::cout << std::endl;
            std::cout << "Resetting board" << std::endl;
            std::cout << std::endl;
            std::cout << "Enter the size board you want, between 4 and 12: ";
            std::cin >> squaresPerSide;
            resetBoard(board,squaresPerSide);
            score = 0;
            move = 0;
            placeRandomPiece(board,squaresPerSide);
            std::cout << std::endl;
            std::cout << "Game ends when you reach " << boardGoal(squaresPerSide) << "." << std::endl;
            break;
        //when user press A to move Pieces in the left.
        case 'A':
            for( int i = 0; i < squaresPerSide; i++){
                bool notMove = true;
                for (int j = 0; j < squaresPerSide - 1; j++){                    
                    if((board[getIndex(i,j,squaresPerSide)] == 0)&&(board[getIndex(i,j+1,squaresPerSide)] != 0)){
                        notMove = false;
                        board[getIndex(i,j,squaresPerSide)] = board[getIndex(i,j+1,squaresPerSide)];
                        board[getIndex(i,j+1,squaresPerSide)] = 0;
                    }
                }
                
                if(notMove == false){
                    i--;
                }
            }
            break;
        //when user press W to move pieces over.
        case 'W':
            for( int j = 0; j < squaresPerSide; j++){
                bool notMove = true;
                for( int i = 0; i < squaresPerSide-1; i++){
                    if((board[getIndex(i,j,squaresPerSide)] == 0)&&(board[getIndex(i+1,j,squaresPerSide)] !=0)){
                        notMove = false;
                        board[getIndex(i,j,squaresPerSide)] = board[getIndex(i+1,j,squaresPerSide)];
                        board[getIndex(i+1,j,squaresPerSide)] = 0;
                    }
                }
                if(notMove == false){
                    j--;
                }
            }
            break;
        //when user press D to move pieces in the right.
        case 'D':
            for( int i = 0; i < squaresPerSide ; i++){
                bool notMove = true;
                for( int j = squaresPerSide-1; j > 0 ; j--){
                    if((board[getIndex(i,j-1,squaresPerSide)] != 0)&&(board[getIndex(i,j,squaresPerSide)] == 0)){
                        notMove = false;
                        board[getIndex(i,j,squaresPerSide)] = board[getIndex(i,j-1,squaresPerSide)];
                        board[getIndex(i,j-1,squaresPerSide)] = 0;
                    }
                }
                if(notMove == false){
                    i--;
                }
            }
            break;
        //when user press S to move pieces down.
        case 'S':
            for( int j = 0; j < squaresPerSide; j++){
                bool notMove = true;
                for( int i = squaresPerSide - 1; i > 0 ; i--){
                    if((board[getIndex(i-1,j,squaresPerSide)] != 0)&&(board[getIndex(i,j,squaresPerSide)] == 0)){
                        notMove = false;
                        board[getIndex(i,j,squaresPerSide)] = board[getIndex(i-1,j,squaresPerSide)];
                        board[getIndex(i-1,j,squaresPerSide)] = 0;
                    }
                }
                if(notMove == false){
                    j--;
                }
            }
            break;
    }
}

//make the combine near it if it's got the same value
void combine(int board[], char userInput,int &squaresPerSide, int &score, int &move, Node* &pHead){
    switch (userInput){
        case 'A':
            for( int i = 0; i < squaresPerSide; i++){
                for (int j = 0; j < squaresPerSide - 1; j++){
                    if((board[getIndex(i,j,squaresPerSide)] != 0)&&(board[getIndex(i,j+1,squaresPerSide)] != 0)){
                        if(board[getIndex(i,j,squaresPerSide)] == (board[getIndex(i,j+1,squaresPerSide)])){
                            board[getIndex(i,j,squaresPerSide)] *= 2;
                            board[getIndex(i,j+1,squaresPerSide)] = 0;
                            score += (board[getIndex(i,j,squaresPerSide)]);
                            movePieces(board,userInput,squaresPerSide,score, move, pHead);
                        }
                    }
                }
            }
            break;
        case 'W':
            for( int j = 0; j < squaresPerSide; j++){
                for( int i = 0; i < squaresPerSide-1; i++){
                    if((board[getIndex(i,j,squaresPerSide)] != 0)&&(board[getIndex(i+1,j,squaresPerSide)] != 0)){
                        if(board[getIndex(i,j,squaresPerSide)] == (board[getIndex(i+1,j,squaresPerSide)])){
                            board[getIndex(i,j,squaresPerSide)] *= 2;
                            board[getIndex(i+1,j,squaresPerSide)] = 0;
                            score += (board[getIndex(i,j,squaresPerSide)]);
                            movePieces(board,userInput,squaresPerSide,score,move, pHead);
                        }
                    }
                }
            }
            break;
        case 'D':
            for( int i = 0; i < squaresPerSide ; i++){
                for( int j = squaresPerSide-1; j > 0 ; j--){
                    if((board[getIndex(i,j-1,squaresPerSide)] != 0)&&(board[getIndex(i,j,squaresPerSide)] != 0)){
                        if(board[getIndex(i,j,squaresPerSide)] == (board[getIndex(i,j-1,squaresPerSide)])){
                            board[getIndex(i,j,squaresPerSide)] *= 2;
                            board[getIndex(i,j-1,squaresPerSide)] = 0;
                            score += board[getIndex(i,j,squaresPerSide)];
                            movePieces(board,userInput,squaresPerSide,score,move, pHead);
                        }
                    }
                }
            }
            break;
        case 'S':
            for( int j = 0; j < squaresPerSide; j++){
                for( int i = squaresPerSide - 1; i > 0 ; i--){
                    if((board[getIndex(i,j,squaresPerSide)] != 0)&&(board[getIndex(i-1,j,squaresPerSide)] != 0)){
                        if(board[getIndex(i,j,squaresPerSide)] == board[getIndex(i-1,j,squaresPerSide)]){
                            board[getIndex(i,j,squaresPerSide)] *= 2;
                            board[getIndex(i-1,j,squaresPerSide)] = 0;
                            score += board[getIndex(i,j,squaresPerSide)];
                            movePieces(board,userInput,squaresPerSide,score,move, pHead);
                        }
                    }
                }
            }
            break;
    }
}

//this function is checking if my board is full or not.
bool boardFull(int board[]){
    int row;
    int col;
    int squaresPerSide = 4;
    
    //checking if the board have empty space.
    for ( int i = 0; i < squaresPerSide*squaresPerSide; i++){
        if(board[i] == 0){
            return false;
        }
    }
        
    //checking if the collum can combine or not.
    for( int i = 0; i < squaresPerSide; i++){
        for (int j = 0; j < squaresPerSide - 1; j++){                    
            if(board[getIndex(i,j,squaresPerSide)] == board[getIndex(i,j+1,squaresPerSide)]){
                return false;
            }
        }
    }
    
    //checking if the row can combine or not.
    for (int j = 0; j < squaresPerSide; j++){   
        for( int i = 0; i < squaresPerSide-1; i++){          
            if(board[getIndex(i,j,squaresPerSide)] == board[getIndex(i+1,j,squaresPerSide)]){
                return false;
            }
        }
    }    
    return true;
}



//---------------------------------------------------------------------------------------
int main()
{	
    int move = 1;                     // User move counter
	int score = 0;                    // Cummulative score, which is sum of combined tiles
    int squaresPerSide = 4;           // User will enter this value.  Set default to 4
	int board[ MaxBoardSize * MaxBoardSize];          // space for largest possible board
    int previousBoard[ MaxBoardSize * MaxBoardSize];  // space for copy of board, used to see 
													  //    if a move changed the board.
    bool byPass = false; 
    
	// Create the graphical board, an array of Square objects set to be the max size it will ever be.
	Square squaresArray[ MaxBoardSize * MaxBoardSize]; 
    int maxTileValue = 1024;  // 1024 for 4x4 board, 2048 for 5x5, 4096 for 6x6, etc.
    char userInput = ' ';     // Stores user input
	char aString[ 81];        // C-string to hold concatenated output of character literals
    int newNode;
    int boardLine = 4;
    int counter;
    
	// Create the graphics window
	sf::RenderWindow window(sf::VideoMode(WindowXSize, WindowYSize), "Program 5: 1024");
	std::cout << std::endl;
    // Create and initialize the font, to be used in displaying text.
	sf::Font font;  
	initializeFont( font);
	// Create the messages label at the bottom of the graphics screen, for displaying debugging information
	sf::Text messagesLabel( "Welcome to 1024", font, 20);
	// Make a text object from the font
	messagesLabel.setColor( sf::Color(255,255,255));
	// Place text at the bottom of the window. Position offsets are x,y from 0,0 in upper-left of window
	messagesLabel.setPosition( 0, WindowYSize - messagesLabel.getCharacterSize() - 5); 
	
	displayInstructions();
        
    // Get the board size, create and initialize the board, and set the max tile value
    // ...
    std::cout << "Game ends when you reach " << boardGoal(squaresPerSide) << "." << std::endl;
    
    resetBoard(board,squaresPerSide);
    placeRandomPiece( board,  squaresPerSide);
    placeRandomPiece( board,  squaresPerSide);
    
	Node *pHead = NULL;
    // Declare a pointer for the head of the list.  Add a node onto the list.  
    //    The list may grow and shrink, but this first node should always be there.
    // ...
    
    
    int arraySize = squaresPerSide*squaresPerSide;
    
    window.display();
	
	// Add the first node to the list, capturing the starting board, score, and move number.
	// This node should always then be on the list.
	addNode(squaresPerSide, board, move, score, pHead); 
	
	// Run the program as long as the window is open.  This is known as the "Event loop".
	while (window.isOpen()&&(!boardFull(board))&&(!maxGoal(board,squaresPerSide) || byPass))
	{
        
        for(int i = 0; i < squaresPerSide; i++){
            for(int j = 0; j < squaresPerSide; j++){
                char nameBoard[81];
                if(board[i*squaresPerSide+j] == 0){
                    strcpy( nameBoard, "");
                }
                else{
                    sprintf( nameBoard,"%d", board[i*squaresPerSide+j]);
                }
                squaresArray[i*squaresPerSide+j] = Square(90,90 * j + j * 10, 90 * i + i * 10 , sf::Color::Blue, true, nameBoard);
                window.draw(squaresArray[i*squaresPerSide+j].getTheSquare());
                int red = 255, green = 255, blue = 255;
                squaresArray[i*squaresPerSide+j].displayText(&window, font, sf::Color(red,green,blue), 30);
            }
        }
        
        //
        window.clear();
        
        //this line for display the text
        for( int i = 0; i < squaresPerSide; i++){
            for(int j = 0; j < squaresPerSide; j++){
            window.draw(squaresArray[i*squaresPerSide+j].getTheSquare());
            int red=255,green=255,blue=255;
            squaresArray[i*squaresPerSide+j].displayText(&window,font,sf::Color(red,green,blue),30);
            }
        }
        sprintf( aString, "Move %d", move );
        messagesLabel.setString(aString);
        window.draw(messagesLabel);
        
        window.display();
        byPass = false;
		// Display both the graphical and text boards.
		// ...
		
        displayBoardSize(squaresPerSide,board,score,pHead,counter);
        // Make a copy of the board.  After we then attempt a move, the copy will be used to 
        // verify that the board changed, which only then allows randomly placing an additional  
        // piece on the board and updating the move number.
        // ...
        copyBoard(board,previousBoard, squaresPerSide, score);
        
        // Prompt for and handle user input
        std::cout << move << ". Your move: ";
        std::cin >> userInput;
        userInput = toupper(userInput);
		
		// Prompt for and get the user input, and handle the different user inputs
        movePieces(board, userInput,squaresPerSide, score, move, pHead);
		
		// If user input is 'U', then undo move, and continue back up to top of loop.
		// ...
		
        combine(board, userInput, squaresPerSide, score, move, pHead);
        
        // If the move resulted in pieces changing position, then it was a valid move
        // so place a new random piece (2 or 4) in a random open square and update move number.
        // Add the new board, moveNumber and score to a new list node at the front of the list.
        // ...
        if ((userInput !='U')&&(userInput != 'P') && boardChanged(board,previousBoard,squaresPerSide,score) == true){
            placeRandomPiece( board,  squaresPerSide);
            move++;
            addNode(squaresPerSide, board, move, score, pHead);  
        }
        if(userInput == 'P'){
            byPass = true;
        }
		// See if we're done
		// ...

// 		system("clear");   // Clear the screen 

		// Pause the event loop, so that Codio does not think it is a runaway process and kill it after some time
		std::this_thread::sleep_for(std::chrono::milliseconds( 50));
	}//end while( window.isOpen())
    
//when the board is full or user got max Goal the game will break.
if(boardFull(board)){
    displayBoardSize(squaresPerSide,board,score,pHead,counter);
    std::cout << move << ". Your move: ";
    std::cout << "No more available moves. Game is over." << std::endl;
    displayBoardSize(squaresPerSide,board,score,pHead,counter);
}
else if(maxGoal(board,squaresPerSide)){
    std::cout <<  std::endl;
    std::cout << "Congratulations!  You made it to " << boardGoal(squaresPerSide) << " !!!" << std::endl;
    displayBoardSize(squaresPerSide,board,score,pHead,counter);
}

	return 0;
}//end main()