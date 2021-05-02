#include <stdio.h>
#include <stdlib.h>
#include "control.c"
#include "getcost.c"
#include <time.h>
#include "getLS.c"
#include <math.h>
/* run this program using the console pauser or add your own getch, system("pause") or input loop */
//變數 
double *st;//紀錄每個工作的開始時間 
double *ft;//紀錄每個工作的結束時間
int *ms;
int *ss;
int status=0;
int *b_list_top_c;
int *xx_list;
//函式宣告 
double acs(int tmax,double tau0,int knum,double rho_local,double rho_globle);//tmax執行次數 ,tau0費洛蒙初始值,knum螞蟻數量 
double random();
int constuct_sol(int* list,double q0,double tau[theTCount][theTCount]);
int as_build(int Current_node,int* u,int u_num,double tau[theTCount][theTCount]);
int Candidate_sol(int list[theTCount],int list_len,int c_list[]);
int tau_Cal(int list[],double local_tau[theTCount][theTCount],double rho_local,double cost);
double constuct_ms(int ms[],int ss[],double q0,double tau[theTCount][thePCount],double *st,double *ft);
int as_build_ms(int x,double time[],double l_time,double tau[theTCount][thePCount]);
int tau_Cal_ms(int ss[],int ms[],double tau[theTCount][thePCount],double rho,double cost);
void display_graph(double makespan);
void Ranking(double *list);
double ms_acs(int tmax,double tau0,int knum,double rho_local,double rho_globle,int ant_ss[theTCount],int minT[theTCount],int minT_ms[theTCount]);

