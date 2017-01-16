#include "maze_gen_dfs.h"
#include "helper_functions.h"
#include <stdlib.h>

maze gen_maz_dfs(int width,int height,int sx,int sy)
{
	maze ans;
	ans.width=width;
	ans.height=height;
	
	ans.c=(maze_cell**)malloc(sizeof(maze_cell*)*width);
	for(int i=0;i<width;i++)
		ans.c[i]=(maze_cell*)malloc(sizeof(maze_cell)*height);
	
	for(int i=0;i<width;i++)
		for(int j=0;j<height;j++){
			ans.c[i][j].walls=all;
			ans.c[i][j].visited=0;
			ans.c[i][j].icon=' ';
		}
			
	int cx=sx,cy=sy,nx,ny;
	ans.c[cx][cy].visited=1;

	int order[4]={0,1,2,3};
	int delta[4][2]={{-1,0},{+1,0},{0,-1},{0,+1}};
	wall delta_k[4][2]={{left,right},{right,left},{top,bottom},{bottom,top}};
		
	
	int **S = (int**)malloc(width*height*sizeof(int*));
	for(int i=0;i<width*height;i++)
		S[i]=(int*)malloc(2*sizeof(int));
	int Stop=-1;
	
	
	while(1){
		random_shuffle(order,0,3);
		
		char found=0;
		for(int i=0;i<4;i++){
			nx=cx+delta[order[i]][0];
			ny=cy+delta[order[i]][1];
			if(nx>=0 && ny>=0 && nx<width && ny<height)
				if(!ans.c[nx][ny].visited){
					
					S[++Stop][0]=cx;
					S[Stop][1]=cy;
					
					ans.c[cx][cy].walls = (wall)(ans.c[cx][cy].walls & ~delta_k[order[i]][0]);
					ans.c[nx][ny].walls = (wall)(ans.c[nx][ny].walls & ~delta_k[order[i]][1]);
					cx=nx;
					cy=ny;
					ans.c[cx][cy].visited=1;
					found=1;
				break;
				}
		}
		
		if(!found)
			if(Stop>-1){
				cx=nx=S[Stop][0];
				cy=nx=S[Stop--][1];
			}else
				break;
	}
	
	return ans;
}
