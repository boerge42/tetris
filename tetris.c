/*
  
         Teris
  ======================
     Uwe Berger; 2015
 
 
 Interessante Links zum Thema Tetris:
 ------------------------------------
 --> http://www.colinfahey.com/tetris/tetris.html
 
 
  ---------
  Have fun! 
 
 
 ToDo:
 -----
 --> Highscore
 
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

#include "timer.h"
#include "tetris.h"
#include "tetris_io.h"
#include "random.h"


static brick_t brick, next_brick;
static grid_t grid;
static score_t score = {1, 0, 0, 0, 0};
static int drop_key_pressed=0;

// ********************************************
void timer_handler(void) 
{
	brick_t newbrick;
	static int free_iterations=0;
	// Spielzeit
	if (!drop_key_pressed) 
		score.game_time = score.game_time + compute_iteration_time(&score)/10;
	draw_score(&score);
	// Stein eine Zeile runter?
	newbrick=brick;
	newbrick.y++;
	if (!collision(&newbrick, &grid)) {
			// ...ja, passt hin
			clear_brick(&brick);
			draw_brick(&newbrick);
			brick=newbrick;
			if (!drop_key_pressed) free_iterations++;											
	} else {
		// ...nein, Endlage erreicht!
		stop_timer();
		// Spielstein in Spielfeld uebernehmen
		copy_brick2grid(&brick, &grid);
		// Spielfeld auf vollständige Zeilen pruefen, ausgeben, Spielstand
		score.lines = score.lines + delete_full_lines(&grid);
		draw_grid(&grid);
		// Spielstand berechnen/ausgeben, Schalter initialisieren
		score.points=score.points+((21+(3*score.level))-free_iterations);
		compute_level(&score);
		draw_score(&score);
		free_iterations=0;
		drop_key_pressed=0;
		// neuen Spielstein generieren/anzeigen
		brick=next_brick;
		score.bricks++;
		next_brick=generate_new_brick();
		draw_next_brick(&next_brick);	
		if (collision(&brick, &grid)) {
			score.game_over=1;
			draw_score(&score);
		} else {
			draw_brick(&brick);	
			// ...und Timer wieder starten
			start_timer(compute_iteration_time(&score), &timer_handler);
		}
	}
}

// ********************************************
void game_init(void)
{
	int i, j;
	stop_timer();
	// Spielstaende nullen
	score.level=1;
	score.bricks=0;
	score.lines=0;
	score.points=0;
	score.game_time=0;
	score.is_pause=0;
	score.game_over=0;
	// Spielfeld initialisieren
	for (i=0; i<GRID_DX; i++) {
		for (j=0; j<GRID_DY; j++) {
			grid.grid[i][j]=BLANK;
		}	
	}
	// Flags initialisieren
	drop_key_pressed=0;
	// Spielfeld/Spielstand ausgeben
	draw_grid(&grid);
	draw_score(&score);
	// einen Spielstein generieren
	brick=generate_new_brick();
	score.bricks++;
	next_brick=generate_new_brick();
	draw_next_brick(&next_brick);
	draw_brick(&brick);
	// Timer starten
	start_timer(compute_iteration_time(&score), &timer_handler);
}

// ********************************************
int compute_iteration_time(score_t *s) 
{
	return (500-(s->level-1)*50);	
}

// ********************************************
void compute_level(score_t *s)
{
	if (s->lines <= 0) {
		s->level = 1;
	} else if ((s->lines >= 1) && (s->lines <= 90)) {
				s->level = 1 + ((s->lines - 1) / 10);
			} else if (s->lines >= 91) {
  						s->level = 10;
					}	
}


// ********************************************
int delete_full_lines(grid_t *g)
{
	int count=0;
	int full;
	int i, j, k;
	for (j=0; j<GRID_DY; j++) {
		full=1;
		for (i=0; i<GRID_DX; i++) { 
			if (g->grid[i][j]==BLANK) full=0;
		}
		// volle Zeile gefunden
		if (full) {
			count++;
			// drueber liegende Zeilen nach unten verschieben
			for (k=j; k>-1; k--) {
				for (i=0; i<GRID_DX; i++) {
					if (k>0) {
						g->grid[i][k]=g->grid[i][k-1];
					} else {
						g->grid[i][k]=BLANK;
					}
				}
			}
			// Zeile muss nochmal untersucht werden
			//j++;
		}
	}
	return count;
}

// ********************************************
brick_t generate_new_brick(void) 
{
	return default_bricks[generate_random_number()];
}

// ********************************************
void copy_brick2grid(brick_t *b, grid_t *g)
{
	int i, j;
	for (i=0; i < b->dxy; i++) {
		for (j=0; j < b->dxy; j++) {
			// nur uebernehmen, wenn Spielsteinfeld belegt
			if (b->grid[i][j]!=BLANK) 
				g->grid[(j+b->x)][(i+b->y)] = b->grid[i][j];
		}	
	}
}



// ********************************************
brick_t rotate_brick_cw(brick_t b)
{
	brick_t new_b=b;
	int i, j;
	for (i=0; i<b.dxy; i++) {
		for (j=0; j<b.dxy; j++) {
			new_b.grid[i][j]=b.grid[b.dxy-j-1][i];
		}	
	}
	return new_b;	
}


// ********************************************
int collision(brick_t *b, grid_t *g)
{
	int i, j;
	// alle Positionen des Spielsteines pruefen
	for (i=0; i < b->dxy; i++) {
		for (j=0; j < b->dxy; j++) {
			if (b->grid[i][j]!=BLANK) {
				// ...linke Seite
				if ((b->x+j)<0) return 1;
				// ...rechte Seite ((b->x+b->dxy-(b->dxy-j-1))>GRID_DX)
				if ((b->x+j+1)>GRID_DX) return 1;
				// ...oben
				if ((b->y+i)<0) return 1;
				// ...unten
				if ((b->y+i+1)>GRID_DY) return 1;
				// ...ein Feld belegt
				if (g->grid[b->x+j][b->y+i]!=BLANK) return 1;
			}
		}
	}
	// theoretisch fehlt noch eine Pruefung, ob bei
	// der Rotation Steine im Weg sind...!
	// Bei Drehung im Uhrzeigersinn:
	// * alter/neuer Ziegel zusammen 
	// * in den dann freien Feldern rechts oben und
	//   links unten darf kein Stein auf dem Spielfeld
	//   sein 
	return 0;
} 


// ********************************************
// ********************************************
// ********************************************
int main(void)
{
	brick_t newbrick;
	int run;
	
	// Initialisierung Zufallsgenerator
	random_init();
	
	// Spiel aufbauen
	create_game_screen();
	
	// Spiel initialisieren
	game_init();
	
	run=1;
	while (run) {
		// Tastensteuerung		
		switch (get_key()) {
			// Spielende
			case KEY_QUIT_GAME:
					run=0;
					break;
			// neues Spiel
			case KEY_NEW_GAME:
					game_init();
					break;
			// Spielpause
			case KEY_PAUSE_GAME:
					if (score.is_pause) {
						score.is_pause=0;
						start_timer(compute_iteration_time(&score), &timer_handler);
					} else {
						score.is_pause=1;
						stop_timer();
					}
					draw_score(&score);
					break;
			// Spielstein drehen
			case KEY_BRICK_ROTATE:
					if (score.is_pause || score.game_over) break;
					newbrick=rotate_brick_cw(brick);
					if (!collision(&newbrick, &grid)) {
						clear_brick(&brick);
						draw_brick(&newbrick);
						brick=newbrick;
					}
					break;
			// Spielstein runterfallen lassen
			case KEY_BRICK_DROP:
					if (score.is_pause || score.game_over) break;
					stop_timer();
					drop_key_pressed=1;
					// Schnelldurchlauf
					start_timer(5, &timer_handler);
					break;
			// Spielstein nach links
			case KEY_BRICK_LEFT:
					if (score.is_pause || score.game_over) break;
					newbrick=brick;
					newbrick.x--;
					if (!collision(&newbrick, &grid)) {
						clear_brick(&brick);
						draw_brick(&newbrick);
						brick=newbrick;											
					};
					break;
			// Spielstein nach rechts
			case KEY_BRICK_RIGHT:
					if (score.is_pause || score.game_over) break;
					newbrick=brick;
					newbrick.x++;
					if (!collision(&newbrick, &grid)) {
						clear_brick(&brick);
						draw_brick(&newbrick);
						brick=newbrick;											
					};
					break;
		}
	}
	
	stop_timer();
	destroy_game_screen();
	return 0;

}


