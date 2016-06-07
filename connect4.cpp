// Ruoqi Tian
// cs375
// Dr. Hwang
// 10/08/2015

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>

#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <curses.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <gdbm.h>
#include "gamer.h"

#define WIDTH 32
#define HEIGHT 20

int startx = 0;
int starty = 0;

using namespace std;

struct space
{
  bool taken;
  string color;
};

void firework();
void start();
void print_grid(space grid[6][7], WINDOW *menu_win, string ucolor, string ccolor);
int usr_move(space grid[6][7], WINDOW *menu_win, int pair);
int com_move(space grid[6][7], WINDOW *menu_win);
void drop(space grid[6][7], int umove, int color, WINDOW *menu_win);
bool found_win(space grid[6][7], string color);
bool tie(space gird[6][7]);
int menu_select(WINDOW *menu_win);
int color_select(WINDOW *menu_win);
int scan(char username[10], int &win, int &lose, int &draw);
void write(char username[10], int win, int lose, int draw);
int quick_game(WINDOW *menu_win);
void menu_title(WINDOW *menu_win);
int multi_game(WINDOW *menu_win, string username1, string username2);
bool check(space grid[6][7], int x, int y, string color, int &count, int sx, int sy);

int main(int argc, char **argv)
{
  
  cout <<"******************************************" <<endl
       <<"Please resize your terminal window to larger than 25x50."
       <<endl <<"Full-screen is recommended for best experience."
       <<endl <<"When you are ready to go , enter \"y\": ";
  string cont, movie;
  cin >>cont;
  while(cont != "y")
    {
      cout <<"When you are ready to go , enter \"y\": ";
      cin >>cont;
    }

  cout <<"Watch opening video? (Y/N): ";
  cin >>movie;
  while(movie != "y" && movie!= "n")
    {
      cout <<"Watch opening video? (Y/N): ";
      cin >>movie;
    }
  
  struct winsize w;
  ioctl(0, TIOCGWINSZ, &w);

  WINDOW  *menu_win;
  startx = (w.ws_col - WIDTH) / 2;
  starty = (w.ws_row - HEIGHT) / 2;
  int mid_x = w.ws_col/2;
  int mid_y = w.ws_row/2;
  initscr();
  start_color();
  keypad(stdscr, TRUE);
  noecho();
  init_pair(3, COLOR_WHITE, 6);
  init_pair(4, COLOR_BLACK, COLOR_WHITE);
  init_pair(6, COLOR_BLUE, COLOR_WHITE);
  init_pair(5, COLOR_BLACK, 6);
  init_pair(7, COLOR_RED, COLOR_WHITE);
  init_pair(0, 6, COLOR_WHITE);
  chdir(getenv("HOME"));
  GDBM_FILE dbf;
  dbf = gdbm_open(".cfour", 0, GDBM_WRCREAT, 0644, 0);
  gdbm_close(dbf);

  curs_set(0);
  wbkgd(stdscr, COLOR_PAIR(3));

  if(movie == "y")
    start();
  
  wbkgd(stdscr, COLOR_PAIR(3));
  refresh();
  
  menu_win = newwin(HEIGHT, WIDTH, starty, startx);
  keypad(menu_win, TRUE);
  
  wbkgd(menu_win, COLOR_PAIR(4));
  box(menu_win, 0, 0);
  wrefresh(menu_win);

  menu_title(menu_win);

  int select = menu_select(menu_win);

  while(select != 3)
    {

      
      // Starts a quick game
      if(select == 0)
	{
	  quick_game(menu_win);
	}


      // single player
      if(select == 1)
	{
	  
	  
	  space plain_grid[6][7];
	  for(int x = 0; x < 6; x++)
	    {
	      for(int y = 0; y < 7; y++)
		{
		  plain_grid[x][y].taken = false;
		  plain_grid[x][y].color = "None";
		}
	    }
	  
	  print_grid(plain_grid, menu_win, "red", "yellow");

	  // User input name
	  int win = 0, lose = 0, draw = 0;
	  
	      string username;

	  int sure = 'n';
	  while(sure == 'n')
	    {
	      username = "";
	      curs_set(1);
	      echo();
	      cbreak();
	      keypad(menu_win, FALSE);
	      wmove(menu_win, 16, 1);
		  wclrtoeol(menu_win);

	      while(username == "")
		{
		  
		  wmove(menu_win, 15, 1);
		  wclrtoeol(menu_win);
		  box(menu_win, 0, 0);
		  refresh();
		  mvwprintw(menu_win, 15, 1, "Username: ");
		  int c;
		  for(int i = 0; i < 10; i++)
		    {
		      c = wgetch(menu_win);
		      if(c == '\n') break;
		      username += c;
		    }

		  if(username == "")
		    mvwprintw(menu_win, 16,1 , "Usernames cannot be empty!!");
		  wrefresh(menu_win);
		}
	      
	      
	      noecho();
	      curs_set(0);
	      keypad(menu_win, TRUE);
	      wmove(menu_win, 16, 1);
		  wclrtoeol(menu_win);
		  box(menu_win, 0, 0);
	      mvwprintw(menu_win, 16, 1, "Are you sure? Y/N");
	      sure = wgetch(menu_win);
	      while(sure != 'y' && sure != 'n')
		  sure = wgetch(menu_win);
	      if(sure == 'y') break;
	      
	    }
	  
	  wmove(menu_win, 16, 1);
	  wclrtoeol(menu_win);
	  box(menu_win, 0, 0);
	  wrefresh(menu_win);
	  
	  // Load user information
	  if(scan(const_cast<char*>(username.c_str()), win, lose, draw) == 0)
	    mvwprintw(menu_win, 16, 1, "New user: Welcome!");
	  else
	    {
	      mvwprintw(menu_win, 16, 1, "Record: %dW %dD %dL", win, draw, lose);
	    }
	  mvwprintw(menu_win, 17, 1, "Press ENTER to start");
	  int ent;
	  ent = wgetch(menu_win);
	  while(ent != 10)
	    {
	      ent = wgetch(menu_win);
	    }
	  wclear(menu_win);
	  box(menu_win, 0, 0);
	  wrefresh(menu_win);
	  
	  int winner;

	  winner = quick_game(menu_win);

	  if(winner == 1)
	    win ++;
	  else if(winner ==2)
	    lose++;
	  else
	    draw++;
	  write(const_cast<char*>(username.c_str()), win, lose, draw);
	  scan(const_cast<char*>(username.c_str()), win, lose, draw);
	  mvwprintw(menu_win, 16, 1, username.c_str());
	  mvwprintw(menu_win, 16, 1, "Record: %dW %dD %dL", win, draw, lose);
	  
	}


      //multiplayer
      if(select == 2)
	{
	  space plain_grid[6][7];
	  for(int x = 0; x < 6; x++)
	    {
	      for(int y = 0; y < 7; y++)
		{
		  plain_grid[x][y].taken = false;
		  plain_grid[x][y].color = "None";
		}
	    }

	  
	      string username1;
	  string username2;
	  int win1 = 0, win2 = 0, lose1 = 0,
	    lose2 = 0, draw1 = 0, draw2 = 0;
	  
	  while(username1 == username2)
	    {
	  print_grid(plain_grid, menu_win, "red", "yellow");

	  
	  // User1 input name
	  

	  int sure = 'n';
	  while(sure == 'n')
	    {
	      username1 = "";
	      curs_set(1);
	      echo();
	      cbreak();
	      keypad(menu_win, FALSE);

	      wmove(menu_win, 16, 1);
	      wclrtoeol(menu_win);
	      while(username1 == "")
		{
		  wmove(menu_win, 15, 1);
		  wclrtoeol(menu_win);
		  box(menu_win, 0, 0);
		  refresh;
		  mvwprintw(menu_win, 14, 1, "Player1");
		  mvwprintw(menu_win, 15, 1, "Username: ");
		  int c;
		  for(int i = 0; i < 10; i++)
		    {
		      c = wgetch(menu_win);
		      if(c == '\n') break;
		      username1 += c;
		    }

		  if(username1 == "")
		    mvwprintw(menu_win, 16, 1, "Usernames cannot be empty!!");
		}
	      
	      wmove(menu_win, 16, 1);
	      wclrtoeol(menu_win);
	      box(menu_win, 0, 0);
	      
	      noecho();
	      curs_set(0);
	      keypad(menu_win, TRUE);
	      mvwprintw(menu_win, 16, 1, "Are you sure? Y/N");
	      sure = wgetch(menu_win);
	      while(sure != 'y' && sure != 'n')
		  sure = wgetch(menu_win);
	      if(sure == 'y') break;
	      
	    }
	  
	  wmove(menu_win, 16, 1);
	  wclrtoeol(menu_win);
	  box(menu_win, 0, 0);
	  wrefresh(menu_win);


	  
	  // Load user1 information
	  if(scan(const_cast<char*>(username1.c_str()), win1, lose1, draw1) == 0)
	    mvwprintw(menu_win, 16, 1, "New user: Welcome!");
	  else
	    {
	      mvwprintw(menu_win, 16, 1, "Record: %dW %dD %dL", win1, draw1, lose1);
	    }
	  mvwprintw(menu_win, 17, 1, "Press ENTER to continue");
	  int ent;
	  ent = wgetch(menu_win);
	  while(ent != 10)
	    {
	      ent = wgetch(menu_win);
	    }
	  wclear(menu_win);
	  box(menu_win, 0, 0);
	  wrefresh(menu_win);

	  

	  
	  // User2 input name
	  print_grid(plain_grid, menu_win, "red", "yellow");
	  
	      

	  sure = 'n';
	  while(sure == 'n')
	    {
	      username2 = "";
	      curs_set(1);
	      echo();
	      cbreak();
	      keypad(menu_win, FALSE);

	      wmove(menu_win, 16, 1);
		  wclrtoeol(menu_win);
	      while(username2 == "")
		{
		  wmove(menu_win, 15, 1);
		  wclrtoeol(menu_win);
		  box(menu_win, 0, 0);
		  refresh;
		  mvwprintw(menu_win, 14, 1, "Player2");
		  mvwprintw(menu_win, 15, 1, "Username: ");
		  int c;
		  for(int i = 0; i < 10; i++)
		    {
		      c = wgetch(menu_win);
		      if(c == '\n') break;
		      username2 += c;
		    }

		  if(username2 == "")
		    mvwprintw(menu_win, 16, 1, "Usernames cannot be empty!!");
		}
	      
	      wmove(menu_win, 16, 1);
	      wclrtoeol(menu_win);
	      box(menu_win, 0, 0);

	      
	      noecho();
	      curs_set(0);
	      keypad(menu_win, TRUE);
	      mvwprintw(menu_win, 16, 1, "Are you sure? Y/N");
	      sure = wgetch(menu_win);
	      while(sure != 'y' && sure != 'n')
		  sure = wgetch(menu_win);
	      if(sure == 'y') break;
	      
	    }
	  
	  wmove(menu_win, 16, 1);
	  wclrtoeol(menu_win);
	  box(menu_win, 0, 0);
	  wrefresh(menu_win);


	  
	  // Load user2 information
	  if(scan(const_cast<char*>(username2.c_str()), win2, lose2, draw2) == 0)
	    mvwprintw(menu_win, 16, 1, "New user: Welcome!");
	  else
	    {
	      mvwprintw(menu_win, 16, 1, "Record: %dW %dD %dL", win2, draw2, lose2);
	    }
	  mvwprintw(menu_win, 17, 1, "Press ENTER to start");
	  ent = wgetch(menu_win);
	  while(ent != 10)
	    {
	      ent = wgetch(menu_win);
	    }
	  wclear(menu_win);
	  box(menu_win, 0, 0);
	  wrefresh(menu_win);

	  if(username1 == username2)
	    {
	      mvwprintw(menu_win, 8, 6, "Seriously?");
	      mvwprintw(menu_win,9, 6, "Against yourself?");
	      mvwprintw(menu_win,10, 6 ,"Re-enter after 3 s");
	      wrefresh(menu_win);
	      sleep(1);
	      mvwprintw(menu_win,10, 6 ,"Re-enter after 2 s");
	      wrefresh(menu_win);
	      sleep(1);
	      mvwprintw(menu_win,10, 6 ,"Re-enter after 1 s");
	      wrefresh(menu_win);
	      sleep(1);
	      
	    }

	}
	  int winner = multi_game(menu_win, username1, username2);
	  if(winner == 1)
	    {
	      win1++;
	      lose2++;
	      mvwprintw(menu_win, 15, 1, "%s won!", username1.c_str());
	      mvwprintw(menu_win, 16, 1, "Record: %dW %dD %dL", win1, draw1, lose1);
	      mvwprintw(menu_win, 17, 1, "%s lost!", username2.c_str());
	      mvwprintw(menu_win, 18, 1, "Record: %dW %dD %dL", win2, draw2, lose2);
	    }

	  else if(winner == 2)
	    {
	      win2++;
	      lose1++;
	      mvwprintw(menu_win, 15, 1, "%s won!", username2.c_str());
	      mvwprintw(menu_win, 16, 1, "Record: %dW %dD %dL", win2, draw2, lose2);
	      mvwprintw(menu_win, 17, 1, "%s lost!", username1.c_str());
	      mvwprintw(menu_win, 18, 1, "Record: %dW %dD %dL", win1, draw1, lose1);
	    }

	  else
	    {
	      draw1++;
	      draw2++;
	      mvwprintw(menu_win, 15, 1, "Tie!");
	      mvwprintw(menu_win, 16, 1, "%s: %dW %dD %dL", username1.c_str(),win1, draw1, lose1);
	      mvwprintw(menu_win, 17, 1, "%s: %dW %dD %dL", username2.c_str(),win2, draw2, lose2);
	    }
	  wrefresh(menu_win);

	  write(const_cast<char*>(username1.c_str()), win1, lose1, draw1);
	  write(const_cast<char*>(username2.c_str()), win2, lose2, draw2);
	  
	}

      // Return to main menu
      mvwprintw(menu_win, 2,1, "Do you want to exit the game?");
      mvwprintw(menu_win, 3,1, "Y: exit");
      mvwprintw(menu_win, 4,1, "N: return to main menu");
      wrefresh(menu_win);
      char exit;
      exit = wgetch(menu_win);
      while(exit != 'y' && exit != 'n')
	exit = wgetch(menu_win);
      if(exit == 'y') break;

      menu_title(menu_win);

      select = menu_select(menu_win);
    }
  
  delwin(menu_win);
  endwin();
  return 0;
}

