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
#define GAMESPEED 50000

#define MINO_I_COLOR 103
#define MINO_O_COLOR 104
#define MINO_L_COLOR 105
#define MINO_S_COLOR 106
#define MINO_J_COLOR 107
#define MINO_Z_COLOR 108
#define MINO_T_COLOR 109

using namespace std;

WINDOW *menuBoard, *title, *board;

struct Cor;
class Mino;
int saveData[20][10];
void start();
int menuScreen(int menuNum);
void startGame();
void setColor();
int randomShape(int a, int b);
int minoColor[7] = {MINO_I_COLOR, MINO_O_COLOR, MINO_L_COLOR, MINO_S_COLOR, MINO_J_COLOR, MINO_Z_COLOR, MINO_T_COLOR};
bool gameOver(WINDOW* win);
void drawMino( WINDOW* win, Cor* pos, Mino* mino, bool isErase);

char TETRIS[] = "TETRIS";
//"▣▣▣▣▣▣ ▣▣▣▣▣▣ ▣▣▣▣▣▣ ▣▣▣▣   ▣▣▣▣ ▣▣▣▣▣\n  ▣▣   ▣▣       ▣▣   ▣   ▣   ▣▣  ▣\n  ▣▣   ▣▣▣▣▣▣   ▣▣   ▣▣▣▣    ▣▣  ▣▣▣▣▣\n  ▣▣   ▣▣       ▣▣   ▣   ▣   ▣▣      ▣\n  ▣▣   ▣▣▣▣▣▣   ▣▣   ▣    ▣ ▣▣▣▣ ▣▣▣▣▣\n";

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
		pos.y = 1;
		pos.x = 5;
	}
	bool isCollapse(int arr[20][10], int y, int x, int dy, int dx);
};

bool Mino::isCollapse(int arr[20][10], int y, int x, int dy, int dx){
	for(int i = 0; i < 4; i++){
		Cor minoPos = this->minoList[this->shape][i];
		if(minoPos.y + y + dy > 20) return true;
		if(arr[minoPos.y+y+dy][minoPos.x+x+dx] > 0) return true;
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
	setColor();

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

void setColor(){
	start_color();
	init_color(MINO_I_COLOR, 2,241,241);
	init_color(MINO_L_COLOR, 240,162,0);
	init_color(MINO_T_COLOR, 162,0,241);
	init_pair(1, COLOR_BLACK, COLOR_WHITE);
	init_pair(2, COLOR_WHITE, COLOR_BLACK);
	init_pair(MINO_I_COLOR, MINO_I_COLOR, MINO_I_COLOR);
	init_pair(MINO_J_COLOR, COLOR_BLUE, COLOR_BLUE);
	init_pair(MINO_L_COLOR, MINO_L_COLOR, MINO_L_COLOR);
	init_pair(MINO_O_COLOR, COLOR_YELLOW, COLOR_YELLOW);
	init_pair(MINO_S_COLOR, COLOR_GREEN, COLOR_GREEN);
	init_pair(MINO_T_COLOR, MINO_T_COLOR, MINO_T_COLOR);
	init_pair(MINO_Z_COLOR, COLOR_RED, COLOR_RED);
	return;
}

void startGame(){
	int status[20][10] = {0,};
	WINDOW * mainw = newwin(22, 22, 10, 14);
	int gameLevel = 0;

	//draw loaded game status
	box(mainw, ACS_VLINE, ACS_HLINE);
	for(int i = 0; i < 20; i++){
		for(int j = 0; j < 10; j++){
			int minoShape = status[i][j];
			if(minoShape > 0){
				int color = minoColor[minoShape];
				wattron( mainw, COLOR_PAIR(color));
				mvwprintw( mainw, i, j, "  ");
				wattroff( mainw, COLOR_PAIR(color));
			}
		}
	}
	
	while(true){ //gameOver( mainw )){
		Mino block;
		Cor curPos = block.pos;
		while(true){
			drawMino( mainw, &curPos, &block, 0);
			usleep(GAMESPEED - 10 * gameLevel);
			int movY, movX;
			movY = 1;
			movX = 0;
			if(!block.isCollapse(status, curPos.y, curPos.x, movY, movX)) {
				drawMino( mainw, &curPos, &block, 1);
				curPos.y += movY;
				curPos.x += movX;
			}else{
				for(int i = 0; i < 4; i++) {
					status[curPos.y+block.minoList[block.shape][i].y][curPos.x+block.minoList[block.shape][i].x] = block.shape;
				}
				break;
			}
		}
		if(curPos.y == block.pos.y) break;
	}
	gameOver( mainw );
	getch();
	werase( mainw );
	delwin( mainw );

}

bool gameOver(WINDOW* win){
	mvwprintw( win, 5, 4, "GAMEOVER!!");
	wrefresh(win);
	getch();
	return true;
}

void drawMino( WINDOW* win, Cor* pos, Mino* mino, bool isErase){
		for(int i = 0; i < 4; i++){
			Cor blockPos = mino->minoList[mino->shape][i];
			int color = minoColor[mino->shape];
			wattron( win, isErase? 0 : COLOR_PAIR(color));
			mvwprintw( win, pos->y + blockPos.y, 2*(pos->x + blockPos.x), "  ");
			wattroff( win, isErase? 0 : COLOR_PAIR(color));
		}
		if(!isErase) wrefresh( win );
		return;
}

int menuScreen(int menuNum){
	menuBoard = newwin(15,25,10,14);
	box(menuBoard, ACS_VLINE, ACS_HLINE);

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

