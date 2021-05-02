#include <stdio.h>
#include <stdlib.h>
int thePCount=0;
int theTCount=0;
int theECount=0;

double* LS_list;

double** theCommRate;
double **dag;
double **cost;
int **b_list;//紀錄所有節點的前節點 
int **a_list;//紀錄所有節點的後節點 
int *b_list_top;
int *a_list_top;
double makeSpan;
int freeAll();
int loadFile(char* fileName);
int initialization();
/* run this program using the console pauser or add your own getch, system("pause") or input loop */
int initialization(){
	char fileName[]="n4_04.dag";
	loadFile(fileName);
	getLS();
	return 0;
}
int freeAll(){
	int i;
	for(i=0;i<theTCount;i++){
		free(cost[i]);
		free(dag[i]);
		free(b_list[i]);
		free(a_list[i]);
	}
	for(i=0;i<thePCount;i++){
		free(theCommRate[i]);
	}
	free(theCommRate);
	free(cost);
	free(dag);
	free(b_list);
	free(a_list);
	free(b_list_top);
	free(a_list_top);
	free(LS_list);
	return 0;
}
int loadFile(char* fileName){
	FILE* f_in;
	char readBuf[256];
	char* buf_p=readBuf;
	if((f_in=fopen(fileName,"r"))==NULL){
		printf("檔案無法開啟");
		return -1;
	}
	int comment_bool=0;
	int section=0;
	int localCounter=0;
	while((fgets(readBuf,256,f_in))!=NULL){
		if(comment_bool==1){
			if(readBuf[1]=='/'){
				comment_bool=0;
				section++;
				if(section==2){
					int i,j;
					dag=calloc(theTCount,sizeof(double*));
					for(i=0;i<theTCount;i++){
						dag[i]=calloc(theTCount*theTCount,sizeof(double));
					}
					
					b_list=calloc(theTCount,sizeof(int*));
					b_list_top=calloc(theTCount,sizeof(int*));
					for(i=0,j=0;i<theTCount;i++,j+=theTCount){
						b_list[i]=calloc(theTCount*theTCount,sizeof(int));;
						b_list_top[i]=0;
					}
					
					a_list=calloc(theTCount,sizeof(int*));
					a_list_top=calloc(theTCount,sizeof(int*));
					for(i=0,j=0;i<theTCount;i++,j+=theTCount){
						a_list[i]=calloc(theTCount*theTCount,sizeof(int));;
						a_list_top[i]=0;
					}
					
					theCommRate=calloc(thePCount,sizeof(double*));
					for(i=0;i<thePCount;i++){
						theCommRate[i]=calloc(thePCount*thePCount,sizeof(double));
					}
					
					cost=calloc(theTCount,sizeof(double*));
					for(i=0;i<theTCount;i++){
						cost[i]=calloc(theTCount*thePCount,sizeof(double));
					}

				}	
				localCounter=0;
				continue;
			}
		}else{
			if(readBuf[0]=='/'){
				comment_bool=1;
			}
		}
		//String 切割用變數
  		char *delim = " ";
  		char * pch;
  		int buf=0;
		if(comment_bool==0){
			switch(section){
				case 1:
					switch(localCounter){
						case 0:
							thePCount=atoi(readBuf);
							break;
						case 1:
							theTCount=atoi(readBuf);
							break;
						case 2:
							theECount=atoi(readBuf);
							break;
					}
					break;
				case 2:
					pch = strtok(readBuf,delim);
					buf=0;
					while (pch != NULL && *pch !='\n'){
						theCommRate[localCounter][buf++]=atof(pch);
						pch = strtok (NULL, delim);
					}
					break;
				case 3:
					pch = strtok(readBuf,delim);
					buf=0;
					while (pch != NULL && *pch !='\n'){	
						cost[localCounter][buf++]=atof(pch);
						pch = strtok (NULL, delim);
					}
					break;
				case 4:
					
					pch = strtok(readBuf,delim);
					buf=0;
					double list_buf[3];
					while (pch != NULL && *pch !='\n'){
						list_buf[buf++]=atof(pch);
						pch = strtok (NULL, delim);
					}
					dag[(int)list_buf[0]][(int)list_buf[1]]=list_buf[2];
					b_list[(int)list_buf[1]][b_list_top[(int)list_buf[1]]++]=(int)list_buf[0];//紀錄前項 
					a_list[(int)list_buf[0]][a_list_top[(int)list_buf[0]]++]=(int)list_buf[1];//紀錄後項 
					break;
			}
			localCounter++;
		}	
	}
	return 0;
}