// Print the board with the curent grid
void print_grid(space grid[6][7], WINDOW *menu_win, string ucolor, string ccolor)
{
  wclear(menu_win);
  box(menu_win, 0, 0);
  mvwprintw(menu_win, 5, 4, "________________________");
  for(int i = 6; i < 12; i++)
    mvwprintw(menu_win, i, 4, "||__|__|__|__|__|__|__||");
  mvwprintw(menu_win, 12, 4, "|     Connect Four     |");
  mvwprintw(menu_win, 13, 3, "/========================\\");
  for(int x = 0; x < 6; x++)
    {
      for(int y = 0; y < 7; y++)
	{
	  if(grid[x][y].taken == true)
	    {
	      if(grid[x][y].color == ucolor)
		{
		  wattron(menu_win, COLOR_PAIR(1));
		  mvwprintw(menu_win, 6+x, 3*y+6, "__");
		  wattroff(menu_win, COLOR_PAIR(1));
		}
	      if(grid[x][y].color == ccolor)
		{
		  wattron(menu_win, COLOR_PAIR(2));
		  mvwprintw(menu_win, 6+x, 3*y+6, "__");
		  wattroff(menu_win, COLOR_PAIR(2));
		}
	    }
	}
    }
  wrefresh(menu_win);
}

// Automatically generates an integer between 0 and 6
// If a column is full, then the integer of that column cannot be returned
int com_move(space grid[6][7], WINDOW *menu_win)
{
  int c = 0;
  
  wattron(menu_win, COLOR_PAIR(2));
  mvwprintw(menu_win, 5, 3*c + 6, "__");
  wattroff(menu_win, COLOR_PAIR(2));
  wmove(menu_win, 15,6);
  wrefresh(menu_win);
  usleep(100000);

  srand ( time(NULL) );
  c = rand() % 7;
  while(grid[0][c].taken == true)
    c = rand() % 7;

  for(int i = 0; i < c; i++)
    {
      mvwprintw(menu_win, 5, 3*i + 6, "__");
      wattron(menu_win, COLOR_PAIR(2));
      mvwprintw(menu_win, 5, 3*i + 9, "__");
      wmove(menu_win, 15, 6);
      wrefresh(menu_win);
      usleep(100000);
      wattroff(menu_win, COLOR_PAIR(2));
    }
  
  return c;
}

