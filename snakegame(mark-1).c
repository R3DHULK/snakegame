#include <stdio.h>
#include <math.h>
#include <time.h>
#include <ctype.h>
#include <string.h>
#define SNAKE_ARRAY_SIZE 310

//Linux Libraries
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>


//Linux Constants
//Controls (arrow keys for Manjaro)
#define UP_ARROW  'A'
#define LEFT_ARROW  'D'
#define RIGHT_ARROW  'C'
#define DOWN_ARROW 'B'
#define ENTER_KEY 10

const char SNAKE_HEAD = '@';
const char SNAKE_BODY = '*';
const char WALL = '|';
const char CEIL = '-';
const char FOOD = 'X';
const char BLANK = ' ';

//Linux Functions - These functions emulate some functions from the windows only conio header file
void gotoxy(int x,int y)
{
    printf("%c[%d;%df",0x1B,y,x);
}


int kbhit(void)
{
  struct termios oldt, newt;
  int ch;
  int oldf;

  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

  ch = getchar();

  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);

  if(ch != EOF)
  {
    ungetc(ch, stdin);
    return 1;
  }

  return 0;
}

char getch()
{
    char c;
    system("stty raw");
    c= getchar();
    system("stty sane");
    return(c);
}

void clrscr()
{
    system("clear");
    return;
}
//End linux Functions


//This should be the same on both operating systems
#define EXIT_BUTTON 27 //ESC
#define PAUSE_BUTTON 112 //P

char waitForAnyKey(void)
{
int pressed;

while(!kbhit());

pressed = getch();
return((char)pressed);
}

int getGameSpeed()
{
int speed = 1;
clrscr();

while(speed < 1 || speed > 9){
        return(speed);
}
return 1;
}



//This function checks if a key has pressed, then checks if its any of the arrow keys/ p/esc key. It changes direction acording to the key pressed.
int checkKeysPressed(int direction)
{
int pressed;

if(kbhit()) //If a key has been pressed
{
    pressed=getch();
    if (direction != pressed)
    {
        if(pressed == DOWN_ARROW && direction != UP_ARROW)
            direction = pressed;
        else if (pressed == UP_ARROW && direction != DOWN_ARROW)
            direction = pressed;
        else if (pressed == LEFT_ARROW && direction != RIGHT_ARROW)
            direction = pressed;
        else if (pressed == RIGHT_ARROW && direction != LEFT_ARROW)
            direction = pressed;
    }
}
return(direction);
}

//Cycles around checking if the x y coordinates ='s the snake coordinates as one of this parts
//One thing to note, a snake of length 4 cannot collide with itself, therefore there is no need to call this function when the snakes length is <= 4
int collisionSnake (int x, int y, int snakeXY[][SNAKE_ARRAY_SIZE], int snakeLength, int detect)
{
int i;
for (i = detect; i < snakeLength; i++) //Checks if the snake collided with itself
{
    if ( x == snakeXY[0][i] && y == snakeXY[1][i])
        return(1);
}
return(0);
}

//Generates food & Makes sure the food doesn't appear on top of the snake <- This sometimes causes a lag issue!!! Not too much of a problem tho
int generateFood(int foodXY[], int width, int height, int snakeXY[][SNAKE_ARRAY_SIZE], int snakeLength)
{

do
{
    srand ( time(NULL) );
    foodXY[0] = rand() % (width-2) + 2;
    srand ( time(NULL) );
    foodXY[1] = rand() % (height-6) + 2;
} while (collisionSnake(foodXY[0], foodXY[1], snakeXY, snakeLength, 0)); //This should prevent the "Food" from being created on top of the snake. - However the food has a chance to be created ontop of the snake, in which case the snake should eat it...

gotoxy(foodXY[0] ,foodXY[1]);
printf("%c", FOOD);

return(0);
}

/*
Moves the snake array forward, i.e.
This:
x 1 2 3 4 5 6
y 1 1 1 1 1 1
Becomes This:
x 1 1 2 3 4 5
y 1 1 1 1 1 1

Then depending on the direction (in this case west - left) it becomes:

x 0 1 2 3 4 5
y 1 1 1 1 1 1

snakeXY[0][0]--; <- if direction left, take 1 away from the x coordinate
*/
void moveSnakeArray(int snakeXY[][SNAKE_ARRAY_SIZE], int snakeLength, int direction)
{
int i;
for( i = snakeLength-1; i >= 1; i-- )
{
    snakeXY[0][i] = snakeXY[0][i-1];
    snakeXY[1][i] = snakeXY[1][i-1];
}

/*
because we don't actually know the new snakes head x y,
we have to check the direction and add or take from it depending on the direction.
*/
switch(direction)
{
    case DOWN_ARROW:
        snakeXY[1][0]++;
        break;
    case RIGHT_ARROW:
        snakeXY[0][0]++;
        break;
    case UP_ARROW:
        snakeXY[1][0]--;
        break;
    case LEFT_ARROW:
        snakeXY[0][0]--;
        break;
}

return;
}

