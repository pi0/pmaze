#include <stdlib.h>
#include <time.h>
#include "helper_functions.h"


int irand()
{
	return rand();
}

void random_shuffle(int *arr,int s,int e)
{
	if(s>=e)return;
	int t;
	for(int i=s+1;i<=e;i++)
		if(irand()%2){
			t=arr[s];
			arr[s]=arr[i];
			arr[i]=t;
		}
	random_shuffle(arr,s+1,e);
}