// Help user to select which column to drop a block
// Also does not allow an illegal move
// Returns the number of the column
int usr_move(space grid[6][7], WINDOW *menu_win, int pair)
{
  int ch = 0;
  int i = 0;

  wattron(menu_win, COLOR_PAIR(pair));
  mvwprintw(menu_win, 5, 3*i + 6, "__");
  wattroff(menu_win, COLOR_PAIR(pair));
  wmove(menu_win, 15,6);
  wrefresh(menu_win);
  

  

  while(ch != KEY_DOWN || grid[0][i].taken == true)
    {
      ostringstream s;
      string str;
      s <<ch;
      ch = getch();
      if(ch == KEY_LEFT && i > 0)
	{
	  mvwprintw(menu_win, 5, 3*i + 6, "__");
	  wattron(menu_win, COLOR_PAIR(pair));
	  mvwprintw(menu_win, 5, 3*i + 3, "__");
	  wattroff(menu_win, COLOR_PAIR(pair));
	  i--;
	}
      if(ch == KEY_RIGHT && i < 6)
	{
	  mvwprintw(menu_win, 5, 3*i + 6, "__");
	  wattron(menu_win, COLOR_PAIR(pair));
	  mvwprintw(menu_win, 5, 3*i + 9, "__");
	  wattroff(menu_win, COLOR_PAIR(pair));
	  i++;
	}
      wmove(menu_win, 15, 6);
      wrefresh(menu_win);
    }

  return i;
}

