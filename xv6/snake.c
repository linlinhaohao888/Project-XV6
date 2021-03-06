//
// Created by masteryoda on 18-5-21.
//
#include"types.h"
#include"stat.h"
#include"user.h"
#include "x86.h"


#define MAX        100
#define COL        40
#define ROW        20
#define UP         1
#define DOWN       2
#define LEFT       3
#define RIGHT      4
#define BEND       5
#define STRAIGHT   0

struct Pos{
    int x;
    int y;
};

struct Snake{
    struct Pos body[MAX];
    int len;
    int state;
    int dirc;
};


struct Pos food;
struct Snake snake;
int turningCount = 0; //to check if turning is complete
int grade = 0;
int eatingCount = 0;
int level = 1;
int speed = 10;
unsigned int randSeed;

void acquireInput();
void gameFail(void);
void exitGame(void);
void move();

//generate random data
unsigned int rand(void)
{
    unsigned int r;
    r = randSeed * 1103515245 + 12345;
    randSeed = r;
    return (r << 16) | ((r >> 16) & 0xFFFF);
}

//print out game map
void printMap(void){
    for (int i = 0; i < ROW; ++i) {
        if(i == 0 || i == ROW - 1){
            for (int j = 0; j < COL; ++j)
                printf(1, "*");
            printf(1, "\n");
        }
        else{
            for (int j = 0; j < COL; ++j){
                if(j == 0 || j == COL - 1)
                    printf(1, "*");
                else
                    printf(1, " ");
            }
            printf(1, "\n");
        }
    }
}

//print out the whole snake
void printSnake(){
    for (int i = 0; i < snake.len; ++i) {
        outexac(snake.body[i].x, snake.body[i].y, '*');
    }
}

//print out score
void printScore(){
    char a[] = "Grade:";
    char b[] = "Level:";

    for (int i = 0; a[i] ; ++i) {
        outexac(50 + i, 10, a[i]);
    }
    printf(1, "%d", grade);

    for (int i = 0; b[i] ; ++i) {
        outexac(50 + i, 14, b[i]);
    }
    printf(1, "%d", level);

    outexac(49, 18, ' ');
    printf(1, "Press 'q' to quit");
}

//game inition
void initGame(){
    //device inition
    cmdmod(2, 2);
    clrscr();
    //init rand seed
    randSeed = time();

    //init map
    printMap();

    //init snake
    snake.len = 3;
    snake.dirc = RIGHT;
    for (int i = 0; i < 3; ++i) {
        snake.body[i].x = 2 + i;
        snake.body[i].y = 1;
    }
    printSnake();

    //init food
    food.x = rand() % (COL - 1);
    food.y = rand() % (ROW - 1);
    if(food.x == 0)
        food.x += 4;
    if(food.y == 0)
        food.y += 4;
    outexac(food.x, food.y, '*');

    printScore();
}

//check if snake collides with the wall or itself
int collision(){
    if(snake.body[snake.len - 1].x == 0 || snake.body[snake.len - 1].x == COL - 1
       || snake.body[snake.len - 1].y == 0 || snake.body[snake.len - 1].y == ROW - 1){
        return 1;
    }

    for (int i = 0; i < snake.len - 3; ++i) {
        if(snake.body[i].x == snake.body[snake.len - 1].x
           && snake.body[i].y == snake.body[snake.len - 1].y)
            return 1;
    }
    return 0;
}

//check if snake eats a food
void eat(){
    if(snake.body[snake.len - 1].x == food.x && snake.body[snake.len - 1].y == food.y){
        //level up
        grade += 10;
        eatingCount++;
        if(eatingCount > 10){
            level++;
            eatingCount = 0;
            if(speed > 0)
                alarm(--speed, move);
        }
        printScore();

        //add a new body
        if(snake.len < MAX){
            switch (snake.dirc){
                case UP:
                    snake.body[snake.len].x = snake.body[snake.len - 1].x;
                    snake.body[snake.len].y = snake.body[snake.len - 1].y - 1;
                    snake.len++;
                    break;
                case DOWN:
                    snake.body[snake.len].x = snake.body[snake.len - 1].x;
                    snake.body[snake.len].y = snake.body[snake.len - 1].y + 1;
                    snake.len++;
                    break;
                case LEFT:
                    snake.body[snake.len].x = snake.body[snake.len - 1].x - 1;
                    snake.body[snake.len].y = snake.body[snake.len - 1].y;
                    snake.len++;
                    break;
                case RIGHT:
                    snake.body[snake.len].x = snake.body[snake.len - 1].x + 1;
                    snake.body[snake.len].y = snake.body[snake.len - 1].y;
                    snake.len++;
                    break;
            }
        }

        //add new food
        food.x = rand() % COL;
        food.y = rand() % ROW;

        outexac(food.x, food.y, '*');
    }
}

//snake's movement
void move(){
    //initial check
    eat(snake);
    if(collision(snake))
        gameFail();

    //the snake's movement contains three steps:
    //1. cut the tail
    //2. every node of the snake's body moves to its previous node's position except the head node
    //3. move the head of snake according to the snake's advancing direction
    //def: head node represents the foremost node of the body with respect to the snake's advancing direction
    outexac(snake.body[0].x, snake.body[0].y, ' ');
    for (int i = 0; i < snake.len - 1; ++i) {
        snake.body[i].x = snake.body[i + 1].x;
        snake.body[i].y = snake.body[i + 1].y;
    }
    switch (snake.dirc){
        case UP:
            snake.body[snake.len - 1].y--;
            outexac(snake.body[snake.len - 1].x, snake.body[snake.len - 1].y, '*');
            break;
        case DOWN:
            snake.body[snake.len - 1].y++;
            outexac(snake.body[snake.len - 1].x, snake.body[snake.len - 1].y, '*');
            break;
        case LEFT:
            snake.body[snake.len - 1].x--;
            outexac(snake.body[snake.len - 1].x, snake.body[snake.len - 1].y, '*');
            break;
        case RIGHT:
            snake.body[snake.len - 1].x++;
            outexac(snake.body[snake.len - 1].x, snake.body[snake.len - 1].y, '*');
            break;
    }
    //check if the snake is already straight
    if(snake.state != STRAIGHT){
        if(turningCount == snake.len){
            snake.state = STRAIGHT;
            turningCount = 0;
        }
        else {
            turningCount++;
        }
    }
}

//acquire keyboard input and shift the snake's state and direction
void acquireInput(){
    char input;
    input = gameget();
    if (input > 0){
        switch (input){
            case 'w':
                turningCount = 0;
                snake.dirc = UP;
                snake.state = BEND;
                break;
            case 's':
                turningCount = 0;
                snake.dirc = DOWN;
                snake.state = BEND;
                break;
            case 'a':
                turningCount = 0;
                snake.dirc = LEFT;
                snake.state = BEND;
                break;
            case 'd':
                turningCount = 0;
                snake.dirc = RIGHT;
                snake.state = BEND;
                break;
            case 'q':
                exitGame();
            default:
                break;
        }
    }
}

//execute when game fails
void gameFail(void){
    alarm(speed, acquireInput);
}

//exit game
void exitGame(void){
    cmdmod(1, 1);
    clrscr();
    exit();
}

//game run function
//function executes every constant period
void gameRun(){
    move();
    acquireInput();
}

int main(int argc, char *argv[]){
    initGame();
    alarm(speed, gameRun);
    while (1);
    return 0;
}


