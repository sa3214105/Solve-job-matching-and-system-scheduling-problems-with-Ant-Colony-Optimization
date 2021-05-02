#include <stdio.h>
#include <stdlib.h>
double getCost(int* ss,int* ms,double* st,double* ft);
/* run this program using the console pauser or add your own getch, system("pause") or input loop */
double getCost(int* ss,int* ms,double* st,double* ft){
	int i,j,k;//迴圈變數 
	double pc_last_f_Time[thePCount];//pc最晚工作完成時間
	double pc_space_area[thePCount][theTCount-1][3];//pc空閒空間(0),開始時間(1),結束時間(2) 
	int pc_space_area_top[128];//pc空閒空間top *記憶體空間重疊 
	int task;//儲存工作 
	int pc;//儲存pc 
	int check_list[theTCount];//紀錄完成的前項
	for(i=0;i<thePCount;i++){
		pc_last_f_Time[i]=0;//pc最晚工作完成時間設為0 
		pc_space_area_top[i]=-1;//pc空閒空間top設為-1
	}
	for(i=0;i<theECount;i++){ 
		check_list[i]=0;//完成表歸0 
	}
	for(i=0;i<theTCount;i++){//走訪task
		task=ss[i];//處理的工作 
		pc=ms[task];//選擇的處理器
		double trans_time=0;//傳輸延時
		double max_trans_time=0;//最大傳輸延時 
		double max_load=pc_last_f_Time[pc];//最大等待時間 *同時也是工作開始時間* (pc的最晚工作完成時間) ;
		int insert_bool=0;//紀錄是否有做過插入 
		for(j=0;j<b_list_top[task];j++){//走訪task的前項
			int b_node=b_list[task][j];//前項 
			trans_time=0.0;//歸零 
			if(check_list[b_node]==1){//確定前項已完成 
				trans_time=ft[b_node]+theCommRate[(int)ms[b_node]][pc]*(dag[b_node][task]);
				//計算此前項完成工作且傳輸完畢的時間(前項的結束時間+傳送速度*傳送成本) *後簡稱傳輸時間 
				if(trans_time>max_trans_time){//若此前項的傳輸時間大於最大等待時間 
					max_trans_time=trans_time;//取代
				}
			}else{//前項未完成  
				return -1;//回傳-1以表示錯誤
			}
		}
		
		if(max_trans_time>max_load){//最大傳輸時間>目前的最大等待時間(pc的最晚工作完成時間),表示會有空閒時間產生 
			max_load=max_trans_time;//最大等待時間更改為最大傳輸時間
			pc_space_area[pc][++pc_space_area_top[pc]][0]=max_load-pc_last_f_Time[pc];//計算並新增空閒時間
			pc_space_area[pc][pc_space_area_top[pc]][1]=pc_last_f_Time[pc];//計算並新增空間開始時間 
			pc_space_area[pc][pc_space_area_top[pc]][2]=max_load;//計算並新增空間結束時間 
			//printf("pc%d area%f st%f et%f\n",pc,pc_space_area[pc][pc_space_area_top[pc]-1][0],pc_space_area[pc][pc_space_area_top[pc]-1][1],pc_space_area[pc][pc_space_area_top[pc]-1][2]);
		}	
		for(j=0;j<=pc_space_area_top[pc];j++){//走訪該pc之空閒空間,尋找插入機會 
			if(pc_space_area[pc][j][0]>cost[task][pc]){//如果空閒空間足夠大 		
				double insert_start_time=pc_space_area[pc][j][2]-cost[task][pc];//insert_start_time為開始此空間此task的開始時間
				if(max_trans_time<=insert_start_time){//若符合插入條件 
					max_load=insert_start_time;//更新最大等待時間 
					insert_bool=1;//紀錄有做過插入 
					pc_space_area[pc][j][0]-=cost[task][pc];//空間歸0
					//printf("%f",pc_space_area[pc][j][0]);
					pc_space_area[pc][j][2]=insert_start_time;
					break;
				}
			}
		}
		//printf("%d:%f:%d ",task,max_load,insert_bool);
		st[task]=max_load;//task的開始時間
		makeSpan=max_load+cost[task][pc];//makespan暫存
		ft[task]=makeSpan;//task的結束時間
		if(insert_bool!=1){//若沒有進行插入動作 
			pc_last_f_Time[pc]=makeSpan;//pc的最晚工作完成時間改為此工作完成之時間
		}
		check_list[task]=1;//把完成的前項填1 
	}
	return makeSpan;//回傳makespan 
}