// Displays the animation of a color block dropping
void drop(space grid[6][7], int umove, int color, WINDOW *menu_win)
{
  for(int k = 0; k < 3; k++)
    {
      mvwprintw(menu_win, 5, 3*umove + 6, "__");
      wmove(menu_win, 15, 6);
      wrefresh(menu_win);
      usleep(100000);
      wattron(menu_win, COLOR_PAIR(color));
      mvwprintw(menu_win, 5, 3*umove + 6, "__");
      wmove(menu_win, 15, 6);
      wrefresh(menu_win);
      usleep(100000);
      wattroff(menu_win, COLOR_PAIR(color));
    }

  for(int i = 0; grid[i][umove].taken == false && i < 6; i++)
    {
      mvwprintw(menu_win, 5+i, 3*umove + 6, "__");
      wmove(menu_win, 15, 6);
      wrefresh(menu_win);
      wattron(menu_win, COLOR_PAIR(color));
      mvwprintw(menu_win, 6+i, 3*umove + 6, "__");
      wmove(menu_win, 15, 6);
      wrefresh(menu_win);
      usleep(100000);
      wattroff(menu_win, COLOR_PAIR(color));
    }
}


// Returns true if any player (including computer) wins the game
bool found_win(space grid[6][7], string color)
{
  int count = 0;
  for(int i = 0; i < 6; i++)
    {
      for(int j = 0; j < 7; j++)
	{
	  if( check(grid, i,j,color, count, 1,0) ||
	      check(grid, i,j,color, count, 0,1) ||
	      check(grid, i,j,color, count, 1,1) ||
	      check(grid, i,j,color, count, -1,1))
	    return true;
	}
    }
  return false;
}

