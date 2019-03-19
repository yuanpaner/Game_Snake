#include <cstdlib>
#include <ncurses.h>
#include <unistd.h> // sleep 

#define TICKRATE 100
 
#define WORLD_WIDTH 50
#define WORLD_HEIGHT 20
 
#define SNAKEY_LENGTH 40

#define QUIT 'q'


unsigned int microseconds;

bool g_GameOver; 
const int width = 20;
const int height = 20;
int x, y, fruitX, fruitY, score;
enum class eDirection {
    STOP = 0, 
    LEFT, 
    RIGHT, 
    UP, 
    DOWN
}dir;

int TailX[100], TailY[100];
int nTail = 0; 

// eDirection dir;

void Setup()
{
    initscr(); // also causes the first refresh operation to clear the screen
    // clear();
    // start_color();
    // init_color(COLOR_RED, 900, 0, 0);
    noecho(); 

    cbreak(); 
    curs_set(0); // cursor mode 0 Invisible

    g_GameOver = false;
    dir = eDirection::STOP;
    x = width / 2;
    y = height / 2;
    fruitX = (rand() % width) + 1;
    fruitY = (rand() % height) + 1;
    score = 0;
}

void Draw()
{
    clear(); 
    for (int i = 0; i < height + 2; ++i) // line,  y
    {
        for (int j = 0; j < width + 2; ++j) // col, x
        {
            // box(stdscr, 0, 0);

            if(i == 0 || i == 21)
                mvprintw(i, j, "*");  // print y, x
            else if(j  == 0 || j == 21)
                mvprintw(i, j , "*");
            else if( i == y && j == x)
                mvprintw(i, j , "O");
            else if( i == fruitY && j == fruitX)
                mvprintw(i, j , "");
            else /* find all the point in the tail section */
            {
                for(int k = 0; k < nTail; ++k)
                {
                    if(TailX[k] == j && TailY[k] == i)
                        mvprintw(i, j, "O");
                }
            }
        }
    }
    mvprintw(24, 0, "Score %d", score);
    refresh();  
}
/* how, and what to act 
 */
void Input()
{
    keypad(stdscr, TRUE);
    halfdelay(1);

    int c = getch();
    switch(c)
    {
        case KEY_LEFT:
            dir = eDirection::LEFT;
            break;
        case KEY_RIGHT:
            dir = eDirection:: RIGHT;
            break;
        case KEY_UP:
            dir = eDirection::UP;
            break;
        case KEY_DOWN:
            dir = eDirection::DOWN;
            break;
        case QUIT: // 113
            g_GameOver = true; 
            break;
        default: break;
    }
}

void Logic()
{
    int prevX = TailX[0];
    int prevY = TailY[0];
    int prev2X, prev2Y;
    TailX[0] = x;
    TailY[0] = y;
    for (int i = 1; i < nTail; ++i)
    {
        prev2X = TailX[i];
        prev2Y = TailY[i];
        TailX[i] = prevX;
        TailY[i] = prevY;
        prevX = prev2X;
        prevY = prev2Y;
    }
    /* direction */
    switch(dir)
    {
        case eDirection::LEFT:
            --x;
            break;
        case eDirection::RIGHT:
            ++x;
            break;
        case eDirection::UP:
            --y;
            break;
        case eDirection::DOWN:
            ++y;
            break;
        default:
            break;
    }
    if(x > width || x < 1 || y < 1 || y > height)
        g_GameOver = true;
    if(x == fruitX && y == fruitY)
    {
        ++score;
        fruitX = (rand() % width) + 1;
        fruitY = (rand() % height) + 1;
        ++nTail;
    }
    for (int i = 0; i < nTail; ++i)
    {
        if(TailX[i] == x && TailY[i] == y)
        {
            g_GameOver = true;
        }
    }

}
int main(int argc, char const *argv[])
{
    Setup();

    while(!g_GameOver)
    {
        Draw();
        Input();
        Logic();
        usleep(1000 * microseconds); // sleep(60)  recomended speed is 40 to 80;
    }

    // while(getch())
    // {
    //     endwin();
    //     break;
    // }
    // refresh();
    getch(); 
    endwin(); /* End curses mode      */
    return 0;
}



