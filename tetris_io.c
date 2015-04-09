/*
  
  Teris (Ein-/Ausgaben via mcurses)
  =================================
          Uwe Berger; 2015


  ---------
  Have fun! 
 
 *----------------------------------------------------------------------
 * Copyright (c) 2015 Uwe Berger - bergeruw(at)gmx.net
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *----------------------------------------------------------------------
*/


#include "tetris_io.h"


// ********************************************
void mcurses_create_game_screen(void)
{
	int i;
	initscr();
	// Ego befriedigen...
	attrset(A_REVERSE);
	mvaddstr(0,0, "     Tetris; Uwe Berger; 2015     ");
	attrset(A_NORMAL);
	// Spielfeld zeichnen...
	for (i=GRID_Y; i<(GRID_Y+GRID_DY); i++) {
		mvaddch(i, GRID_X-1, ACS_VLINE);
		mvaddch(i, GRID_X+GRID_DX, ACS_VLINE);
	}
	mvaddch(GRID_Y+GRID_DY, GRID_X-1, ACS_LLCORNER);
	mvaddch(GRID_Y+GRID_DY, GRID_X+GRID_DX, ACS_LRCORNER);
	for (i=GRID_X; i<GRID_X+GRID_DX; i++) {
		mvaddch(GRID_Y+GRID_DY, i, ACS_HLINE);
	}
	mvaddstr(SCORE_Y, SCORE_X, "Next:");
	mvaddstr(SCORE_Y+5, SCORE_X, "Level:");
	mvaddstr(SCORE_Y+8, SCORE_X, "Bricks:");
	mvaddstr(SCORE_Y+11, SCORE_X, "Lines:");
	mvaddstr(SCORE_Y+14,  SCORE_X, "Points:");
	mvaddstr(SCORE_Y+17,  SCORE_X, "Time [s]:");
	curs_set(0);
}

// ********************************************
void mcurses_destroy_game_screen(void)
{
	clear();
	curs_set(1);
	endwin();
}

// ********************************************
void mcurses_draw_score(score_t *s)
{
	char buf[10];
	//...Zeit liegt in 100tel Sekunden vor
	sprintf(buf, "%u,%02u", (unsigned int)(s->game_time/100), (unsigned int)(s->game_time%100));
	mvaddstr(SCORE_Y+18, SCORE_X, "          ");		
	mvaddstr(SCORE_Y+18, SCORE_X, buf);
	// Level
	sprintf(buf, "%u", s->level);
	mvaddstr(SCORE_Y+6, SCORE_X, "        ");		
	mvaddstr(SCORE_Y+6, SCORE_X, buf);
	// Anzahl versenkte Spielsteine
	sprintf(buf, "%u", s->bricks);
	mvaddstr(SCORE_Y+9, SCORE_X, "        ");		
	mvaddstr(SCORE_Y+9, SCORE_X, buf);
	// Anzahl abgeraumte Zeilen
	sprintf(buf, "%u", s->lines);
	mvaddstr(SCORE_Y+12, SCORE_X, "        ");		
	mvaddstr(SCORE_Y+12, SCORE_X, buf);
	// Punkte
	sprintf(buf, "%u", s->points);
	mvaddstr(SCORE_Y+15, SCORE_X, "        ");		
	mvaddstr(SCORE_Y+15, SCORE_X, buf);
	// Ausgabe Pause bzw. Spielende
	if (s->is_pause||s->game_over) {
		attrset(B_RED);
		if (s->is_pause) {
			mvaddstr(SCORE_Y+20, SCORE_X, "Play pause!");
		} else {
			mvaddstr(SCORE_Y+20, SCORE_X, "Game over!");
		}
		attrset(A_NORMAL);
	} else {
		mvaddstr(SCORE_Y+120, SCORE_X, "           ");		
	}
}

// ********************************************
void mcurses_clear_brick(brick_t *b) 
{
	int i, j;
	for (i=0; i < b->dxy; i++) {
		for (j=0; j < b->dxy; j++) {
			if (b->grid[i][j]) {
				attrset(A_NORMAL);
				mvaddch((b->y+GRID_Y+i), (b->x+GRID_X+j), ' ');
			}
		}
	}
}

// ********************************************
static void mcurses_set_color(int c)
{
	switch (c) {
		case BLANK:    attrset(A_NORMAL);	break;
		case GREEN:    attrset(B_GREEN);	break;
		case BLUE :    attrset(B_BLUE);		break;
		case YELLOW:   attrset(B_WHITE);	break;
		case RED:      attrset(B_RED);		break;
		case BROWN:    attrset(B_BROWN);	break;
		case MAGENTA:  attrset(B_MAGENTA);	break;
		case CYAN:     attrset(B_CYAN);		break;
	}
}


// ********************************************
void mcurses_draw_brick(brick_t *b) 
{
	int i, j;
	for (i=0; i < b->dxy; i++) {
		for (j=0; j < b->dxy; j++) {
			mcurses_set_color(b->grid[i][j]);
			if (b->grid[i][j]) mvaddch((b->y+GRID_Y+i), (b->x+GRID_X+j), ' '); 
			attrset(A_NORMAL);
		}	
	}
}

// ********************************************
void mcurses_draw_next_brick(brick_t *b)
{
	int i, j;
	for (i=0; i<MAX_DXY; i++) {
		for (j=0; j<MAX_DXY; j++) {
			mcurses_set_color(b->grid[i][j]);
			mvaddch((SCORE_Y+i+1), (SCORE_X+j), ' '); 
			attrset(A_NORMAL);			
		}	
	}
}

// ********************************************
void mcurses_draw_grid(grid_t *g) 
{
	int i, j;
	for (i=0; i<GRID_DX; i++) {
		for (j=0; j<GRID_DY; j++) {
			mcurses_set_color(g->grid[i][j]);
			if (g->grid[i][j]) {
				mvaddch((GRID_Y+j), (GRID_X+i), ' '); 
			} else {
				attrset(A_NORMAL);
				mvaddch((GRID_Y+j), (GRID_X+i), ' '); 
			}
			attrset(A_NORMAL);
		}	
	}
}

// ********************************************
int mcurses_get_key(void)
{
	int ret;
	switch(getch()) {
		case 'q': 
		case 'Q':
		case KEY_F(9):
				ret=KEY_QUIT_GAME;	
				break;
		case 'p': 
		case 'P':
				ret=KEY_PAUSE_GAME;	
				break;
		case 'n': 
		case 'N':
				ret=KEY_NEW_GAME;	
				break;
		case KEY_DOWN:
				ret=KEY_BRICK_DROP;	
				break;
		case KEY_UP:
				ret=KEY_BRICK_ROTATE;	
				break;
		case KEY_LEFT:
				ret=KEY_BRICK_LEFT;	
				break;
		case KEY_RIGHT:
				ret=KEY_BRICK_RIGHT;	
				break;
		default:
				ret=KEY_UNKNOWN;	
				break;
	}	
	return ret;
}