// Subfunction used with in function bool found_win
bool check(space grid[6][7], int x, int y, string color, int &count, int sx, int sy)
{

  
  if(x < 0 | x > 5| y < 0| y> 6  )
    {
      return false;
    }
  else if(grid[x][y].color != color)
    return false;
  else
    {
      count++;
      check(grid, x+sx, y+sy, color, count, sx, sy);
      if(count ==4)
	{
	  return true;
	}
      else
	{
	  count = 0;
	  return false;
	}
    }

}

// Firework performance for start animation
void firework()
{
  attron(A_BOLD);
  struct winsize w;
  ioctl(0, TIOCGWINSZ, &w);
  string fire;
  int mid_x = w.ws_col / 2;
  int mid_y = w.ws_row / 2;
  int spaces = mid_x - 8;
  fire+="-- -==o0()";

  attron(COLOR_PAIR(3));
  mvprintw(mid_y, 0, "0");
  refresh();
  usleep(30000);
  clear();
  
  for(int i = 0; i < spaces; i++)
    {
      mvprintw(mid_y, i, fire.c_str());
      refresh();
      usleep(10000);
      clear();
    }


  attron(COLOR_PAIR(5));
  mvprintw(mid_y-1, mid_x-2, "o\\|/o");
  mvprintw(mid_y,   mid_x-2, ">>O<<");
  mvprintw(mid_y+1, mid_x-2, "o/|\\o");
  refresh();
  usleep(50000);
  clear();
  attroff(COLOR_PAIR(5));
  refresh();
  usleep(30000);

  
  mvprintw(mid_y-1, mid_x-2, "o\\|/o");
  mvprintw(mid_y,   mid_x-2, ">>O<<");
  mvprintw(mid_y+1, mid_x-2, "o/|\\o");
  refresh();
  usleep(50000);
  clear();
  refresh();
  usleep(30000);
  
  attron(COLOR_PAIR(5));
  mvprintw(mid_y-2, mid_x-3, "o *|* o");
  mvprintw(mid_y-1, mid_x-3, " \\o o/ ");
  mvprintw(mid_y,   mid_x-3, "*>- -<*");
  mvprintw(mid_y+1, mid_x-3, " /o o\\ ");
  mvprintw(mid_y+2, mid_x-3, "o *|* o");
  refresh();
  usleep(30000);
  clear();
  attroff(COLOR_PAIR(5));
  refresh();
  usleep(30000);

  attron(COLOR_PAIR(3));
  mvprintw(mid_y-2, mid_x-3, "o *|* o");
  mvprintw(mid_y-1, mid_x-3, " \\o o/ ");
  mvprintw(mid_y,   mid_x-3, "*>- -<*");
  mvprintw(mid_y+1, mid_x-3, " /o o\\ ");
  mvprintw(mid_y+2, mid_x-3, "o *|* o");
  refresh();
  usleep(30000);
  clear();
  attroff(COLOR_PAIR(3));
  refresh();
  usleep(30000);

  attroff(A_BOLD);
}

// Start animation before the menu shows up
void start()
{
  struct winsize w;
  
  ioctl(0, TIOCGWINSZ, &w);
  int mid_x = w.ws_col/2;
  int mid_y = w.ws_row/2;
  firework();
  usleep(200000);

  attron(COLOR_PAIR(4));
  mvprintw(mid_y, mid_x-3, "For CS 375");
  refresh();
  sleep(2);
  clear();
  attroff(COLOR_PAIR(4));
  
  firework();
  usleep(200000);

  attron(COLOR_PAIR(4));
  mvprintw(mid_y, mid_x-9, "Thanks to Dr. Hwang");
  refresh();
  sleep(2);
  clear();
  attroff(COLOR_PAIR(4));
  
  firework();
  usleep(200000);
  
  attron(COLOR_PAIR(4));
  mvprintw(mid_y, mid_x-9, "A Ruoqi Tian's Game");
  refresh();
  sleep(2);
  clear();
  attroff(COLOR_PAIR(4));
  
  
  for(int w = 0; w <3; w++)
    {
      wbkgd(stdscr, COLOR_PAIR(3));
      refresh();
      usleep(60000);
      wbkgd(stdscr, COLOR_PAIR(6));
      refresh();
      usleep(60000);
    }

}

// Returns true if there is a tie
bool tie(space grid[6][7])
{
  bool result = true;
  for(int i = 0; i < 6; i++)
    {
      for(int j = 0; j < 7; j++)
	{
	  if(grid[i][j].taken == false)
	    return false;
	}
    }
  return result;
}

