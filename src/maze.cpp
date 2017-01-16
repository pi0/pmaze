#include "maze.h"
#include <stdio.h>

void print_maze(maze m)
{
	for(int i=0;i<m.height;i++){
		//Top
		for(int j=0;j<m.width;j++)
			if(m.c[j][i].walls&top || i==0)
				printf("+---");
			else
				printf("+   ");
		printf("+\n");
		//Current
		for(int j=0;j<m.width;j++){
			if(m.c[j][i].walls&left || j==0) printf("|");
			else printf(" ");
			printf(" %c ",(m.c[j][i].icon=='\0')?' ':m.c[j][i].icon);
			}
		printf("|\n");	
	}
	//Bottom
	for(int j=0;j<m.width;j++)
		printf("+---");
	
	printf("+\n");
}