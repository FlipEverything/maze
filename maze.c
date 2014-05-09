#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>

typedef struct square {
	char borderN;
	char borderE;
} square;

typedef struct maze {
	int width;
	int height;
	/*
 	 * squares[0] is top left
 	 * squares[1] is 2nd left on the top
 	 * ...
 	 * squares[width*height-2] is 2nd right on the bottom
 	 * squares[width*height-1] is bottom right
	 */
	square* squares;
} maze;

maze *new_maze(int width, int height) {
	maze* maze = malloc(sizeof(maze));
	maze->width = width;
	maze->height = height;
	maze->squares = malloc(width*height*sizeof(square));
	return maze;
}

void free_maze(maze *maze) {
	free(maze->squares);
	free(maze);
}

void print_maze(maze *maze, FILE *f) {
	for (int y=0; y < maze->height; y++) {
		// top border
		for (int x=0; x < maze->width; x++) {
            fprintf(f, "1");
			if (maze->squares[maze->width*y +x].borderN) {
                fprintf(f, "1");
			} else {
                fprintf(f,"0");
			}
		}
        fprintf(f, "1\n");
		// East border and blank for square
		for (int x=0; x < maze->width; x++) {
			if (maze->squares[maze->width*y +x].borderE) {
                fprintf(f,"1");
			} else {
                fprintf(f,"0");
			}
            fprintf(f,"0");
		}
        fprintf(f,"1\n");
	}
	// Very bottom border
	for (int x=0; x < maze->width-1; x++) {
        fprintf(f,"11");
	}
    fprintf(f, "101\n");
}

void remove_maze_boundary(maze *m, int square_a, int square_b) {
	if (square_a > square_b) {
		int tmp = square_a;
		square_a = square_b;
		square_b = tmp;
	}
	if (square_b - square_a == 1) {
		m->squares[square_b].borderE = 0;
	} else {
		m->squares[square_b].borderN = 0;
	}
}

void create_passages(maze *m) {
	// Set all borders to on
	memset(m->squares, 1, m->width*m->height*sizeof(square));
	GArray *explored = g_array_sized_new(FALSE, FALSE, sizeof(int), m->width*m->height);
	int i =0;
	g_array_append_val(explored, i);
	enum square_status {EXPLORED=0, UNEXPLORED=1};
	char square_statuses[m->width * m->height];
	memset(square_statuses, (int)UNEXPLORED, sizeof(square_statuses));
	square_statuses[0] = EXPLORED;
	m->squares[0].borderN = 0;

	while( explored->len ) {
		int explored_index = g_random_int_range(0, explored->len);
		int explored_cell_index = g_array_index(explored, int, explored_index);
		int y = explored_cell_index/m->width;
		int x = explored_cell_index - y*m->width;
		GArray *bordering_squares = g_array_sized_new(FALSE, FALSE, sizeof(int), 4);
		int bordering_square;
		if (y>0) {
			bordering_square = explored_cell_index - m->width;
			if (square_statuses[bordering_square] == UNEXPLORED) {
				g_array_append_val(bordering_squares, bordering_square);
			}
		}
		if (x< m->width-1) {
			bordering_square = explored_cell_index +1;
			if (square_statuses[bordering_square] == UNEXPLORED) {
				g_array_append_val(bordering_squares, bordering_square);
			}
		}
		if (y< m->height-1) {
			bordering_square = explored_cell_index + m->width;
			if (square_statuses[bordering_square] == UNEXPLORED) {
				g_array_append_val(bordering_squares, bordering_square);
			}
		}
		if (x > 0) {
			bordering_square = explored_cell_index -1;
			if (square_statuses[bordering_square] == UNEXPLORED) {
				g_array_append_val(bordering_squares, bordering_square);
			}
		}
		if (bordering_squares->len==0) {
			g_array_remove_index_fast(explored, explored_index);
		} else {
			bordering_square = g_array_index(
				bordering_squares, int, g_random_int_range(0, bordering_squares->len)
			);
			remove_maze_boundary(m, bordering_square, explored_cell_index);
			g_array_append_val(explored, bordering_square);
			square_statuses[bordering_square] = EXPLORED;
		}
		g_array_free(bordering_squares, TRUE);
	}

	g_array_free(explored, TRUE);
}

int main(int argc, char **argv) {
	int width = (argc < 3) ? 0 : atoi(argv[1]);
	int height = (argc < 3) ? 0 : atoi(argv[2]);
	if (width < 2) width = 10;
	if (height < 2) height = 10;
	maze *m = new_maze(width, height);
    FILE *f = fopen("maze.txt","w");
	create_passages(m);
    fprintf(f,"%d %d\n", width*2+1, height*2+1);
    print_maze(m, f);
	free_maze(m);
    fclose(f);
	return 0;
}