// Returns the selection from main menu
// 0 = quick game
// 1 = single
// 2 = multiple
// 3 = exit
int menu_select(WINDOW *menu_win)
{
  int select = 0;
  int ch = 0;
  wattron(menu_win, A_BOLD);
  mvwprintw(menu_win, 9, 8, ">");
  wrefresh(menu_win);

  while(ch != KEY_RIGHT && ch != 10)
    {
      ch = getch();
      if(ch == KEY_UP && select > 0)
	{
	  mvwprintw(menu_win, 2*select+9, 8, " ");
	  mvwprintw(menu_win, 2*select+7, 8, ">");
	  wrefresh(menu_win);
	  select--;
	}
      if(ch == KEY_DOWN && select < 3)
	{
	  mvwprintw(menu_win, 2*select+9, 8, " ");
	  mvwprintw(menu_win, 2*select+11, 8, ">");
	  wrefresh(menu_win);
	  select++;
	}
    }
  wattroff(menu_win, A_BOLD);
  return select;
}

// Returns the color selection between red and yellow
int color_select(WINDOW *menu_win)
{
  int select = 0;
  int ch;
  wattron(menu_win, A_BOLD);
  mvwprintw(menu_win, 11, 12, ">");
  wrefresh(menu_win);

  while(ch != KEY_RIGHT && ch != 10)
    {
      ch = getch();
      if(ch == KEY_UP && select > 0)
	{
	  mvwprintw(menu_win, 2*select+11, 12, " ");
	  mvwprintw(menu_win, 2*select+9, 12, ">");
	  wrefresh(menu_win);
	  select--;
	}
      if(ch == KEY_DOWN && select < 1)
	{
	  mvwprintw(menu_win, 2*select+11, 12, " ");
	  mvwprintw(menu_win, 2*select+13, 12, ">");
	  wrefresh(menu_win);
	  select++;
	}
    }
  wattroff(menu_win, A_BOLD);
  return select;
}

// Looks for the contents with a specific key
int scan(char username[10], int &win, int &lose, int &draw)
{
  int i = 0;
  GDBM_FILE dbf;
  datum key, datain;

  struct keystr keydata;
  struct gamrec gamer;
  dbf = gdbm_open(".cfour", 0, GDBM_READER, 0, 0);
  key = gdbm_firstkey(dbf);
  int k = 0;
  while(key.dptr != NULL)
    {
      keydata = *((struct keystr*)(key.dptr));
      datain = gdbm_fetch(dbf, key);
      if(keydata.type == GAMREC)
	{
	  gamer = *((struct gamrec*)(datain.dptr));
	  free(datain.dptr);
	  if(strcmp(gamer.username, username) == 0)
	    {
	      i = 1;
	      win = gamer.win;
	      lose = gamer.lose;
	      draw = gamer.draw;
	    }
	  
	}
      key = gdbm_nextkey(dbf, key);
    }
  gdbm_close(dbf);

  return i;
}

// Stores the data with a specific key into a database
void write(char username[10], int win, int lose, int draw)
{
  GDBM_FILE dbf;
  datum key, content, datain;

  struct keystr keydata;
  struct gamrec gamer;

  key.dptr = (char*)(&keydata);
  key.dsize = sizeof(keydata);

  dbf = gdbm_open(".cfour", 0, GDBM_WRCREAT, 0644, 0);
  strcpy(gamer.username, username);
  gamer.win = win;
  gamer.lose = lose;
  gamer.draw = draw;


  content.dptr = (char*)(&gamer);
  content.dsize = sizeof(gamer);
  strcpy(keydata.username, gamer.username);
  keydata.type = GAMREC;
  int store = gdbm_store(dbf, key, content, GDBM_REPLACE);
  gdbm_close(dbf);
}

