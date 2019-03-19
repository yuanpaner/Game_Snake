#include <cstdlib>

#include <ncurses.h>
#include <unistd.h> // sleep 


// #include <stdio.h>
// #include <thread>         // std::this_thread::sleep_for
// #include <chrono>         // std::chrono::seconds


// #include <iostream>
#include <deque>
#include <string>
#include <vector>
// #include <unordered_set>
// #include <unordered_map>

using namespace std;


#define TICKRATE 100
#define WORLD_WIDTH 30
#define WORLD_HEIGHT 30
#define SNAKEY_LENGTH 40
#define DEFAUT_SPEED 1
#define QUIT 'q'

#define EMPTY_INT 0
#define BOUNDARY_INT 1
#define BODY_INT 2
#define FRUIT_INT 3
const vector<const char *> printable{"", "*", "@", ""};

#define MARGIN 2



// struct COORDINATE{
//     int row;
//     int col;
//     bool operator==(COORDINATE & rhs) const
//     { 
//         return rhs.row == row && rhs.col == col;
//     }
// };

enum class Direction{  
    STOP = 0,
    LEFT,
    RIGHT,
    UP,
    DOWN
};
    
class SnakeGame
{

    static int m_counter;
    bool m_isOver;

    /* windows width and height */
    const int m_width;
    const int m_height;

    /* snake's direction, location, speed, score and body's points */
    Direction m_dir; 
    int m_r, m_c;
    int m_score;
    int m_speed;
    
    deque<pair<int,int>> m_snake_body_q;
    vector<vector<int>> m_board;
    const int m_board_width;
    const int m_board_height;

    /* fruit's location */
    int m_fruitc, m_fruitr;

    void win_init()
    {   
        initscr(); 
        noecho();
        cbreak();
        curs_set(0);
    }
    void generate_fruit()
    {
        do{
            m_fruitc = (rand() % m_width) + 1;
            m_fruitr = (rand() % m_height) + 1;
        }while(m_board[m_fruitr][m_fruitc] != 0);        
    }
public:
    SnakeGame(int w = WORLD_WIDTH, int h = WORLD_HEIGHT, int s = DEFAUT_SPEED)
    :   m_isOver{false}, 
        m_width{w * 2}, 
        m_height{h}, 
        m_speed{s}, 
        m_board_height{m_height + 2}, 
        m_board_width{m_width + 2}{

        win_init();

        m_dir = Direction::STOP;
        m_c = m_width/2;
        m_r = m_height/2;
        m_score = 0;


        m_board.resize(m_board_height);
        for (int i = 0; i < m_board_height; ++i)
        {
            m_board[i].resize(m_board_width);
            if(i == 0 || i == m_board_height - 1)
                fill(m_board[i].begin(), m_board[i].end(), BOUNDARY_INT);
            else
                fill(m_board[i].begin(), m_board[i].end(), EMPTY_INT);
        }
        for (int i = 1; i < m_board_height - 1; ++i)
        {
            m_board[i][0] = m_board[i][m_board_width - 1] = BOUNDARY_INT;
        }

        m_board[m_r][m_c] = BODY_INT;
        m_snake_body_q.push_back({m_r, m_c});
        

        generate_fruit();
        m_board[m_fruitr][m_fruitc] = FRUIT_INT;
    }


    

    
    void draw()
    {
        clear(); 

        for(int r = 0; r < m_board_height; ++r)
        {
            for(int c = 0; c < m_board_width; ++c)
            {
                if(m_board[r][c] != EMPTY_INT)
                    mvprintw(r, c, printable[m_board[r][c]]);
            }
        }

        mvprintw(m_height + MARGIN, 0 , "SCORE : %d", m_score);
        mvprintw(m_height + MARGIN + 1, 0 , "SPEED : %d", m_speed);
        mvprintw(m_height + MARGIN + 2, 0 , "OPERATION : %d", m_counter);
        refresh();

    }

    void input()
    {
        // mvprintw(m_height + MARGIN + 5, 0 , "here\n"); 
        keypad(stdscr, TRUE);
        halfdelay(1);
        // // int c = getch();
        int c = getch();
        
        

        switch(c)
        {

            case KEY_LEFT:
                if(m_dir != Direction::RIGHT)
                    m_dir = Direction::LEFT;
                m_counter++;
                break;
            case KEY_RIGHT:
                if(m_dir != Direction::LEFT)
                    m_dir = Direction::RIGHT;
                m_counter++;
                break;
            case KEY_UP:
                if(m_dir != Direction::DOWN)
                    m_dir = Direction::UP;
                m_counter++;
                break;
            case KEY_DOWN:
                if(m_dir != Direction::UP)
                    m_dir = Direction::DOWN;
                m_counter++;
                break;
            case QUIT:
                m_isOver = true;
                break;
            default:
                break;
        }


    }

    void logic()
    {

        switch(m_dir)
        {
            case Direction::LEFT:
                --m_c;
                break;
            case Direction::RIGHT:
                ++m_c;
                break;
            case Direction::UP:
                --m_r;
                break;
            case Direction::DOWN:
                ++m_r;
                break;
            default:
                break;

        }



        // /* erase the tail */

        if(m_r <= 0 || m_r > m_height || m_c <= 0 || m_c > m_width)
        {
            m_isOver = true;
            return;
        }
        if(m_r == m_fruitr && m_c == m_fruitc) // eat the fruit
        {
            ++m_score;

            m_board[m_r][m_c] = BODY_INT;
            m_snake_body_q.push_front({m_r, m_c});

            generate_fruit();
            m_board[m_fruitr][m_fruitc] = FRUIT_INT;
        }
        else
        {
            int tailr = m_snake_body_q.back().first;
            int tailc = m_snake_body_q.back().second;

            m_board[tailr][tailc] = EMPTY_INT;
            if(m_board[m_r][m_c] != EMPTY_INT)
            {
                m_isOver = true;
                return;
            }
            else
            {
                m_board[m_r][m_c] = BODY_INT;
                m_snake_body_q.pop_back();
                m_snake_body_q.push_front({m_r,m_c});
            }            
        }
    }
    void delay() const 
    {
        usleep(1000 * (100 / m_speed)  + 1);
        // std::this_thread::sleep_for (std::chrono::seconds(1/m_speed));
    }
    bool isOver() const
    {
        return m_isOver;
    }
    void stop()
    {
        endwin();
    }
};

int SnakeGame::m_counter = 0;

int main(int argc, char const *argv[])
{

    SnakeGame sg(20,20, 5);
    // sg.setup(); 
    while(!sg.isOver())
    {
        sg.draw();
        sg.input();
        sg.logic();
        sg.delay();
    }
    getch();
    sg.stop();

    return 0;
}



