/*
	pmaze.cpp
	A simple maze generator and solver
	Written by pooya parsa ( pyapar@gmail.com )
	Feel free to use my code :)
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "SDL2\include\SDL.h"
#ifdef WIN32
/*--------------- Windows --------------- */

#include <windows.h>
#include <conio.h>

#define gotoxy(x,y) {COORD position={x,y};SetConsoleCursorPosition( GetStdHandle(STD_OUTPUT_HANDLE) , position );}

#else
/*--------------- Unix --------------- */

#include <unistd.h>
#define gotoxy(x,y) printf("\x1B[%d;%df", y, x);fflush(stdout); 
#define Sleep(t)	;		//TODO!!!
//TODO : Support for getch in linux!

#endif


/*=================================================
	Struct and enums
/*=================================================*/

enum wall { left=1<<0,right=1<<1,top=1<<2,bottom=1<<3,all=(1<<4)-1 };
typedef struct { wall walls ; char visited;char icon; int x; int y; } maze_cell;
typedef struct { int width ; int height ; maze_cell** c; } maze;





typedef struct queue_item queue_item;
struct queue_item{ 
	void *value;
	struct queue_item *next ; 
};

typedef struct { 
	queue_item *first_item;
	queue_item *last_item;
	int items_count;
}queue;


void QueueAdd ( queue &l , void *value )
{
	queue_item *new_item = (queue_item *)malloc(sizeof(queue_item));
	queue_item TT;
	*new_item=TT;
	
	new_item->value=value;
	
	if(l.items_count==0){
		l.first_item=new_item;
		l.last_item=new_item;
	}else{
		l.last_item->next=new_item;
		l.last_item=new_item;
	}
	l.items_count++;
}

void* QueuePop ( queue &l )
{
	if(l.items_count==0)return NULL;
	void* val=l.first_item->value;
	if ( (l.first_item=l.first_item->next)!=NULL)
		free(l.first_item);
	l.items_count--;
	return val;
}





/*=================================================
	Prototypes
/*=================================================*/

void random_shuffle(int*,int,int);
char is_inside_maze (int,int y,maze&);
void print_maze(maze&);
maze gen_maze_dfs(int,int);
char solve_maze_righthand(maze&,int,int,int,int);
char solve_maze_dfs(maze&,int,int,int,int);
char solve_maze_bfs(maze&,int,int,int,int);
void maze_editor(maze&);
void maze_init (maze&,int,int,wall);
void maze_clean (maze&);
void SaveMaze (maze& , const char[]);
void LoadMaze (maze& , const char[]);
char** maze2map (maze);
maze map2maze(char**,int,int);


/*=================================================
	Application main entry point
/*=================================================*/

int main(int argc, char *argv[]){

	srand(time(NULL));
	
	maze m ;
	maze_init(m,12,12,(wall)0);
	
	while(1){
		
		system("cls");
		
		printf(
				"            ~ PMAZE ~          \n"
				" ------------------------------\n"
				" 1) Generate a new maze        \n"
				" 2) Maze editor/viewer         \n"
				" ------------------------------\n"
				" 3) Solve it!                  \n"
				" ------------------------------\n"
				" 4) Load maze from file        \n"
				" 5) Save current maze          \n"
				" ------------------------------\n"
				" 6) About this app             \n"
				" 7) Exit                       \n"
				"\n Your choice : (   )\b\b\b"   );
		
		char read=getch();
		putchar(read);
		system("cls");
		
		switch( read - '0' ){
		
		case 1: //generate
			printf ( "Enter size of maze : " );
			int s;
			scanf("%d",&s);
			printf(
				"Select a generate algorithm:\n"
				"1) DFS\n"
				"\n Your choice : (   )\b\b\b"   
				);
			read=getch();
			
			switch(read - '0'){
			case 1:
				m = gen_maze_dfs ( s , s );
			break;
			}
		break;
		
		case 2: //editor
			maze_editor (m);
		break;
		
		case 3: //solve
			printf(
			"Select a solve algorithm:\n"
			"1) DFS\n"
			"2) Right-Hand\n"
			"3) BFS\n"
			"\n Your choice : (   )\b\b\b"   
			);
			
			read=getch();
			switch(read - '0'){
			case 1:
				solve_maze_dfs ( m,0,0,m.width-1,m.height-1 );
				getch();
			break;
			case 2:
				solve_maze_righthand ( m,0,0,m.width-1,m.height-1 );
				getch();
			break;
			case 3:
				solve_maze_bfs ( m,0,0,m.width-1,m.height-1 );
				getch();
			break;
			}
		break;
		
		case 4: //load
			char str_buff[50];
			printf("Enter file name: ");
			scanf("%s",str_buff);
			LoadMaze(m,str_buff);
			printf("\nLoaded!");
			getch();
		break;
		
		case 5: //save
			printf("Enter file name: ");
			scanf("%s",str_buff);
			SaveMaze(m,str_buff);
			printf("\nSaved!");
			getch();
		break;
		
		case 6: //about
			printf(":)");
			getchar();
		break;
		
		case 7: //exit
			return 0;
		break;
		
		}
	}

}

