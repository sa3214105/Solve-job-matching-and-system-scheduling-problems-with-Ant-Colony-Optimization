#include <stdio.h>
#include <stdlib.h>

void getLS(){
	int i,j,k;
	double time_count=0;
	LS_list=calloc(theTCount,sizeof(double));
	LS_list[theTCount-1]=0;
	for(k=0;k<thePCount;k++){//cost走訪找最大 
		if(cost[theTCount-1][k]>LS_list[theTCount-1])LS_list[theTCount-1]=cost[theTCount-1][k];
	}
	
	for(i=theTCount-1;i>=0;i--){//現項 
		for(j=0;j<b_list_top[i];j++){//前項 
			double maxP=0;
			for(k=0;k<thePCount;k++){//cost走訪找最大 
				if(cost[b_list[i][j]][k]>maxP)maxP=cost[b_list[i][j]][k];
			}
			double sum=LS_list[i]+dag[b_list[i][j]][i]+maxP;
			if(sum>LS_list[b_list[i][j]])LS_list[b_list[i][j]]=sum;
		}
	}
}