void move(int snakeXY[][SNAKE_ARRAY_SIZE], int snakeLength, int direction)
{
int x;
int y;

//Remove the tail ( HAS TO BE DONE BEFORE THE ARRAY IS MOVED!!!!! )
x = snakeXY[0][snakeLength-1];
y = snakeXY[1][snakeLength-1];

gotoxy(x,y);
printf("%c",BLANK);

//Changes the head of the snake to a body part
gotoxy(snakeXY[0][0],snakeXY[1][0]);
printf("%c", SNAKE_BODY);

moveSnakeArray(snakeXY, snakeLength, direction);

gotoxy(snakeXY[0][0],snakeXY[1][0]);
printf("%c",SNAKE_HEAD);

gotoxy(1,1); //Gets rid of the darn flashing underscore.

return;
}

//This function checks if the snakes head his on top of the food, if it is then it'll generate some more food...
int eatFood(int snakeXY[][SNAKE_ARRAY_SIZE], int foodXY[])
{
if (snakeXY[0][0] == foodXY[0] && snakeXY[1][0] == foodXY[1])
{
    foodXY[0] = 0;
    foodXY[1] = 0; //This should prevent a nasty bug (loops) need to check if the bug still exists...

    printf("\7"); //Beep
    return(1);
}

return(0);
}

int collisionDetection(int snakeXY[][SNAKE_ARRAY_SIZE], int consoleWidth, int consoleHeight, int snakeLength ) //Need to Clean this up a bit
{
int colision = 0;
if ((snakeXY[0][0] == 1) || (snakeXY[1][0] == 1) || (snakeXY[0][0] == consoleWidth) || (snakeXY[1][0] == consoleHeight - 4)) //Checks if the snake collided wit the wall or it's self
    colision = 1;
else
    if (collisionSnake(snakeXY[0][0], snakeXY[1][0], snakeXY, snakeLength, 1)) //If the snake collided with the wall, there's no point in checking if it collided with itself.
        colision = 1;

return(colision);
}

void refreshInfoBar(int score, int speed)
{
gotoxy(5,23);
printf("Score: %d", score);

gotoxy(5,24);
printf("Speed: %d", speed);

gotoxy(52,23);
printf("Coders : n/a");

gotoxy(52,24);
printf("Version: 0.1");

return;
}


void gameOverScreen(void)
{
int x = 17, y = 3;

gotoxy(x,y++);
printf(":'######::::::'###::::'##::::'##:'########:\n");
gotoxy(x,y++);
printf("'##... ##::::'## ##::: ###::'###: ##.....::\n");
gotoxy(x,y++);
printf(" ##:::..::::'##:. ##:: ####'####: ##:::::::\n");
gotoxy(x,y++);
printf(" ##::'####:'##:::. ##: ## ### ##: ######:::\n");
gotoxy(x,y++);
printf(" ##::: ##:: #########: ##. #: ##: ##...::::\n");
gotoxy(x,y++);
printf(" ##::: ##:: ##.... ##: ##:.:: ##: ##:::::::\n");
gotoxy(x,y++);
printf(". ######::: ##:::: ##: ##:::: ##: ########:\n");
gotoxy(x,y++);
printf(":......::::..:::::..::..:::::..::........::\n");
gotoxy(x,y++);
printf(":'#######::'##::::'##:'########:'########::'####:\n");
gotoxy(x,y++);
printf("'##.... ##: ##:::: ##: ##.....:: ##.... ##: ####:\n");
gotoxy(x,y++);
printf(" ##:::: ##: ##:::: ##: ##::::::: ##:::: ##: ####:\n");
gotoxy(x,y++);
printf(" ##:::: ##: ##:::: ##: ######::: ########::: ##::\n");
gotoxy(x,y++);
printf(" ##:::: ##:. ##:: ##:: ##...:::: ##.. ##::::..:::\n");
gotoxy(x,y++);
printf(" ##:::: ##::. ## ##::: ##::::::: ##::. ##::'####:\n");
gotoxy(x,y++);
printf(". #######::::. ###:::: ########: ##:::. ##: ####:\n");
gotoxy(x,y++);
printf(":.......::::::...:::::........::..:::::..::....::\n");

waitForAnyKey();
clrscr(); //clear the console
return;
}

