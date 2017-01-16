#ifndef __maze_included__
#define __maze_included__

enum wall { 
	left=1<<0,
	right=1<<1,
	top=1<<2,
	bottom=1<<3,
	all=(1<<4)-1 
};


typedef struct {
	wall walls ;
	char visited;
	char icon;
} maze_cell;

typedef struct {
	int width;
	int height;
	maze_cell** c; 
} maze;

void print_maze(maze m);

#endif //__maze_included__
