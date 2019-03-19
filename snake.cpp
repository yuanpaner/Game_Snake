#include <cstdlib>

#include <ncurses.h>
#include <unistd.h> // sleep 


// #include <stdio.h>
#include <termios.h> // tcflush
// #include <unistd.h>
// #include <fcntl.h>
// #include <ctype.h>



#include <deque>
#include <string>
// #include <vector>
#include <unordered_set>
#include <unordered_map>

#define TICKRATE 100
#define WORLD_WIDTH 30
#define WORLD_HEIGHT 30
#define SNAKEY_LENGTH 40
#define DEFAUT_SPEED 1
#define BOUNDARY "*"
#define BOUNDARY_UD '-'
#define BOUNDARY_LR '|'
#define BODY "@"
#define FRUIT ""
#define MARGIN 2

#define QUIT 'q'

using namespace std;
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

    /* custom comparator and hash function*/
    struct StringEqualByXY {
        bool operator()(const pair<int,int> & p1, const pair<int,int> & p2) const {
     
            return p1.first == p2.first && p1.second == p2.second;
        }
    };
    struct StringHashByXY {
        size_t operator()(const pair<int,int> & p) const {
            string str = to_string(p.first) + "#" + to_string(p.second);
            return std::hash<string>()(str);
        }
    };

    bool m_isOver;

    /* windows width and height */
    const int m_width;
    const int m_height;

    /* snake's direction, location, speed, score and body's points */
    Direction m_dir; 
    int m_r, m_c;
    int m_score;
    int m_speed;
    unordered_set<pair<int,int>, StringHashByXY, StringEqualByXY> m_snake_body;
    deque<pair<int,int>> m_snake_body_q;

    /* fruit's location */
    int m_fruitc, m_fruitr;

    
    void win_init()
    {   
        initscr(); 
        noecho();
        // cbreak();
        curs_set(0);
    }
    void generate_fruit()
    {
        do{
            m_fruitc = (rand() % m_width) + 1;
            m_fruitr = (rand() % m_height) + 1;
        }while(m_snake_body.find({m_fruitr, m_fruitc}) != m_snake_body.end());        
    }
public:
    SnakeGame(int w = WORLD_WIDTH, int h = WORLD_HEIGHT, int s = DEFAUT_SPEED)
    : m_isOver{false}, m_width{w * 2}, m_height{h}, m_speed{s}{

        win_init();

        m_dir = Direction::STOP;
        m_r = m_width/2;
        m_c = m_height/2;
        m_snake_body.insert({m_r, m_c});
        m_snake_body_q.push_back({m_r, m_c});
        m_score = 0;

        generate_fruit();
                
    }


    

    
    void draw()
    {
        clear();
        mvhline(0, 0, BOUNDARY_UD, m_width + 2);
        mvhline(m_height + 1, 0, BOUNDARY_UD, m_width + 2);
        mvvline(1, 0, BOUNDARY_LR, m_height);
        mvvline(1, m_width + 1, BOUNDARY_LR, m_height);

        for(int r = 1; r < m_height + 1; ++r)
        {
            for(int c = 1; c < m_width + 1; ++c)
            {
                // if(r == 0 || r == m_height + 1)
                //     mvprintw(r, c, BOUNDARY);
                // else if(c == 0 || c == m_width + 1)
                //     mvprintw(r, c, BOUNDARY);
                if(m_snake_body.find({r,c}) != m_snake_body.end()) // including x, y and old body
                    mvprintw(r, c, BODY);
                else if(r == m_fruitr && c == m_fruitc)
                    mvprintw(r, c, FRUIT);
            }
        }

        mvprintw(m_height + MARGIN, 0 , "SCORE : %d", m_score);
        mvprintw(m_height + MARGIN + 1, 0 , "SPEED : %d", m_speed);
        refresh();
    }

    void input()
    {
        keypad(stdscr, TRUE);
        halfdelay(1);
        int c = getch();
        switch(c)
        {
            case KEY_LEFT:
                if(m_dir != Direction::RIGHT)
                    m_dir = Direction::LEFT;
                break;
            case KEY_RIGHT:
                if(m_dir != Direction::LEFT)
                    m_dir = Direction::RIGHT;
                break;
            case KEY_UP:
                if(m_dir != Direction::DOWN)
                    m_dir = Direction::UP;
                break;
            case KEY_DOWN:
                if(m_dir != Direction::UP)
                    m_dir = Direction::DOWN;
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
        if(m_r <= 0 || m_r > m_height || m_c <= 0 || m_c > m_width)
        {
            m_isOver = true;
            return;
        }

        /* erase the tail */

        if(m_r == m_fruitr && m_c == m_fruitc) // eat the fruit
        {
            ++m_score;

            m_snake_body.insert({m_r, m_c});
            m_snake_body_q.push_front({m_r, m_c});
            generate_fruit();
        }
        else
        {
            m_snake_body.erase(m_snake_body_q.back());
            m_snake_body_q.pop_back();
            if(m_snake_body.find({m_r, m_c}) != m_snake_body.end())
            {
                m_isOver = true;
                return;
            }
            m_snake_body.insert({m_r, m_c});
            m_snake_body_q.push_front({m_r, m_c});
            
        }
    }
    void delay() const 
    {
        usleep(1000 * (100 / m_speed)  + 1);
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
int main(int argc, char const *argv[])
{

    SnakeGame sg(20,20);
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