//Messy, need to clean this function up
void startGame( int snakeXY[][SNAKE_ARRAY_SIZE], int foodXY[], int consoleWidth, int consoleHeight, int snakeLength, int direction, int score, int speed)
{
int gameOver = 0;
clock_t endWait;

//CLOCKS_PER_SEC-(n-1)*(CLOCKS_PER_SEC/10)
int waitMili = CLOCKS_PER_SEC-(speed)*(CLOCKS_PER_SEC/10);  //Sets the correct wait time according to the selected speed
int tempScore = 10*speed;
int oldDirection;
int canChangeDirection = 1;
//int seconds = 1;

endWait = clock() + waitMili;

do
{
    if(canChangeDirection)
    {
        oldDirection = direction;
        direction = checkKeysPressed(direction);
    }

    if(oldDirection != direction)//Temp fix to prevent the snake from colliding with itself
        canChangeDirection = 0;

    if(clock() >= endWait) //it moves according to how fast the computer running it is...
    {

        move(snakeXY, snakeLength, direction);
        canChangeDirection = 1;


        if(eatFood(snakeXY, foodXY))
        {
            generateFood( foodXY, consoleWidth, consoleHeight, snakeXY, snakeLength); //Generate More Food
            snakeLength++;
            score+=10;

            if( score >= 10*speed+tempScore)
            {
                speed++;
                tempScore = score;

                if(speed <= 9)//this needs to be fixed
                    waitMili = waitMili - (CLOCKS_PER_SEC/10);
                else
                {
                    if(waitMili >= 40) //Maximum Speed (the game has to be beatable)
                        waitMili = waitMili - (CLOCKS_PER_SEC/200);

                }

            }

            refreshInfoBar(score, speed);
        }

        endWait = clock() + waitMili; //TEMP FIX, NEED TO FIND A WAY TO RESET CLOCK().. Na, seems to work fine this way...
    }

    gameOver = collisionDetection(snakeXY, consoleWidth, consoleHeight, snakeLength);

    if(snakeLength >= SNAKE_ARRAY_SIZE-5) //Just to make sure it doesn't get longer then the array size & crash
    {
        gameOver = 2;//You Win! <- doesn't seem to work - NEED TO FIX/TEST THIS
        score+=1500; //When you win you get an extra 1500 points!!!
    }

} while (!gameOver);

switch(gameOver)
{
    case 1:
        printf("\7"); //Beep
        printf("\7"); //Beep

        gameOverScreen();
        break;
}

return;
}


void loadEnviroment(int consoleWidth, int consoleHeight)//This can be done in a better way... FIX ME!!!! Also i think it doesn't work properly in ubuntu <- Fixed
{
//int i;
int x = 1, y = 1;
int rectangleHeight = consoleHeight - 4;
clrscr(); //clear the console

gotoxy(x,y); //Top left corner

for (; y < rectangleHeight; y++)
{
    gotoxy(x, y); //Left Wall
    printf("%c",WALL);

    gotoxy(consoleWidth, y); //Right Wall
    printf("%c",WALL);
}

y = 1;
for (; x < consoleWidth+1; x++)
{
    gotoxy(x, y); //Left Wall
    printf("%c",WALL);

    gotoxy(x, rectangleHeight); //Right Wall
    printf("%c",WALL);
}

return;
}


void loadSnake(int snakeXY[][SNAKE_ARRAY_SIZE], int snakeLength)
{
int i;
/*
First off, The snake doesn't actually have enough XY coordinates (only 1 - the starting location), thus we use
these XY coordinates to "create" the other coordinates. For this we can actually use the function used to move the snake.
This helps create a "whole" snake instead of one "dot", when someone starts a game.
*/
//moveSnakeArray(snakeXY, snakeLength); //One thing to note ATM, the snake starts of one coordinate to whatever direction it's pointing...

//This should print out a snake :P
for (i = 0; i < snakeLength; i++)
{
    gotoxy(snakeXY[0][i], snakeXY[1][i]);
    printf("%c", SNAKE_BODY); //Meh, at some point I should make it so the snake starts off with a head...
}

return;
}


void prepairSnakeArray(int snakeXY[][SNAKE_ARRAY_SIZE], int snakeLength)
{
int i;
int snakeX = snakeXY[0][0];
int snakeY = snakeXY[1][0];



for(i = 1; i <= snakeLength; i++)
{
    snakeXY[0][i] = snakeX + i;
    snakeXY[1][i] = snakeY;
}

return;
}

