#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>
#include <conio.h>
#include <unistd.h>

#if 0
(1)业务逻辑设计
1.1获取运动方向 WSAD   Control
1.2收到命令 Move      Move
--> 撞墙、
吃到自己、
迟到食物、
移动方向
1.3刷新屏幕           View

#endif

//Food
typedef struct Pos
{
    int x_;
    int y_;
}Pos;

typedef struct fd
{
    Pos Pos_;

}Food;

void initFood(Food *fd)//产生的食物位置
{
    fd->Pos_.x_ = rand()%14+1;
    fd->Pos_.y_ = rand()%14+1;
}

//snake
enum SnakeBodyType
{
    Head,Tail,Body
};

typedef struct snake
{
    Pos Pos_;
    enum SnakeBodyType Body_;
    struct snake *next;
}Snake;

void initSnake(Snake **head, Food *fd)
{
    (*head) = (Snake*)malloc(sizeof(Snake));
    do
    {
        (*head)->Pos_.x_ = rand()%10+3;
        (*head)->Pos_.y_ = rand()%10+3;
        (*head)->Body_ = Head;
    }while((*head)->Pos_.x_ == fd->Pos_.x_ && (*head)->Pos_.y_ == fd->Pos_.y_);
    (*head)->next = NULL;//**********************
}

int isSnake(int i,int j,Snake *snake,enum SnakeBodyType *type)
{
    while(snake)
    {
        //在不在这个点上
        if(snake->Pos_.x_ == i && snake->Pos_.y_ == j)
        {
            *type = snake->Body_; //?????
            return 1;
        }
        snake = snake->next;
    }
    return 0;
}

//获取方向
enum Direction
{
    //ASWD的ASCII美国信息交换标准代码
    UP=119,DOWN=115,LEFT=97,RIGHT=100
};

void initDirection(enum Direction *dir)
{
    *dir = rand()%4;
    switch(*dir)
    {
    case 0:
        *dir = UP;
        break;
    case 1:
        *dir = DOWN;
        break;
    case 2:
        *dir = LEFT;
        break;
    case 3:
        *dir = RIGHT;
        break;
    default:
        break;
    }
}

void getDirection(enum Direction *dir)
{
    enum Direction newDir; //中间值 WSAD 之外的键
    while(kbhit())
    {
        newDir = getch();
        if(newDir != *dir)
        {
            if(newDir == UP ||newDir == DOWN ||newDir == LEFT ||newDir == RIGHT)
                *dir = newDir;
        }
    }
}

int isMeetWall(Snake *snake)
{
    if(snake->Pos_.x_ == 0 ||snake->Pos_.x_ == 15 ||snake->Pos_.y_ == 0 ||snake->Pos_.y_ == 15)
        return 1;
    else
        return 0;
}
int isMeetSelf(Snake *snake)
{
    Pos pos = snake->Pos_;
    snake = snake->next;
    while(snake)
    {
        if(snake->Pos_.x_ == pos.x_ && snake->Pos_.y_ == pos.y_)
            return 1;
        snake = snake->next;
    }
    return 0;
}
int isFood(Snake *snake, Food *fd)
{
    if(snake->Pos_.x_ == fd->Pos_.x_ && snake->Pos_.y_ == fd->Pos_.y_)
        return 1;
    else
        return 0;
}


int isFoodonBody(Snake *snake, Food *fd)
{
    while(snake)
    {
        if(snake->Pos_.x_ == fd->Pos_.x_ && snake->Pos_.y_ == fd->Pos_.y_)
            return 1;
        snake = snake->next;
    }
    return 0;
}
void generateFood(Snake *snake, Food *fd)
{
    do
    {
        fd->Pos_.x_ = rand()%14+1;
        fd->Pos_.y_ = rand()%14+1;
    }while(isFoodonBody(snake,fd));
}

//吃食
void eatFood(Snake *snake, Food *fd, int *score)
{
    //尾插法
    //    while(*snake) snake = snake->next; **×
    while(snake->next)  snake = snake->next;
    Snake *newNode = (Snake *)malloc(sizeof(Snake));
    snake->next = newNode;
    newNode->next = NULL;

    //判断是什么类型的
    if(snake->Body_ == Head)
    {
        newNode->Body_ = Tail;
        *score += 5;
    }
    else
    {
        snake->Body_ = Body;
        newNode->Body_ = Tail;
        *score += 5;
    }

    generateFood(snake,fd);
}

int snakeMove(enum Direction *dir,Food *fd,Snake *snake, int *score)
{
    //与保存头节点的位置
    Pos prePos = snake->Pos_;
    Snake *sh = snake;

    //方向
    switch (*dir)
    {
    case UP:
        snake->Pos_.x_--;
        break;
    case DOWN:
        snake->Pos_.x_++;
        break;
    case RIGHT:
        snake->Pos_.y_++;
        break;
    case LEFT:
        snake->Pos_.y_--;
        break;
    }

    //结点前移
    Pos curpos;
    while(snake->next)
    {
        curpos = snake->next->Pos_;
        snake->next->Pos_ = prePos;
        prePos = curpos;//完美承接
        snake = snake->next;
    }
    //撞墙
    if(isMeetWall(sh))
        return -1;
    //撞到身子
    if(isMeetSelf(sh))
        return -1;
    //吃到食物
    if(isFood(sh,fd))
        eatFood(sh,fd,score);
}

void flushScreen(Food *fd, Snake *snake)
{
    enum SnakeBodyType type;

    for(int i=0; i<16; i++)
    {
        for(int j=0; j<16; j++)
        {
            if(i==0 || j==0 || i==15 || j==15)
                printf(" #");
            else if(fd->Pos_.x_ == i && fd->Pos_.y_ ==j) //扫描判断那个点是食物的点
                printf(" &");//空格的魅力
            //            else if(snake->Pos_.x_ == i && snake->Pos_.y_ == j)
            //扫描 判断是否是🐍的一部分，并判断是什么类型的（头、身子、尾巴？）
            else if(isSnake(i,j,snake,&type))
            {
                switch (type) {
                case Head:
                    printf(" @");
                    break;
                case Body:
                    printf(" *");
                    break;
                case Tail:
                    printf(" ^");
                    break;
                }
            }
            else
                printf("  ");
        }
        putchar(10);
    }
}

int main()
{
    srand(time(NULL));
    Food fd;
    initFood(&fd);
    Snake *snake;
    initSnake(&snake,&fd);
    enum Direction dir;
    initDirection(&dir);
    printf("press s to start game\n");
    getch();
    int score = 0;
    while(1)
    {
        system("cls");
        getDirection(&dir);
        if(snakeMove(&dir,&fd,snake,&score) == -1)//集大成者
        {
            printf("your score : %d",score);
            getchar();
        }
        flushScreen(&fd,snake); //屏幕刷新 随机食物
        if(score <= 10)
            usleep(500000);
        else if(score <= 20)
            usleep(300000);
        else if(score <= 30)
            usleep(100000);
        else if(score <= 40)
            usleep(50000);
    }
    return 0;
}