// Start a game without login
int quick_game(WINDOW *menu_win)
{

	  mvwprintw(menu_win, 9, 8, "              ");
	  mvwprintw(menu_win, 11, 8, "              ");
	  mvwprintw(menu_win, 13, 8, "              ");
	  mvwprintw(menu_win, 15, 8, "              ");
	  wattron(menu_win, A_STANDOUT);
	  mvwprintw(menu_win, 9, 10, "CHOOSE COLOR");
	  wattroff(menu_win, A_STANDOUT);
	  mvwprintw(menu_win, 11, 14, "RED");
	  mvwprintw(menu_win, 13, 14, "YELLOW");
	  wrefresh(menu_win);
	  
	  string ucolor, ccolor;
	  int color = color_select(menu_win);
	  if(color == 0)
	    {
	      init_pair(1, COLOR_BLACK, COLOR_RED);
	      init_pair(2, COLOR_BLACK, COLOR_YELLOW);
	      ucolor = "red";
	      ccolor = "yellow";
	    }
	  else
	    {
	      init_pair(2, COLOR_BLACK, COLOR_RED);
	      init_pair(1, COLOR_BLACK, COLOR_YELLOW);
	      ccolor = "red";
	      ucolor = "yellow";
	    }
	  space grid[6][7];
	  for(int x = 0; x < 6; x++)
	    {
	      for(int y = 0; y < 7; y++)
		{
		  grid[x][y].taken = false;
		  grid[x][y].color = "None";
		}
	    }
	  
	  print_grid(grid, menu_win, ucolor, ccolor);
	  
	  int winner = 0;
	  while(winner == 0)
	    {
	      int umove = usr_move(grid, menu_win, 1);
	      drop(grid, umove, 1, menu_win);
	      int k = 0;
	      while(grid[k][umove].taken == false && k < 6)
		k++;
	      grid[k-1][umove].taken = true;
	      grid[k-1][umove].color = ucolor;
	      
	      if(found_win(grid, ucolor))
		{
		  winner = 1;
		  mvwprintw(menu_win, 15, 1, "You won!");
		  break;
		}
	      
	      int cmove = com_move(grid, menu_win);
	      drop(grid, cmove, 2, menu_win);
	      int j = 0;
	      while(grid[j][cmove].taken == false && j < 6)
		j++;
	      grid[j-1][cmove].taken = true;
	      grid[j-1][cmove].color = ccolor;
	      
	      if(found_win(grid, ccolor))
		{
		  winner = 2;
		  mvwprintw(menu_win, 15, 1, "You lost!");
		  break;
		}
	      
	      if(tie(grid))
		{
		  winner = 3;
		  mvwprintw(menu_win, 15, 1, "Tie!");
		  break;
		}
	  
	      print_grid(grid, menu_win, ucolor, ccolor);
	    }
	  return winner;

}

// Displays menu title animation
void menu_title(WINDOW* menu_win)
{
  
  string title1 = "CONNECT";
  string title2 = "FOU";
  
  wattron(menu_win, COLOR_PAIR(7));
  wattron(menu_win, A_BOLD);
  wattron(menu_win, A_UNDERLINE);
  for(int i = 0; i < 13; i++)
    {
      mvwprintw(menu_win, 5, i, title1.c_str());
      wrefresh(menu_win);
      usleep(10000);
      wclear(menu_win);
    }

  mvwprintw(menu_win, 5, 12, title1.c_str());
  wrefresh(menu_win);
  usleep(500000);
  wattroff(menu_win, COLOR_PAIR(7));
  wattroff(menu_win, A_BOLD);
  wattroff(menu_win, A_UNDERLINE);

  wattron(menu_win, COLOR_PAIR(6));
  wattron(menu_win, A_BOLD);
  for(int i = 0; i < 10; i++)
    {
      mvwprintw(menu_win, 6, 17, title2.c_str());
      wrefresh(menu_win);
      usleep(25000*i);
      mvwprintw(menu_win, 6, 17, "       ");
      wrefresh(menu_win);
      usleep(25000*i);
    }
  wattron(menu_win, A_UNDERLINE);
  mvwprintw(menu_win, 6, 17, "FOUR");
  wrefresh(menu_win);
  sleep(1);
  wattroff(menu_win, A_UNDERLINE);
  wattroff(menu_win, COLOR_PAIR(6));
  wattroff(menu_win, A_BOLD);

  box(menu_win, 0, 0);
  wattron(menu_win, A_STANDOUT);
  mvwprintw(menu_win, 9, 10, "QUICK  START");
  mvwprintw(menu_win, 11, 10, "SINGLEPLAYER");
  mvwprintw(menu_win, 13, 10, "MULTI-PLAYER");
  mvwprintw(menu_win, 15, 10, "    EXIT    ");
  wattroff(menu_win, A_STANDOUT);
  mvwprintw(menu_win, 18, 1, "Ruoqi Tian         v0.0.9 beta");
  wrefresh(menu_win);

}