//This function loads the environment, snake, etc
void loadGame(void)
{
int snakeXY[2][SNAKE_ARRAY_SIZE]; //Two Dimensional Array, the first array is for the X coordinates and the second array for the Y coordinates

int snakeLength = 2; //Starting Length

int direction = LEFT_ARROW; //DO NOT CHANGE THIS TO RIGHT ARROW, THE GAME WILL INSTANTLY BE OVER IF YOU DO!!! <- Unless the prepareSnakeArray function is changed to take into account the direction....

int foodXY[] = {5,5};// Stores the location of the food

int score = 0;
//int level = 1;

//Window Width * Height - at some point find a way to get the actual dimensions of the console... <- Also somethings that display don't take this dimensions into account.. need to fix this...
int consoleWidth = 80;
int consoleHeight = 25;

int speed = getGameSpeed();

//The starting location of the snake
snakeXY[0][0] = 40;
snakeXY[1][0] = 10;

loadEnviroment(consoleWidth, consoleHeight); //borders
prepairSnakeArray(snakeXY, snakeLength);
loadSnake(snakeXY, snakeLength);
generateFood( foodXY, consoleWidth, consoleHeight, snakeXY, snakeLength);
refreshInfoBar(score, speed); //Bottom info bar. Score, Level etc
startGame(snakeXY, foodXY, consoleWidth, consoleHeight, snakeLength, direction, score, speed);

return;
}


int main() //Need to fix this up
{
  loadGame();

  return(0);
}
c
termios
Share
Follow
edited Oct 20, 2019 at 8:28
user avatar
EsmaeelE
1,94955 gold badges1818 silver badges2727 bronze badges
asked Oct 19, 2019 at 18:06
user avatar
Konstantinos Lazaros
1111 silver badge33 bronze badges
1
Can you see where there is #define UP_ARROW  'A' etc? Just change those definitions to the keys you want to use, and recompile. The code is not using the actual arrow keys. – 
Weather Vane
 Oct 19, 2019 at 18:24 
@WeatherVane It compiled fine on my linux system – 
klutt
 Oct 19, 2019 at 18:38
@klutt are they in termios then? – 
Weather Vane
 Oct 19, 2019 at 18:39
@WeatherVane No idea. I just tried to compile, and the only thing i needed to do was to include stdio. After that it compiled without any warnings. – 
klutt
 Oct 19, 2019 at 18:41
1
@klutt I see now that the code implements its own versions of getch() and kbhit(). Too hasty. – 
Weather Vane
 Oct 19, 2019 at 18:43
Show 6 more comments
2 Answers
Sorted by:

Highest score (default)

0

the move() function does 3 things:

replaces the snake head with an extension of the body
moves the cursor to the next location and inserts a new snake head
moves the cursor to the top/left of the screen
Here are the messages from the compiler:

gcc    -ggdb -Wall -Wextra -Wconversion -pedantic -std=gnu11  -c "untitled.c"  (in directory: /home/richard/Documents/forum)
untitled.c: In function ‘gotoxy’:
untitled.c:32:9: warning: implicit declaration of function ‘printf’ [-Wimplicit-function-declaration]
         printf("%c[%d;%df",0x1B,y,x);
         ^~~~~~
untitled.c:32:9: warning: incompatible implicit declaration of built-in function ‘printf’
untitled.c:32:9: note: include ‘<stdio.h>’ or provide a declaration of ‘printf’
untitled.c: In function ‘kbhit’:
untitled.c:44:20: warning: negative integer implicitly converted to unsigned type [-Wsign-conversion]
       newt.c_lflag &= ~(ICANON | ECHO);
                    ^~
untitled.c:49:12: warning: implicit declaration of function ‘getchar’; did you mean ‘getcwd’? [-Wimplicit-function-declaration]
       ch = getchar();
            ^~~~~~~
            getcwd
untitled.c:54:16: error: ‘EOF’ undeclared (first use in this function); did you mean ‘VEOF’?
       if(ch != EOF)
                ^~~
                VEOF
untitled.c:54:16: note: each undeclared identifier is reported only once for each function it appears in
untitled.c:56:9: warning: implicit declaration of function ‘ungetc’ [-Wimplicit-function-declaration]
         ungetc(ch, stdin);
         ^~~~~~
untitled.c:56:20: error: ‘stdin’ undeclared (first use in this function); did you mean ‘sin’?
         ungetc(ch, stdin);
                    ^~~~~
                    sin
untitled.c: In function ‘getch’:
untitled.c:67:12: warning: conversion to ‘char’ from ‘int’ may alter its value [-Wconversion]
         c= getchar();
            ^~~~~~~
untitled.c: In function ‘generateFood’:
untitled.c:149:17: warning: conversion to ‘unsigned int’ from ‘time_t {aka long int}’ may alter its value [-Wconversion]
         srand ( time(NULL) );