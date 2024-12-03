#include <iostream>
#include <string>
#include <ncurses.h>
#include <locale.h>
#include <stdlib.h>
#include <unistd.h>

#define CONTROL_UP	'i'
#define CONTROL_DOWN	'k'
#define CONTROL_LEFT	'j'
#define CONTROL_RIGHT	'l'
#define CONTROL_NEXT	's'
#define CONTROL_BACK	'a'
#define CONTROL_ENTER '\n'
#define MITEMS		6
#define NO_LEVELS	10
#define GAMESPEED 1000000

using namespace std;

WINDOW *menuBoard, *title, *board;

int saveData[20][10];
void start();
int menuScreen(int menuNum);
void startGame();
int randomShape(int a, int b);

char TETRIS[] = "▣▣▣▣▣▣ ▣▣▣▣▣▣ ▣▣▣▣▣▣ ▣▣▣▣   ▣▣▣▣ ▣▣▣▣▣\n  ▣▣   ▣▣       ▣▣   ▣   ▣   ▣▣  ▣\n  ▣▣   ▣▣▣▣▣▣   ▣▣   ▣▣▣▣    ▣▣  ▣▣▣▣▣\n  ▣▣   ▣▣       ▣▣   ▣   ▣   ▣▣      ▣\n  ▣▣   ▣▣▣▣▣▣   ▣▣   ▣    ▣ ▣▣▣▣ ▣▣▣▣▣\n";

struct Cor{
	int y;
	int x;
};

class Mino
{
private:
	enum
    {
        MinoI = 0,
        MinoO = 1,
        MinoL = 2,
		MinoS = 3,
		MinoJ = 4,
 		MinoZ = 5,
		MinoT = 6,
    };
public:
	int shape;
	struct Cor pos;
	struct Cor minoList[7][4] = { 
		{ {0,0}, {0,-2}, {0,-1}, {0,1} },   //I
		{ {0,0}, {0,-1}, {-1,0}, {-1,-1} }, //O
		{ {0,-1}, {0,0}, {0,1}, {-1,1} },   //L
		{ {0,-1}, {0,0}, {-1,0}, {-1,1} },  //S
		{ {-1,-1}, {0,-1}, {0,0}, {0,1} },  //J
		{ {-1,-1}, {-1,0}, {0,0}, {0,1} },  //Z
		{ {0,-1}, {0,0}, {-1,0}, {0,1} }    //T
	};
	Mino(){
		shape = randomShape(0,6);
		pos.y = 0;
		pos.x = 5;
	}
	bool isCollapse(int arr[20][10], int y, int x);
};

bool Mino::isCollapse(int arr[20][10], int y, int x){
	for(int i = 0; i < 4; i++){
		Cor blockPos = this->minoList[this->shape][i];
	}
	return false;
}

int randomShape(int a, int b){
	srand((unsigned int)time(NULL));
	return (int)rand()%(b-a) + a;
}

void start(){
	initscr();
	noecho();
	curs_set(0);
	start_color();
	init_pair(1, COLOR_BLACK, COLOR_WHITE);
	init_pair(2, COLOR_WHITE, COLOR_BLACK);

	refresh();
	title = newwin(5,42,2,10);
	wprintw(title, TETRIS);
	wrefresh(title);

	int menu = menuScreen(0);
	switch(menu){
		case 0:
			startGame();
			break;
		case 1:
			//saveFile();
			break;
		case 2:
			//showInst();
			break;
		case 3:
			//scoreBoard();
			break;
		case 4:
			//showDev();
			break;
		default:
			printf("Exit!");
			return;
	}
	return;
}

void startGame(){
	int status[20][10] = {0,};
	WINDOW * mainw = newwin(12, 12, 10, 14);
	int gameLevel = 0;
	Mino block;
	Cor curPos = block.pos;
	while(!block.isCollapse(status, curPos.y, curPos.x)){
		box(mainw, '|', '-');
		for(int i = 1; i <= 10; i++){
			for(int j = 1; j <= 20; j++){
				if(status[i][j] > 0){
					mvwprintw( mainw, i, j, "■");
				}
			}
		}
		for(int i = 0; i < 4; i++){
			Cor blockPos = block.minoList[block.shape][i];
			mvwprintw( mainw, curPos.y + blockPos.y, curPos.x + blockPos.x, "■");
		}
		wrefresh( mainw );
		werase(mainw);
		usleep(GAMESPEED - 10 * gameLevel);
		if(!block.isCollapse(status, curPos.y+1, curPos.x)) curPos.y++;
		else break; 
	}
	mvwprintw( mainw, 5, 4, "GAMEOVER!!");
	getch();
	werase( mainw );
	delwin( mainw );

}

int menuScreen(int menuNum){
	menuBoard = newwin(15,25,10,14);
	box(menuBoard, '|', '-');

	int i;
	char item[20];
	char list[MITEMS][20] = {	"New Game",
				"Load/Delete Game",
				"Instructions",
				"Score board",
				"About Developers",
				"Exit" };
    while(true){
		// now print all the menu items and highlight the first one
		for( i = 0; i < MITEMS; i++ ) {
			snprintf(item, sizeof(item), "%s",  list[i]);
			if(i == menuNum) wattron(menuBoard, COLOR_PAIR(1));
			mvwprintw( menuBoard, i+1, 2, "%s", item );
			wattroff(menuBoard, COLOR_PAIR(1));

		}

		mvwprintw( menuBoard, 11, 2, "'%c' -> Up", CONTROL_UP);
		mvwprintw( menuBoard, 12, 2, "'%c' -> Down", CONTROL_DOWN);
		mvwprintw( menuBoard, 13, 2, "Press '%c' to select ", CONTROL_NEXT );

		wrefresh( menuBoard ); // update the terminal screen
		char inp;
		
		inp = getch();

		if(inp == CONTROL_DOWN){
			if(menuNum < 5) menuNum++;
		}else if(inp == CONTROL_UP){
			if(menuNum > 0) menuNum--;
		}else if(inp == CONTROL_ENTER || inp == CONTROL_NEXT){
			break;
		}
	}
	werase( menuBoard );
	wrefresh( menuBoard );
	delwin( menuBoard );
	return menuNum;
}

int main(){
	setlocale(LC_CTYPE, "ko_KR.utf-8");
	start();
	endwin();
	return 0;
}

