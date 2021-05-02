#include <stdio.h>
#include <stdlib.h>
#include "control.c"
#include "getcost.c"
#include <time.h>
#include "getLS.c"
#include <math.h>
/* run this program using the console pauser or add your own getch, system("pause") or input loop */
//�ܼ� 
double *st;//�����C�Ӥu�@���}�l�ɶ� 
double *ft;//�����C�Ӥu�@�������ɶ�
int *ms;
int *ss;
int status=0;
int *b_list_top_c;
int *xx_list;
//�禡�ŧi 
double acs(int tmax,double tau0,int knum,double rho_local,double rho_globle);//tmax���榸�� ,tau0�O���X��l��,knum���Ƽƶq 
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
int init_rand=1;//�����O�_���͹L�ü� 
int main(int argc, char *argv[]) {
	initialization();
	int i,j;
	double stx[theTCount];//�����C�Ӥu�@���}�l�ɶ� 
	st=stx;
	double ftx[theTCount];//�����C�Ӥu�@�������ɶ�
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
	double mksp[run_count];//�����C����makespan
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
		
		//�������
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
	int i,j;//�j���ܼ� 
	int t,k;//t���ƭp�ƾ� ,k���ƹM�� 
	double tau_local_ms[theTCount][thePCount];//�إ߶O���X�a��,�ΥH�����C�����|���O���Xms task pc
	double tau_globle_ms[theTCount][thePCount];//�إ߶O���X�a��,�ΥH�����C�����|���O���Xms task pc
	double tau_local_ss[theTCount][theTCount];//�إ߶O���X�a��,�ΥH�����C�����|���O���X x���_�I,y���I(ss)
	double tau_globle_ss[theTCount][theTCount];//�����ϰ�O���X(ss)
	int *minT=ss;//maxT�ܼƬ����̨θ� 
	int *minT_ms=ms;//maxT�ܼƬ����̨θ� 
	double minL=99999;//minL����minT��makespan
	int ant[theTCount];//xx�������Ƹ��|	
	//int ms[theTCount];//�Ƶ{��C 
	int count=0;
	for(i=0;i<theTCount;i++){
		for(j=0;j<theTCount;j++){
			tau_globle_ss[i][j]=tau0;//ss�O���X��l�� 
		}
	}
	//mainloop start
	for(t=0;t<tmax;t++){
		//
		for(k=0;k<knum;k++){//���Ҧ����ƫغc��
			double st[theTCount];//�����C�Ӥu�@���}�l�ɶ� 
			double ft[theTCount];//�����C�Ӥu�@�������ɶ� 
			int ant_ms[theTCount];
			int ms_buf[theTCount];
			for(i=0;i<theTCount;i++){
				st[i]=0;
				ft[i]=0;
				ant_ms[i]=0;
				ms_buf[i]=0;
			}	
			constuct_sol(ant,0.3,tau_local_ss);//���ƫغc��(ss)0.2
			double L=9999;
			int local_minT[theTCount];
			int local_minT_ms[theTCount];
			L=ms_acs(25,5,5,0.05,0.05,ant,local_minT,local_minT_ms);//30,5,5,0.05,0.05,ant,minT
			tau_Cal(ant,tau_local_ss,rho_local,L);//�ϰ�O���X��s(ss)
			if(L<=minL){//�p�G�����u��ثe�̨θ�,�h�i����N
				minL=L;
				for(i=0;i<theTCount;i++){
					minT[i]=ant[i];
					minT_ms[i]=local_minT_ms[i];
				}
			}else{
				count++;
				if(count>100){
					/*for(i=0;i<theTCount;i++){//��Xtau 
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
		//����O���X��s (ss)
		tau_Cal(minT,tau_globle_ss,rho_globle,minL);
		//����O���X��s (ms)
	}
	/*for(k=0;i<theTCount;i++){//��Xtau 
		for(j=0;j<theTCount;j++){
			printf("%f ",tau_globle_ss[i][j]);
		}
		puts("");
	}*/
	return minL;	
}
double ms_acs(int tmax,double tau0,int knum,double rho_local,double rho_globle,int ant_ss[theTCount],int minT[theTCount],int minT_ms[theTCount]){
	int i,j;//�j���ܼ� 
	int t,k;//t���ƭp�ƾ� ,k���ƹM�� 
	double tau_local_ms[theTCount][thePCount];//�إ߶O���X�a��,�ΥH�����C�����|���O���Xms task pc
	double tau_globle_ms[theTCount][thePCount];//�إ߶O���X�a��,�ΥH�����C�����|���O���Xms task pc
	//int *minT_ms=ms;//maxT�ܼƬ����̨θ� 
	double minL=99999;//minL����minT��makespan
	int ant[theTCount];//xx�������Ƹ��|	
	int count=0;//�����̨θѸg�L�h�֦��S�ܰ� 
	
	for(i=0;i<theTCount;i++){
		for(j=0;j<thePCount;j++){
			tau_globle_ms[i][j]=tau0;//ss�O���X��l�� 
		}
	}
	for(i=0;i<theTCount;i++){
		tau_globle_ms[i][ms[i]]+=8.0;	
	}
	//�ھڥ���̨�ms�[�j�O���X 
	int ant_ms[theTCount];

	for(t=0;t<tmax;t++){
		for(i=0;i<theTCount;i++){
			for(j=0;j<thePCount;j++){
				tau_local_ms[i][j]=tau_globle_ms[i][j];//ms�O���X��l�� 
			}
		}
		for(k=0;k<knum;k++){//���Ҧ����ƫغc��
			double st[theTCount];//�����C�Ӥu�@���}�l�ɶ� 
			double ft[theTCount];//�����C�Ӥu�@�������ɶ� 
			int ms_buf[theTCount];
			for(i=0;i<theTCount;i++){
				st[i]=0;
				ft[i]=0;
				ms_buf[i]=0;
			}
			double Lg=constuct_ms(ms_buf,ant_ss,0.1,tau_local_ms,st,ft);//���ƫغc��(ms)
			tau_Cal_ms(ant,ms_buf,tau_local_ms,rho_local,Lg);//�ϰ�O���X��s
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
		//����O���X��s (ss)
		tau_Cal_ms(minT,ant_ms,tau_globle_ms,rho_globle,minL);
	}
	
	//mainloop end
	//getCost(minT,minT_ms,st,ft);
	return minL;
}
int constuct_sol(int* list,double q0,double tau[theTCount][theTCount]){//list �n�إߪ���C,q acs���v�Ѽ� ,tau�ϰ�O���X 
	int i,j;//�j���ܼ� 
	int start_node=0;//�_�l�`�I 
	int Current_node=start_node;//��e�`�I 
	list[0]=0;
	int b_list_top_c_b[theTCount]; //�إ�blisttop 
	b_list_top_c=b_list_top_c_b;
	int xx_list_b[theTCount];//�����w���L���`�I 
	xx_list=xx_list_b;//�����w���L���`�I 
	status=1;
	for(i=0;i<theTCount;i++){
		xx_list[i]=1;//xx_list�w�]��1
		b_list_top_c[i]=b_list_top[i];
	}	
	for(i=0;i<theTCount;i++){//�v�B�غc�� 
		double q=random();//�D�ü� 
		int c_list[theTCount];//�Կ�� 
		int c_list_num=Candidate_sol(list,i,c_list);//�����Կ�ѭӼ� �ëغc�Կ�� 
		//puts("x");
		if(q>q0){//�P�w�O�_�n��as�غc�k 
			//as�غc�k 
			//puts("as");
			list[i]=as_build(Current_node,c_list,c_list_num,tau);
		}else{
			//acs�غc�k
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
	int i,j,k;//�j���ܼ� 
	int s;//pc���X�j���ܼ� 
	double pc_last_f_Time[thePCount];//pc�̱ߤu�@�����ɶ�
	double pc_space_area[thePCount][theTCount-1][3];//pc�Ŷ��Ŷ�(0),�}�l�ɶ�(1),�����ɶ�(2) 
	int pc_space_area_top[128];//pc�Ŷ��Ŷ�top *�O����Ŷ����| 
	int task;//�x�s�u�@ 
	int pc;//�x�spc 
	int check_list[theTCount];//�����������e��
	for(i=0;i<thePCount;i++){
		pc_last_f_Time[i]=0;//pc�̱ߤu�@�����ɶ��]��0 
		pc_space_area_top[i]=-1;//pc�Ŷ��Ŷ�top�]��-1
	}
	for(i=0;i<theECount;i++){ 
		check_list[i]=0;//�������k0 
	}
	for(i=0;i<theTCount;i++){//���Xtask
		task=ss[i];//�B�z���u�@
		double pc_time[thePCount];//�O���b�C��pc���ɶ� 
		int pc_insert_bool[thePCount];
		int xxx[thePCount];
		int insert_space[thePCount];
		double iii_time[thePCount];
		for(s=0;s<thePCount;s++){
			pc=s;//��ܪ��B�z��
			double trans_time=0;//�ǿ驵��
			double max_trans_time=0;//�̤j�ǿ驵�� 
			double max_load=pc_last_f_Time[pc];//�̤j���ݮɶ� *�P�ɤ]�O�u�@�}�l�ɶ�* (pc���̱ߤu�@�����ɶ�) ;
			int insert_bool=0;//�����O�_�����L���J 
			xxx[s]=0;	
			insert_space[s]=0;
			iii_time[s]=0;
			for(j=0;j<b_list_top[task];j++){//���Xtask���e��
				int b_node=b_list[task][j];//�e�� 
				trans_time=0.0;//�k�s 
				if(check_list[b_node]==1){//�T�w�e���w���� 
					trans_time=ft[b_node]+theCommRate[(int)ms[b_node]][pc]*(dag[b_node][task]);
					//printf(" %f ",ft[b_node]);
					//�p�⦹�e�������u�@�B�ǿ駹�����ɶ�(�e���������ɶ�+�ǰe�t��*�ǰe����) *��²�ٶǿ�ɶ� 
					if(trans_time>max_trans_time){//�Y���e�����ǿ�ɶ��j��̤j���ݮɶ� 
						max_trans_time=trans_time;//���N
					}
				}else{//�e��������  
					//return -1;//�^��-1�H��ܿ��~
				}
				//printf(" %f ",max_trans_time);
			}
			if(max_trans_time>max_load){//�̤j�ǿ�ɶ�>�ثe���̤j���ݮɶ�(pc���̱ߤu�@�����ɶ�),��ܷ|���Ŷ��ɶ����� 
				max_load=max_trans_time;//�̤j���ݮɶ���אּ�̤j�ǿ�ɶ�
				pc_space_area[pc][++pc_space_area_top[pc]][0]=max_load-pc_last_f_Time[pc];//�p��÷s�W�Ŷ��ɶ�
				pc_space_area[pc][pc_space_area_top[pc]][1]=pc_last_f_Time[pc];//�p��÷s�W�Ŷ��}�l�ɶ� 
				pc_space_area[pc][pc_space_area_top[pc]][2]=max_load;//�p��÷s�W�Ŷ������ɶ� 
				xxx[s]=1;
			}
			for(j=0;j<=pc_space_area_top[pc];j++){//���X��pc���Ŷ��Ŷ�,�M�䴡�J���| 
				if(pc_space_area[pc][j][0]>cost[task][pc]){//�p�G�Ŷ��Ŷ������j 		
					double insert_start_time=pc_space_area[pc][j][2]-cost[task][pc];//insert_start_time���}�l���Ŷ���task���}�l�ɶ�
					if(max_trans_time<=insert_start_time){//�Y�ŦX���J���� 
						max_load=insert_start_time;//��s�̤j���ݮɶ� 
						insert_bool=1;//���������L���J 
						insert_space[s]=j;//�����t�L���O���ӪŶ� 
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
		double q=random();//�üƨM�wacs||as 
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
		st[task]=max_load;//task���}�l�ɶ�
		makeSpan=max_load+cost[task][min_node];//makespan�Ȧs
		ft[task]=makeSpan;//task�������ɶ�
		if(xxx[min_node]==1){
			pc_space_area_top[min_node]++; //�襤��pc 
		} 
		if(insert_bool!=1){//�Y�S���i�洡�J�ʧ@ 
			pc_last_f_Time[min_node]=makeSpan;//pc���̱ߤu�@�����ɶ��אּ���u�@�������ɶ�
		}
		
		check_list[task]=1;//�⧹�����e����1 	
	}
	//puts("");
	return makeSpan;//�^��makespan 
}
int Candidate_sol(int list[theTCount],int list_len,int c_list[]){//��Կ��C, list �غc������C, list_len�H�غc���`�I ,c_list���G��C��X ,return ���G��C���� 
	int i,j;
	int c_list_num=0;
	int curr=list_len-1;
	if (curr!=-1){
		xx_list[list[curr]]=0; //�{��`�I�]�����L 
		for(j=0;j<a_list_top[list[curr]];j++){
			int a=a_list[list[curr]][j];//��e���s��a 
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
	double s[thePCount];//�`�I�p�q 
	double p[thePCount];//���v�p�� 
	double delta_t[thePCount];//�p��C�Ӷ}�l�ɶ����t���� 
	double q=random();//��l 
	double sum=0;
	double result=0;
	int i,j;
	for(i=0;i<thePCount;i++){
		delta_t[i]=time[i]-l_time+1;//�`�ƥi�H�קK0// delta_t��time[i]-min(time[i])
		//Ranking(delta_t);
		//delta_t[i]=sqrt(delta_t[i]);
		//printf("%d %f\n",i,tau[x][i]);
		//printf("%f\n",delta_t[i]);
		s[i]=pow(tau[x][i],1)/delta_t[i];//�O���X*�i���� 
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
int as_build(int Current_node,int* u,int u_num,double tau[theTCount][theTCount]){//Current_node �{�b�`�I,u �Կ�Ѧ�C,u_num �Կ�Ѽƶq,tau�ϰ�O���X 
	double s[u_num];//�`�I�p�q 
	double p[u_num];//���v�p�� 
	double q=random();//��l 
	double sum=0;
	double result=0;
	int i,j;
	for(i=0;i<u_num;i++){
		s[i]=tau[Current_node][u[i]]*LS_list[u[i]]+200;//�O���X*�i���� 
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
	int beta=1000;//��d�O���X�j�׫Y�� 
	for(i=0;i<theTCount;i++){
		for(j=0;j<theTCount;j++){
			tau[i][j]=(1-rho)*tau[i][j];
		}
	}
	double delta_rho=1/cost*beta;
	delta_rho=pow(delta_rho,2);
	for(i=0;i<theTCount-1;i++){//���Xlist
		tau[list[i]][list[i+1]]+=rho*delta_rho;
	}
	/*for(i=0;i<theTCount;i++){//��Xtau 
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
	int beta=1000;//��d�O���X�j�׫Y��
	for(i=0;i<theTCount;i++){
		for(j=0;j<thePCount;j++){
			tau[i][j]=(1-rho)*tau[i][j];
		}
	}
	double delta_rho=(1/cost)*beta;
	
	delta_rho=pow(delta_rho,2);
	//printf("%f\n",delta_rho); 
	//printf("%f\n",delta_rho);
	for(i=0;i<theTCount-1;i++){//���Xlist
		tau[i][ms[i]]+=	rho*delta_rho;
	}
	/*for(i=0;i<theTCount;i++){//��Xtau 
		for(j=0;j<thePCount;j++){
			printf("%f ",tau[i][j]);
		}
		puts("");
	}
	puts("");*/
	return 0;
}
void display_graph(double makespan){//��ܵ��G�ϧ� 
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
void Ranking(double *list){//�ƦW�k 
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