//
int init_rand=1;//紀錄是否產生過亂數 
int main(int argc, char *argv[]) {
	initialization();
	int i,j;
	double stx[theTCount];//紀錄每個工作的開始時間 
	st=stx;
	double ftx[theTCount];//紀錄每個工作的結束時間
	ft=ftx;
	int ssx[theTCount];
	ss=ssx;
	int msx[theTCount];
	ms=msx;
	for(i=0;i<theTCount;i++){
		msx[i]=0;
	}
	double avg=0;
	double minL=9999;
	double maxL=0;
	double time_sum=0;
	int run_count=3;
	double mksp[run_count];//紀錄每次的makespan
	for(i=0;i<run_count;i++){
		clock_t start_time = clock();
		initialization();
		double x=acs(20,5,5,0.05,0.05);//65,5,20,0.01,0.03
		//printf("Time : %f sec \n", total_time);
		if(x<minL)minL=x;
		if(x>maxL)maxL=x;
		
		mksp[i]=x;
		avg+=x;
		//printf("%f\n",x);
		
		//網頁顯示
		for(j=0;j<theTCount;j++){
			printf("%d ",ss[j]);
		}
		puts("");
		for(j=0;j<theTCount;j++){
			printf("%d ",ms[j]);
		}
		puts("");
		printf("makespan:%f\n",x);
		getCost(ss,ms,st,ft);
		display_graph(x);
		freeAll();
		clock_t end_time = clock();
		double total_time = (float)(end_time - start_time)/CLOCKS_PER_SEC;
		time_sum+=total_time;
	}
	avg/=run_count;
	time_sum/=run_count;
	double sd=0;
	for(i=0;i<run_count;i++){
		sd+=(mksp[i]-avg)*(mksp[i]-avg);
	}
	sd/=run_count;
	sd=sqrt(sd);
	printf("avg:%f Best:%f Worst:%f time_sum:%f sec sd:%f",avg,minL,maxL,time_sum,sd);
	return 0;
}
double acs(int tmax,double tau0,int knum,double rho_local,double rho_globle){
	int i,j;//迴圈變數 
	int t,k;//t次數計數器 ,k螞蟻遍歷 
	double tau_local_ms[theTCount][thePCount];//建立費洛蒙地圖,用以紀錄每條路徑之費洛蒙ms task pc
	double tau_globle_ms[theTCount][thePCount];//建立費洛蒙地圖,用以紀錄每條路徑之費洛蒙ms task pc
	double tau_local_ss[theTCount][theTCount];//建立費洛蒙地圖,用以紀錄每條路徑之費洛蒙 x為起點,y終點(ss)
	double tau_globle_ss[theTCount][theTCount];//紀錄區域費洛蒙(ss)
	int *minT=ss;//maxT變數紀錄最佳解 
	int *minT_ms=ms;//maxT變數紀錄最佳解 
	double minL=99999;//minL紀錄minT的makespan
	int ant[theTCount];//xx紀錄螞蟻路徑	
	//int ms[theTCount];//排程串列 
	int count=0;
	for(i=0;i<theTCount;i++){
		for(j=0;j<theTCount;j++){
			tau_globle_ss[i][j]=tau0;//ss費洛蒙初始化 
		}
	}
	//mainloop start
	for(t=0;t<tmax;t++){
		//
		for(k=0;k<knum;k++){//讓所有螞蟻建構解
			double st[theTCount];//紀錄每個工作的開始時間 
			double ft[theTCount];//紀錄每個工作的結束時間 
			int ant_ms[theTCount];
			int ms_buf[theTCount];
			for(i=0;i<theTCount;i++){
				st[i]=0;
				ft[i]=0;
				ant_ms[i]=0;
				ms_buf[i]=0;
			}	
			constuct_sol(ant,0.3,tau_local_ss);//螞蟻建構解(ss)0.2
			double L=9999;
			int local_minT[theTCount];
			int local_minT_ms[theTCount];
			L=ms_acs(25,5,5,0.05,0.05,ant,local_minT,local_minT_ms);//30,5,5,0.05,0.05,ant,minT
			tau_Cal(ant,tau_local_ss,rho_local,L);//區域費洛蒙跟新(ss)
			if(L<=minL){//如果此解優於目前最佳解,則進行取代
				minL=L;
				for(i=0;i<theTCount;i++){
					minT[i]=ant[i];
					minT_ms[i]=local_minT_ms[i];
				}
			}else{
				count++;
				if(count>100){
					/*for(i=0;i<theTCount;i++){//輸出tau 
						for(j=0;j<theTCount;j++){
							printf("%f ",tau_globle_ss[i][j]);
						}
						puts("");
					}*/
					return minL;
					
				}
			}
			//printf("\n%f\n",minL);//getcost
		}
		//全域費洛蒙更新 (ss)
		tau_Cal(minT,tau_globle_ss,rho_globle,minL);
		//全域費洛蒙更新 (ms)
	}
	/*for(k=0;i<theTCount;i++){//輸出tau 
		for(j=0;j<theTCount;j++){
			printf("%f ",tau_globle_ss[i][j]);
		}
		puts("");
	}*/
	return minL;	
}
double ms_acs(int tmax,double tau0,int knum,double rho_local,double rho_globle,int ant_ss[theTCount],int minT[theTCount],int minT_ms[theTCount]){
	int i,j;//迴圈變數 
	int t,k;//t次數計數器 ,k螞蟻遍歷 
	double tau_local_ms[theTCount][thePCount];//建立費洛蒙地圖,用以紀錄每條路徑之費洛蒙ms task pc
	double tau_globle_ms[theTCount][thePCount];//建立費洛蒙地圖,用以紀錄每條路徑之費洛蒙ms task pc
	//int *minT_ms=ms;//maxT變數紀錄最佳解 
	double minL=99999;//minL紀錄minT的makespan
	int ant[theTCount];//xx紀錄螞蟻路徑	
	int count=0;//紀錄最佳解經過多少次沒變動 
	
	for(i=0;i<theTCount;i++){
		for(j=0;j<thePCount;j++){
			tau_globle_ms[i][j]=tau0;//ss費洛蒙初始化 
		}
	}
	for(i=0;i<theTCount;i++){
		tau_globle_ms[i][ms[i]]+=8.0;	
	}
	//根據全域最佳ms加強費洛蒙 
	int ant_ms[theTCount];

	for(t=0;t<tmax;t++){
		for(i=0;i<theTCount;i++){
			for(j=0;j<thePCount;j++){
				tau_local_ms[i][j]=tau_globle_ms[i][j];//ms費洛蒙初始化 
			}
		}
		for(k=0;k<knum;k++){//讓所有螞蟻建構解
			double st[theTCount];//紀錄每個工作的開始時間 
			double ft[theTCount];//紀錄每個工作的結束時間 
			int ms_buf[theTCount];
			for(i=0;i<theTCount;i++){
				st[i]=0;
				ft[i]=0;
				ms_buf[i]=0;
			}
			double Lg=constuct_ms(ms_buf,ant_ss,0.1,tau_local_ms,st,ft);//螞蟻建構解(ms)
			tau_Cal_ms(ant,ms_buf,tau_local_ms,rho_local,Lg);//區域費洛蒙跟新
			if(Lg<minL){
				minL=Lg;
				count=0;
				for(j=0;j<theTCount;j++){
					ant_ms[j]=ms_buf[j];
					minT[j]=ant_ss[j];
					minT_ms[j]=ant_ms[j];
				}
			}else{
				count++;
				if(count>65){
					return minL;
				}
			}
			
			//printf("t:%d k:%d %f\n",t,k,minL);
		
		}
		//全域費洛蒙更新 (ss)
		tau_Cal_ms(minT,ant_ms,tau_globle_ms,rho_globle,minL);
	}
	
	//mainloop end
	//getCost(minT,minT_ms,st,ft);
	return minL;
}
int constuct_sol(int* list,double q0,double tau[theTCount][theTCount]){//list 要建立的串列,q acs機率參數 ,tau區域費洛蒙 
	int i,j;//迴圈變數 
	int start_node=0;//起始節點 
	int Current_node=start_node;//當前節點 
	list[0]=0;
	int b_list_top_c_b[theTCount]; //建立blisttop 
	b_list_top_c=b_list_top_c_b;
	int xx_list_b[theTCount];//紀錄已走過的節點 
	xx_list=xx_list_b;//紀錄已走過的節點 
	status=1;
	for(i=0;i<theTCount;i++){
		xx_list[i]=1;//xx_list預設為1
		b_list_top_c[i]=b_list_top[i];
	}	
	for(i=0;i<theTCount;i++){//逐步建構解 
		double q=random();//求亂數 
		int c_list[theTCount];//候選解 
		int c_list_num=Candidate_sol(list,i,c_list);//紀錄候選解個數 並建構候選解 
		//puts("x");
		if(q>q0){//判定是否要用as建構法 
			//as建構法 
			//puts("as");
			list[i]=as_build(Current_node,c_list,c_list_num,tau);
		}else{
			//acs建構法
			//puts("acs");
			int max_index=theTCount-1; 	
			for(j=0;j<c_list_num;j++){
				if(LS_list[c_list[j]]>LS_list[max_index]){
					max_index=c_list[j];
				}
			} 
			list[i]=max_index;
		}
		Current_node=list[i];
	}
	
	/*for(i=0;i<theTCount;i++){
		for(j=0;j<theTCount;j++){
			printf("%f ",tau[i][j]);
		}
		puts("");
	}*/
	/*for(i=0;i<theTCount;i++){
		printf("%d ",list[i]);
	}*/
	//puts("");
	return 0;
}
double constuct_ms(int *ms,int ss[],double q0,double tau[theTCount][thePCount],double *st,double *ft){
	int i,j,k;//迴圈變數 
	int s;//pc走訪迴圈變數 
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
		double pc_time[thePCount];//記錄在每個pc的時間 
		int pc_insert_bool[thePCount];
		int xxx[thePCount];
		int insert_space[thePCount];
		double iii_time[thePCount];
		for(s=0;s<thePCount;s++){
			pc=s;//選擇的處理器
			double trans_time=0;//傳輸延時
			double max_trans_time=0;//最大傳輸延時 
			double max_load=pc_last_f_Time[pc];//最大等待時間 *同時也是工作開始時間* (pc的最晚工作完成時間) ;
			int insert_bool=0;//紀錄是否有做過插入 
			xxx[s]=0;	
			insert_space[s]=0;
			iii_time[s]=0;
			for(j=0;j<b_list_top[task];j++){//走訪task的前項
				int b_node=b_list[task][j];//前項 
				trans_time=0.0;//歸零 
				if(check_list[b_node]==1){//確定前項已完成 
					trans_time=ft[b_node]+theCommRate[(int)ms[b_node]][pc]*(dag[b_node][task]);
					//printf(" %f ",ft[b_node]);
					//計算此前項完成工作且傳輸完畢的時間(前項的結束時間+傳送速度*傳送成本) *後簡稱傳輸時間 
					if(trans_time>max_trans_time){//若此前項的傳輸時間大於最大等待時間 
						max_trans_time=trans_time;//取代
					}
				}else{//前項未完成  
					//return -1;//回傳-1以表示錯誤
				}
				//printf(" %f ",max_trans_time);
			}
			if(max_trans_time>max_load){//最大傳輸時間>目前的最大等待時間(pc的最晚工作完成時間),表示會有空閒時間產生 
				max_load=max_trans_time;//最大等待時間更改為最大傳輸時間
				pc_space_area[pc][++pc_space_area_top[pc]][0]=max_load-pc_last_f_Time[pc];//計算並新增空閒時間
				pc_space_area[pc][pc_space_area_top[pc]][1]=pc_last_f_Time[pc];//計算並新增空間開始時間 
				pc_space_area[pc][pc_space_area_top[pc]][2]=max_load;//計算並新增空間結束時間 
				xxx[s]=1;
			}
			for(j=0;j<=pc_space_area_top[pc];j++){//走訪該pc之空閒空間,尋找插入機會 
				if(pc_space_area[pc][j][0]>cost[task][pc]){//如果空閒空間足夠大 		
					double insert_start_time=pc_space_area[pc][j][2]-cost[task][pc];//insert_start_time為開始此空間此task的開始時間
					if(max_trans_time<=insert_start_time){//若符合插入條件 
						max_load=insert_start_time;//更新最大等待時間 
						insert_bool=1;//紀錄有做過插入 
						insert_space[s]=j;//紀錄差過的是哪個空間 
						iii_time[s]=insert_start_time;
						break;
					}
				}
			}
			if (xxx[s]==1){
				pc_space_area_top[pc]--;
			}
			pc_time[s]=max_load;
			pc_insert_bool[s]=insert_bool;
		}
		double max_load=9999;
		int min_node;
		int insert_bool;
		double q=random();//亂數決定acs||as 
		for(s=0;s<thePCount;s++){
			if(max_load>pc_time[s]){
				min_node=s;
				max_load=pc_time[s];
			}else if(max_load==pc_time[s]){
				double q=random();
				if(q>0.5){
					min_node=s;
					max_load=pc_time[s];
				}
			}
		}
		if(q>q0){
			//as
			ms[task]=as_build_ms(task,pc_time,max_load,tau);
			
		}else{
			//acs
			ms[task]=min_node;
		}
		min_node=ms[task];
		//printf("%d ",min_node);
		max_load=pc_time[min_node];
		insert_bool=pc_insert_bool[min_node];
		if(insert_bool){
			pc_space_area[min_node][insert_space[min_node]][0]-=cost[task][min_node];
			//printf("%f",pc_space_area[min_node][insert_space[min_node]][0]);
			pc_space_area[min_node][insert_space[min_node]][2]=iii_time[min_node];
		}
		//printf("%d:%f:%d ",task,max_load,insert_bool);
		st[task]=max_load;//task的開始時間
		makeSpan=max_load+cost[task][min_node];//makespan暫存
		ft[task]=makeSpan;//task的結束時間
		if(xxx[min_node]==1){
			pc_space_area_top[min_node]++; //選中的pc 
		} 
		if(insert_bool!=1){//若沒有進行插入動作 
			pc_last_f_Time[min_node]=makeSpan;//pc的最晚工作完成時間改為此工作完成之時間
		}
		
		check_list[task]=1;//把完成的前項填1 	
	}
	//puts("");
	return makeSpan;//回傳makespan 
}
int Candidate_sol(int list[theTCount],int list_len,int c_list[]){//找候選串列, list 建構中的串列, list_len以建構的節點 ,c_list結果串列輸出 ,return 結果串列長度 
	int i,j;
	int c_list_num=0;
	int curr=list_len-1;
	if (curr!=-1){
		xx_list[list[curr]]=0; //現行節點設為走過 
		for(j=0;j<a_list_top[list[curr]];j++){
			int a=a_list[list[curr]][j];//把前項存到a 
			if(b_list_top_c[a]>0)b_list_top_c[a]-=1;
			
		}
	}
	//printf("num=%d ",list[curr]);
		
	for(i=0;i<theTCount;i++){
		if(b_list_top_c[i]==0&&xx_list[i]){
			c_list[c_list_num++]=i;
			//printf("%d ",i);
		}
		//printf("%d ",xx_list[i]);
	}
	
	
//	puts("");
	return c_list_num;
}
int as_build_ms(int x,double time[],double l_time,double tau[theTCount][thePCount]){
	double s[thePCount];//節點計量 
	double p[thePCount];//機率計算 
	double delta_t[thePCount];//計算每個開始時間的差異值 
	double q=random();//骰子 
	double sum=0;
	double result=0;
	int i,j;
	for(i=0;i<thePCount;i++){
		delta_t[i]=time[i]-l_time+1;//常數可以避免0// delta_t為time[i]-min(time[i])
		//Ranking(delta_t);
		//delta_t[i]=sqrt(delta_t[i]);
		//printf("%d %f\n",i,tau[x][i]);
		//printf("%f\n",delta_t[i]);
		s[i]=pow(tau[x][i],1)/delta_t[i];//費洛蒙*可見度 
		sum+=s[i];
	}
	for(i=0;i<thePCount;i++){
		p[i]=s[i]/sum;
		//printf("i:%d dt:%f s[i]:%f p[i]:%f\n",i,delta_t[i],s[i],p[i]);
	}
	for(i=0;i<thePCount;i++){
		q-=p[i];
		if(q<=0){
			return i;
		}
	}
}
int as_build(int Current_node,int* u,int u_num,double tau[theTCount][theTCount]){//Current_node 現在節點,u 候選解串列,u_num 候選解數量,tau區域費洛蒙 
	double s[u_num];//節點計量 
	double p[u_num];//機率計算 
	double q=random();//骰子 
	double sum=0;
	double result=0;
	int i,j;
	for(i=0;i<u_num;i++){
		s[i]=tau[Current_node][u[i]]*LS_list[u[i]]+200;//費洛蒙*可見度 
		//printf("%d:%f\n",i,s[i]);
		sum+=s[i];
	}
	for(i=0;i<u_num;i++){
		p[i]=s[i]/sum;
	}
	for(i=0;i<u_num;i++){
		q-=p[i];
		if(q<0){
			return u[i];
		}
	}
	return u[u_num-1];
}
double random(){
	if(init_rand){
		srand(time(NULL));
		init_rand=0;
	}
	double value=(double)(rand()%10001)/10000;
	return value;
}
int tau_Cal(int list[],double tau[theTCount][theTCount],double rho,double cost){
	int i,j;
	int beta=1000;//遺留費洛蒙強度係數 
	for(i=0;i<theTCount;i++){
		for(j=0;j<theTCount;j++){
			tau[i][j]=(1-rho)*tau[i][j];
		}
	}
	double delta_rho=1/cost*beta;
	delta_rho=pow(delta_rho,2);
	for(i=0;i<theTCount-1;i++){//走訪list
		tau[list[i]][list[i+1]]+=rho*delta_rho;
	}
	/*for(i=0;i<theTCount;i++){//輸出tau 
		for(j=0;j<theTCount;j++){
			printf("%f ",tau[i][j]);
		}
		puts("");
	}
	puts("");*/
	return 0;
}
int tau_Cal_ms(int ss[],int ms[],double tau[theTCount][thePCount],double rho,double cost){
	int i,j,k;
	int beta=1000;//遺留費洛蒙強度係數
	for(i=0;i<theTCount;i++){
		for(j=0;j<thePCount;j++){
			tau[i][j]=(1-rho)*tau[i][j];
		}
	}
	double delta_rho=(1/cost)*beta;
	
	delta_rho=pow(delta_rho,2);
	//printf("%f\n",delta_rho); 
	//printf("%f\n",delta_rho);
	for(i=0;i<theTCount-1;i++){//走訪list
		tau[i][ms[i]]+=	rho*delta_rho;
	}
	/*for(i=0;i<theTCount;i++){//輸出tau 
		for(j=0;j<thePCount;j++){
			printf("%f ",tau[i][j]);
		}
		puts("");
	}
	puts("");*/
	return 0;
}
void display_graph(double makespan){//顯示結果圖形 
	int j;
	char url[1024]="https://makespan.herokuapp.com/";
	char xx[1024];
	char buf[1024]="?";
	sprintf(buf,"%sst=%f",buf,st[0]);
	for(j=1;j<theTCount;j++){
		sprintf(buf,"%s,%f",buf,st[j]);
	}
	sprintf(buf,"%s&ft=%f",buf,ft[0]);
	for(j=1;j<theTCount;j++){
		sprintf(buf,"%s,%f",buf,ft[j]);
	}
	sprintf(buf,"%s&ms=%d",buf,ms[0]);
	for(j=1;j<theTCount;j++){
		sprintf(buf,"%s,%d",buf,ms[j]);
	}
	sprintf(buf,"%s&makespan=%f",buf,makespan);
	sprintf(xx,"C:\\\\progra~2\\\\Google\\\\Chrome\\\\Application\\\\chrome.exe \"%s%s\" -incognito",url,buf);
	//printf("string %s\n",xx);
	system(xx);	
}
void Ranking(double *list){//排名法 
	int index[thePCount];
	int i,j;
	for(i=0;i<thePCount;i++){
		index[i]=i;
	}
	for(i=0;i<thePCount;i++){
		for(j=0;j<thePCount-1;j++){
			if(list[i]<list[i+1]){
				double buf=list[i];
				list[i]=list[i+1];
				list[i+1]=buf;
				buf=index[i];
				index[i]=index[i+1];
				index[i+1]=buf;
			}
		}
	}
	for(i=0;i<thePCount;i++){
		list[index[i]]=i+1;
	}
}