// In multiplayer mode, function scan and write are called
// before and after each round of the game
int multi_game(WINDOW *menu_win, string username1, string username2)
{

  wattron(menu_win, A_STANDOUT);
  mvwprintw(menu_win, 9, 13, username1.c_str());
  mvwprintw(menu_win, 11, 13, username2.c_str());
  wattroff(menu_win, A_STANDOUT);
  wrefresh(menu_win);
  
  
  mvwprintw(menu_win, 13, 6, "Press ENTER to draw");
  int ent;
  while(ent != 10)
    {
      wtimeout(menu_win, 200);
      ent = wgetch(menu_win);
      wtimeout(menu_win, -1);
      if(ent == 10) break;
      wattron(menu_win, A_BOLD);
      mvwprintw(menu_win, 6, 7, "Draw for kick-off!");
      wrefresh(menu_win);
      wtimeout(menu_win, 400);
      ent = wgetch(menu_win);
      wtimeout(menu_win, -1);
      mvwprintw(menu_win, 6, 7, "                  ");
      wrefresh(menu_win);
      wattroff(menu_win, A_BOLD);
    }
  
  
  wattron(menu_win, A_BOLD);
  mvwprintw(menu_win, 6, 7, "Draw for kick-off!");
  wrefresh(menu_win);
  wattroff(menu_win, A_BOLD);
  
  
  mvwprintw(menu_win, 13, 6, "Press SPACE to stop");
  wrefresh(menu_win);
  wattron(menu_win, A_BOLD);
  int draw, kickoff;
  
  
  while(draw != ' ')
    {
      mvwprintw(menu_win, 11, 11, " ");
      mvwprintw(menu_win, 9, 11, "o");
      wrefresh(menu_win);
      wtimeout(menu_win, 100);
      draw = wgetch(menu_win);
      wtimeout(menu_win, -1);
      kickoff = 1;
      if(draw == ' ') break;
      mvwprintw(menu_win, 9, 11, " ");
      mvwprintw(menu_win, 11, 11, "o");
      wrefresh(menu_win);
      wtimeout(menu_win, 100);
      draw = wgetch(menu_win);
      wtimeout(menu_win, -1);
      kickoff = 2;
    }
  
  
  wattroff(menu_win, A_BOLD);
  
  wclear(menu_win);
  box(menu_win, 0, 0);
  wrefresh(menu_win);
  
  string kickoff_name;
  
  if(kickoff == 1)
    kickoff_name = username1 + " won the kick-off";
  if(kickoff == 2)
    kickoff_name = username2 + " won the kick-off";
  mvwprintw(menu_win, 5, ((30-kickoff_name.length())/2)+1, kickoff_name.c_str());
  mvwprintw(menu_win, 6, 14, "and");
  mvwprintw(menu_win, 7, 3, "the right to choose color");
  
  
  
  
  

  wrefresh(menu_win);
  wattron(menu_win, A_STANDOUT);
  mvwprintw(menu_win, 9, 10, "CHOOSE COLOR");
  wattroff(menu_win, A_STANDOUT);
  mvwprintw(menu_win, 11, 14, "RED");
  mvwprintw(menu_win, 13, 14, "YELLOW");
  wrefresh(menu_win);
  
  string ucolor, ccolor;
  int color = color_select(menu_win);
  
  space grid[6][7];
  for(int x = 0; x < 6; x++)
    {
      for(int y = 0; y < 7; y++)
	{
	  grid[x][y].taken = false;
	  grid[x][y].color = "None";
	}
    }
	  
  print_grid(grid, menu_win, "red", "yellow");
  
  
  if(color == 0)
    {
      init_pair(1, COLOR_BLACK, COLOR_RED);
      init_pair(2, COLOR_BLACK, COLOR_YELLOW);
      ucolor = "red";
      ccolor = "yellow";
    }
  else
    {
      init_pair(2, COLOR_BLACK, COLOR_RED);
      init_pair(1, COLOR_BLACK, COLOR_YELLOW);
      ccolor = "red";
      ucolor = "yellow";
    }
  
  int winner = 0;
  while(winner == 0)
    {
      
      if(kickoff == 1)
	mvwprintw(menu_win, 15, 1, "%s's turn", username1.c_str());
      else
	mvwprintw(menu_win, 15, 1, "%s's turn", username2.c_str());
      wrefresh(menu_win);
      int umove = usr_move(grid, menu_win,1);
      mvwprintw(menu_win, 15, 1, "                    ");
      wrefresh(menu_win);
      drop(grid, umove, 1, menu_win);
      int k = 0;
      while(grid[k][umove].taken == false && k < 6)
	k++;
      grid[k-1][umove].taken = true;
      grid[k-1][umove].color = ucolor;
      
      if(found_win(grid, ucolor))
	{
	  winner = 1;
	  
	  break;
	}
      
      if(kickoff == 2)
	mvwprintw(menu_win, 15, 1, "%s's turn", username1.c_str());
      else
	mvwprintw(menu_win, 15, 1, "%s's turn", username2.c_str());
      wrefresh(menu_win);
      
      
      
      int cmove = usr_move(grid, menu_win,2);
      
      
      mvwprintw(menu_win, 15, 1, "                    ");
      wrefresh(menu_win);
      drop(grid, cmove, 2, menu_win);
      int j = 0;
      while(grid[j][cmove].taken == false && j < 6)
	j++;
      grid[j-1][cmove].taken = true;
      grid[j-1][cmove].color = ccolor;
      
      if(found_win(grid, ccolor))
	{
	  winner = 2;
	  break;
	}
      
      if(tie(grid))
	{
	  winner = 3;
	  break;
	}
      
      print_grid(grid, menu_win, ucolor, ccolor);
    }
  
  wrefresh(menu_win);
  
  
  wmove(menu_win, 1, 1);
  wclrtoeol(menu_win);
  wmove(menu_win, 2, 1);
  wclrtoeol(menu_win);
  box(menu_win, 0, 0);
  
  if(winner == 3) return 3;
  
  else if(kickoff == 1)
    {
      if(winner == 1)
	return 1;
      else return 2;
    }
  else
    {
      if(winner == 1)
	return 2;
      else return 1;
    }
  
}
