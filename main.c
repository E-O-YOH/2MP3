#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define SIZE 10
#define BOMBS 15

struct Coordinates                           // defining the structure as a global variable so it can be used by every function
{
    int x;
    int y;
};

void initializeBoard(char board[SIZE][SIZE]) // initializes a blank board with '-' in every cell
{
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            board[i][j] = '-';
        }
    }
}

void placeBombs(char board[SIZE][SIZE])     // randomly places bombs on the board
{
    srand(time(NULL));                      // sets the seed for the random number generator

    for (int n = BOMBS; n > 0; n--)
    {
        int i = rand() % SIZE;              // picks a random integer 0<=i<SIZE
        int j = rand() % SIZE;

        if (board[i][j] == 'X')             // checks if a bomb has already been placed, if so then add another step to the iteration so a different cell can be chosen, if not add a bomb there
        {
            n++;
        }
        else
        {
            board[i][j] = 'X';
        }
    }
}

void printBoard(char board[SIZE][SIZE])     // prints the board array with rows and columns numbered
{
    printf("\n ");

    for (int j = 0; j < SIZE; j++)          // prints column labels
    {
        printf(" %d", j);
    }

    printf("\n");

    for (int i = 0; i < SIZE; i++)          // prints row labels and board
    {
        printf("%d", i);                    // prints row labels

        for (int j = 0; j < SIZE; j++)      // prints board
        {
            printf(" %c", board[i][j]);
        }

        printf("\n");
    }
}

void countAdjacentBombs(char board[SIZE][SIZE]) // creates an answer key for the board
{
    for (int i = 0; i < SIZE; i++)              // checks every cell on the board
    {
        for (int j = 0; j < SIZE; j++)
        {
            if (board[i][j] != 'X')             // can skip cells with bombs
            {
                char n = '0';                   // must be a char so it can be properly output in the printBoard() function, as this function prints char with %c

                for (int x = -1; x <= 1; x++)   // check the eight adjacent cells
                {
                    for (int y = -1; y <= 1; y++)
                    {
                        if (x == 0 && y == 0)   // skip the center cell (i, j)
                            continue;

                        int adjRow = i + x;
                        int adjCol = j + y;

                        if (adjRow >= 0 && adjRow < SIZE && adjCol >= 0 && adjCol < SIZE) // check if the adjacent row and column are within bounds, if not then ignore them
                        {
                            if (board[adjRow][adjCol] == 'X')
                            {
                                n++;            // increment n as a character
                            }
                        }
                    }
                }
                board[i][j] = n;                // mark the answer for the solved cell
            }
        }
    }
}

struct Coordinates playerChoice(char playerBoard[SIZE][SIZE]) // user input and error handling
{
    struct Coordinates choice;                                // define a structure which allows both the x and y coordinates to be returned at the end of the function
    int correctInput = 0;                                     // boolean for checking if the user input is acceptable

    while (!correctInput)                                     // checks if the user input is acceptable and error handling if not
    {
        printf("\nEnter coordinates (row column): ");
        int check = scanf("%d %d", &choice.x, &choice.y);     // set equal to check for the first if statement, & meaning save %d to the address of the x part of the structure choice

        if (check != 2) // next 3 lines from ChatGPT          // used to clear the input buffer to prevent an infinite loop if an incorrect input is given
        {
            int c;
            while ((c = getchar()) != '\n' && c != EOF)
                ;                                             // I believe this blank while loop removes the rest of the invalid input until the input buffer is empty. This prevents the issue I was having in which an input that isn't an integer would cause an infinite loop of print messages.

            printf("\nIncorrect Input! Must be 2 whole numbers seperated by a space. Try Again.");
            continue;                                         // used to avoid buffer clearing at the end of the while loop which was causing a bug
        }
        else if (choice.x >= 0 && choice.x < SIZE && choice.y >= 0 && choice.y < SIZE) // if the input is a valid coordinate on the board
        {
            if (playerBoard[choice.x][choice.y] != '-')       // check if the player has already chosen that cell
            {
                printf("\nIncorrect Input! That cell has already been picked. Try Again.");
            }
            else                                              // if the input is a valid coordinate and hasn't been chosen yet, exit the while loop
            {
                correctInput = 1;
            }
        }
        else                                                  // if the inputs are both integers but not coordinates on the board
        {
            printf("\nIncorrect Input! Make sure to enter a coordinate on the board. Try Again.");
        }

        int c; // same ChatGPT code from above                // used to clear input buffer at the end of the iteration to avoid issues with the user inputing 3 integers
        while ((c = getchar()) != '\n' && c != EOF)
            ;
    }
    return choice;
}

int updateBoard(char solutionBoard[SIZE][SIZE], char playerBoard[SIZE][SIZE], struct Coordinates choice, int *remainingCells) // takes the user input and updates cell
{
    int x = choice.x;
    int y = choice.y;

    if (solutionBoard[x][y] == 'X')                 // the player hit a bomb, find and display all other bombs
    {
        for (int i = 0; i < SIZE; i++)
        {
            for (int j = 0; j < SIZE; j++)
            {
                if (solutionBoard[i][j] == 'X')
                {
                    playerBoard[i][j] = 'X';
                }
            }
        }
        return 1;                                   // return gameover = true
    }
    else                                            // no bomb, update the chosen cell
    {
        playerBoard[x][y] = solutionBoard[x][y];

        if (playerBoard[x][y] == '0')               // reveal all adjacent cells to zero
        {
            for (int i = -1; i <= 1; i++)
            {
                for (int j = -1; j <= 1; j++)
                {
                    if (i == 0 && j == 0)           // skip the center cell (i, j)
                        continue;

                    int adjRow = i + x;
                    int adjCol = j + y;

                    if (adjRow >= 0 && adjRow < SIZE && adjCol >= 0 && adjCol < SIZE && playerBoard[adjRow][adjCol]=='-') // check the adjcent cell is on the board and not already revealed
                    {
                        struct Coordinates adj;
                        adj.x = adjRow;
                        adj.y = adjCol;
                        *remainingCells = *remainingCells - 1;                        // increments the remainingCells variable in the main down by one
                        updateBoard(solutionBoard, playerBoard, adj, remainingCells); // reveal more cells if one of the adjacent cells is also zero
                    }
                }
            }
        }
        return 0;                                   // return gameover = false
    }
}

int main()
{
    char solutionBoard[SIZE][SIZE];                 // answer key board
    char playerBoard[SIZE][SIZE];                   // board the player sees

    initializeBoard(solutionBoard);
    initializeBoard(playerBoard);

    placeBombs(solutionBoard);
    countAdjacentBombs(solutionBoard);

    int gameOver = 0;                               // boolean checking if the game is over
    int remainingCells = SIZE * SIZE - BOMBS;

    printf("\nWelcome to Ethan's Minesweeper!\n");

    while (remainingCells > 0 && !gameOver)
    {
        printBoard(playerBoard);
        struct Coordinates choice = playerChoice(playerBoard);
        gameOver = updateBoard(solutionBoard, playerBoard, choice, &remainingCells);
        remainingCells--;
    }

    if (gameOver)
    {
        printBoard(playerBoard);
        printf("\nGame Over! You hit a bomb. Thank you for playing!\n");
    }
    else                                            // only runs if remainingCells = 0
    {
        printBoard(solutionBoard);
        printf("\nYOU WIN! Congratulations! Thank you for playing!\n");
    }
}