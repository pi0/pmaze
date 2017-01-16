/*
	pmaze
	A simple maze generator and solver
	Written by pooya parsa ( pyapar@gmail.com )
	Feel free to use my code :)
*/

#include "maze.h"
#include "maze_gen_dfs.h"
#include <time.h>
#include <stdlib.h>

int main()
{
	srand(time(NULL));
	maze a=gen_maz_dfs(16,16,0,0);
	print_maze(a);
}