/*=================================================
	Generate a maze using DFS algorithm 
	( non recursive )
/*=================================================*/

maze gen_maze_dfs(int width,int height)
{
	int order[4]={0,1,2,3};
	int delta[4][2]={{-1,0},{+1,0},{0,-1},{0,+1}};
	wall delta_k[4][2]={{left,right},{right,left},{top,bottom},{bottom,top}};
		
	maze ans;
	maze_init ( ans , width , height , all );
	
	int cx=0,cy=0,nx,ny;
	ans.c[cx][cy].visited=1;
	
	//My stack :)
	int **S = (int**)malloc(width*height*sizeof(int*));
	for(int i=0;i<width*height;i++)
		S[i]=(int*)malloc(2*sizeof(int));
	int StackTop=-1;
	
	while(1){
		random_shuffle(order,0,3);
		char found=0;
		for(int i=0;i<4;i++){
			nx=cx+delta[order[i]][0];
			ny=cy+delta[order[i]][1];
			if(nx>=0 && ny>=0 && nx<width && ny<height)
				if(!ans.c[nx][ny].visited){
					S[++StackTop][0]=cx;
					S[StackTop][1]=cy;
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
			if(StackTop>-1){
				cx=nx=S[StackTop][0];
				cy=nx=S[StackTop--][1];
			}else
				break;
	}
	
	maze_clean(ans);
	
	return ans;
}

/*=================================================
	Print maze on screen
/*=================================================*/

int LastPrintWidth=0,LastPrintHeight=0;

void print_maze(maze &m)
{
	gotoxy(0,4);

	if(LastPrintWidth>m.width*4 || LastPrintHeight>m.height*4)
		for(int i=0;i<LastPrintHeight;i++){
			printf("\r");
			for(int j=0;j<=LastPrintWidth;j++)
				printf(" ");
			printf("\n");
		}

	LastPrintWidth  = m.width*4;
	LastPrintHeight = m.height*4;
	
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
	
	gotoxy(0,0);
}

/*=================================================
	Initialize a maze
/*=================================================*/

void maze_init ( maze &m , int width , int height , wall default_walls)
{
	//TODO : free previous memory !
	//free(m.c);	
	
	m.width=width;
	m.height=height;
	
	m.c=(maze_cell**)malloc(sizeof(maze_cell*)*m.width);
	for(int i=0;i<m.width;i++){
		m.c[i]=(maze_cell*)malloc(sizeof(maze_cell)*m.height);
		for(int j=0;j<m.height;j++){
			m.c[i][j].walls=default_walls;
			m.c[i][j].x=i;
			m.c[i][j].y=j;
		}
	}
	
	maze_clean (m);
}

/*=================================================
	Clean maze
/*=================================================*/
void maze_clean ( maze &m )
{
	for(int i=0;i<m.width;i++)
		for(int j=0;j<m.height;j++){
			m.c[i][j].icon=' ';
			m.c[i][j].visited=0;
		}
}

/*=================================================
	Let User make a maze
/*=================================================*/

void maze_editor(maze &m)
{
	static const int  mov_dir[4][3]={ {77,+1,0} , {75,-1,0} , {72,0,-1} , {80,0,+1} };	
	static const int  key_wall[] = { 'd','a','w','s' }	;
	static const wall wall_key[] = { right,left,top,bottom } ;
	static const int  wall2_key_d[][2] = { {+1,0} , {-1,0} , {0,-1} , {0,+1} } ;
	static const int  wall2_key[]  = { left,right,bottom,top };

	maze_clean(m);
	
	int current_x=0,current_y=0;
	m.c[current_x][current_y].icon='*';
	
	
	while(1){
	
		print_maze(m);
		printf("\t\t\t~Maze editor~\n  arrow keys : move cursor  ~  WASD : toggle walls \n  N : New maze  ~  Q : Save and return ");	
		
		char read=getch();
		char is_func=0;	
		if(read==-32){
			//Function key
			read=getch();
			is_func=1;
		}
		
		if(read=='q')return;
		else if(read=='n'){
			printf("\r%-20s","Enter maze size:");
			scanf("%d",&m.width);
			m.height=m.width;
			maze_init(m,m.width,m.height,(wall)0);
			continue;
		}
		
		if(is_func){
			//Move cursor position
			for(int i=0;i<4;i++)
				if(read==mov_dir[i][0]){
					int X=current_x+mov_dir[i][1] , Y=current_y+mov_dir[i][2];
					if(is_inside_maze(X,Y,m)){
						m.c[current_x][current_y].icon=' ';
						current_x=X;
						current_y=Y;
						m.c[current_x][current_y].icon='*';
					}
					break;
				}
		}else{
			//Toggle walls
			for (int i=0;i<4;i++)
				if(read==key_wall[i]){
					m.c[current_x][current_y].walls = (wall)(m.c[current_x][current_y].walls ^ wall_key[i]);
					int X=current_x+wall2_key_d[i][0] , Y=current_y+wall2_key_d[i][1];
					if(is_inside_maze(X,Y,m))
						m.c[X][Y].walls = (wall)(m.c[X][Y].walls ^ wall2_key[i]);
					break;
				}
		}
			
	}
	maze_clean(m);
}

/*=================================================
	Solve maze using right-hand algorithm
/*=================================================*/

char solve_maze_righthand(maze &m,int sx,int sy,int ex,int ey)
{
	int dir=3; 
	int x=sx,y=sy,nx,ny,lx,ly;
	char goback=0;
	char res=1;
/*	   0
	   ^
	1<   >3
	   v
	   2	
*/
	int turn_order[]={+3,0,+1,+2};			//Right straight Left Back
	int ddir[][2]={{0,-1},{-1,0},{0,+1},{+1,0}};	//Up left down right
	wall dir_wall []={top,left,bottom,right};
	char dir_icon[]={'^','<','v','>'};
	
	
	maze_clean(m);
	
	m.c[ex][ey].icon='E';

	while(x!=ex || y!=ey){
		char found=0;
		for(int i=0;i<4;i++){
			int ndir=(dir+turn_order[i])%4;
			nx=x+ddir[ndir][0];
			ny=y+ddir[ndir][1];
			
			if(is_inside_maze(nx,ny,m))
				if( !(m.c[x][y].walls & dir_wall[ndir])  && !m.c[x][y].visited){
					
					found=1;
					
					if(i==3)
						goback=1;					
					if(x==lx && y==ly)
						goback=0;
					
					if(dir!=ndir){
						dir=ndir;
						if(i!=3 && !goback){
							lx=x;
							ly=y;
						}
					}
					
					m.c[x][y].icon=goback?'.':dir_icon[ndir];			
				//	m.c[x][y].visited=!goback;
					x=nx;y=ny;
					break;
				}
		}
		if(!found){
				res=0;
				break;
		}
	print_maze(m);Sleep(150);
	}
return res;
}

/*=================================================
	Solve maze using DFS algorithm
/*=================================================*/

char _solve_maze_dfs(maze &m,int x,int y,int ex,int ey)
{
	static const int ddir[][2]={{0,-1},{-1,0},{0,+1},{+1,0}};
	static const wall dir_wall []={top,left,bottom,right};
	static const char dir_icon[]={'^','<','v','>'};

	print_maze(m);Sleep(150);
	
	if(!is_inside_maze(x,y,m))
		return 0;
	
	if(x==ex && y==ey)
		return 1;
	
	m.c[x][y].visited=1;
	
	for (int i=0;i<4;i++)
		if( !(m.c[x][y].walls & dir_wall[i]) ){
			int nx=x+ddir[i][0];
			int ny=y+ddir[i][1];
			if(is_inside_maze(nx,ny,m))
				if(!m.c[nx][ny].visited){
					m.c[x][y].icon=dir_icon[i];
					if(_solve_maze_dfs(m,nx,ny,ex,ey))return 1;
					}
		}
	
	m.c[x][y].visited=0;
	m.c[x][y].icon='.';
	return 0;
}

char solve_maze_dfs(maze &m,int x,int y,int ex,int ey)
{
	maze_clean(m);
	m.c[ex][ey].icon='E';
	_solve_maze_dfs(m,x,y,ex,ey);
	maze_clean(m);
}


/*=================================================
	Solve maze using BFS algorithm
/*=================================================*/

char solve_maze_bfs(maze &m,int x,int y,int ex,int ey)
{
		static const int d[][2]={{-1,0},{+1,0},{0,-1},{0,+1}};
		static const wall dw[]={left,right,top,bottom};
		
		queue q;
		maze_cell** path=(maze_cell**)malloc(m.width*m.height*sizeof(maze_cell*));
		int pathi=0;
		
		maze_clean(m);
		QueueAdd(q,&(m.c[x][y]));
		path[pathi++]=&(m.c[x][y]);
		
		while(q.items_count>0){
			maze_cell* t=(maze_cell*)QueuePop(q);
			if(t->x==ex && t->y==ey){
				//We found a path
				for(int i=0;i<pathi;i++)
					path[i]->icon='.';
				return pathi;//Success!
			}
			
			for ( int i=0;i<4;i++ ){// for each t neighbour
				int nx=t->x+d[i][0];
				int ny=t->y+d[i][1];
				if(!is_inside_maze(nx,ny,m) || (t->walls&dw[i]))
					continue;
				
				maze_cell* n=&(m.c[nx][ny]);
				int n_is_inside_path=0;
				for(int j=0;j<pathi;j++)
					if(path[j]==t){
						n_is_inside_path=1;
						break;
					}
				if(!n_is_inside_path){
					path[pathi++]=n;
					QueueAdd(q,n);
				}
			}
		}
	
	return 0;//No path found!
}

/*=================================================
	Load maze from a file
/*=================================================*/

void LoadMaze (maze &m , const char name[] )
{
	FILE* f=fopen(name,"r");
	fscanf(f,"PMAZE1.0;%d,%d;",&m.width,&m.height);
	
	maze_init(m,m.width,m.height,(wall)0);
	
	for(int i=0;i<m.height;i++)
		for(int j=0;j<m.width;j++)
			fscanf(f,"%d,%d;",&m.c[i][j].walls,&m.c[i][j].icon);
	
	fclose(f);
}

/*=================================================
	Save maze to a file
/*=================================================*/

void SaveMaze ( maze& m , const char name [] )
{
	FILE* f=fopen(name,"w+");
	
	fprintf(f,"PMAZE1.0;%d,%d;",m.width,m.height);
	
	for(int i=0;i<m.height;i++)
		for(int j=0;j<m.width;j++)
			fprintf(f,"%d,%d;",m.c[i][j].walls,m.c[i][j].icon);
	
	fclose(f);
}

/*=================================================
	Convert maze to classic map string
/*=================================================*/

char** maze2map (maze m)
{
	char** map=(char**)malloc(2*m.width*sizeof(char*)+1);


	for (int i=0;i<2*m.width+1;i++){
		map[i]=(char*)malloc(2*m.height*sizeof(char)+1);
		for(int j=0;j<2*m.height+1;j++)
			map[i][j]='.';
	}

	for(int i=0;i<m.width;i++)
		for(int j=0;j<m.height;j++){
			int X=2*i+1,Y=2*j+1;
			map[X-1][Y]=map[X-1][Y-1]=map[X-1][Y+1]=m.c[i][j].walls & left ? '#':'.';
			map[X+1][Y]=map[X+1][Y-1]=map[X+1][Y+1]=m.c[i][j].walls & right ? '#':'.';
			map[X][Y-1]=map[X+1][Y-1]=map[X-1][Y-1]=m.c[i][j].walls & top ? '#':'.';
			map[X][Y+1]=map[X+1][Y+1]=map[X-1][Y+1]=m.c[i][j].walls & bottom ? '#':'.';
		}
		for(int i=0;i<2*m.width+1;i++){
			for(int j=0;j<2*m.height+1;j++)
				putchar(map[i][j]);
			putchar('\n');
		}
		return map;
}

/*=================================================
	Convert classic map string to maze
/*=================================================*/

maze map2maze(char** map,int width,int height)
{
	maze maz;
	maz.width=(width-1)/2;
	maz.height=(height-1)/2;

	maz.c=(maze_cell**)malloc(sizeof(maze_cell*)*width);
	for(int i=0;i<width;i++)
		maz.c[i]=(maze_cell*)malloc(sizeof(maze_cell)*height);

	wall w[]={left,right,top,bottom};
	int dmap[][2]={{-1,0},{+1,0},{0,-1},{0,+1}};

	for(int i=0;i<maz.width;i++)
		for(int j=0;j<maz.height;j++){
			int X=2*i+1,Y=2*j+1;
			for(int t=0;t<4;t++)
				if(map[X+dmap[t][0]][Y+dmap[t][1]]=='#')
					maz.c[i][j].walls = (wall)(maz.c[i][j].walls | w[t]);
				else
					maz.c[i][j].walls = (wall)(maz.c[i][j].walls &  ~w[t]);
		}
		return maz;
}



/*=================================================
	Check if current location in inside maze or no
/*=================================================*/

char is_inside_maze ( int x , int y , maze &m)
{
	return ( x>=0 && y>=0 && x<m.width && y<m.height );
}

/*=================================================
	Shuffle array members ( s and e are index )
/*=================================================*/

void random_shuffle(int *arr,int s,int e)
{
	if(s>=e)return;
	int t;
	for(int i=s+1;i<=e;i++)
		if(rand()%2){
			t=arr[s];
			arr[s]=arr[i];
			arr[i]=t;
		}
	random_shuffle(arr,s+1,e);
}

