#include <stdio.h>
#include <stdlib.h>
double getCost(int* ss,int* ms,double* st,double* ft);
/* run this program using the console pauser or add your own getch, system("pause") or input loop */
double getCost(int* ss,int* ms,double* st,double* ft){
	int i,j,k;//�j���ܼ� 
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
		pc=ms[task];//��ܪ��B�z��
		double trans_time=0;//�ǿ驵��
		double max_trans_time=0;//�̤j�ǿ驵�� 
		double max_load=pc_last_f_Time[pc];//�̤j���ݮɶ� *�P�ɤ]�O�u�@�}�l�ɶ�* (pc���̱ߤu�@�����ɶ�) ;
		int insert_bool=0;//�����O�_�����L���J 
		for(j=0;j<b_list_top[task];j++){//���Xtask���e��
			int b_node=b_list[task][j];//�e�� 
			trans_time=0.0;//�k�s 
			if(check_list[b_node]==1){//�T�w�e���w���� 
				trans_time=ft[b_node]+theCommRate[(int)ms[b_node]][pc]*(dag[b_node][task]);
				//�p�⦹�e�������u�@�B�ǿ駹�����ɶ�(�e���������ɶ�+�ǰe�t��*�ǰe����) *��²�ٶǿ�ɶ� 
				if(trans_time>max_trans_time){//�Y���e�����ǿ�ɶ��j��̤j���ݮɶ� 
					max_trans_time=trans_time;//���N
				}
			}else{//�e��������  
				return -1;//�^��-1�H��ܿ��~
			}
		}
		
		if(max_trans_time>max_load){//�̤j�ǿ�ɶ�>�ثe���̤j���ݮɶ�(pc���̱ߤu�@�����ɶ�),��ܷ|���Ŷ��ɶ����� 
			max_load=max_trans_time;//�̤j���ݮɶ���אּ�̤j�ǿ�ɶ�
			pc_space_area[pc][++pc_space_area_top[pc]][0]=max_load-pc_last_f_Time[pc];//�p��÷s�W�Ŷ��ɶ�
			pc_space_area[pc][pc_space_area_top[pc]][1]=pc_last_f_Time[pc];//�p��÷s�W�Ŷ��}�l�ɶ� 
			pc_space_area[pc][pc_space_area_top[pc]][2]=max_load;//�p��÷s�W�Ŷ������ɶ� 
			//printf("pc%d area%f st%f et%f\n",pc,pc_space_area[pc][pc_space_area_top[pc]-1][0],pc_space_area[pc][pc_space_area_top[pc]-1][1],pc_space_area[pc][pc_space_area_top[pc]-1][2]);
		}	
		for(j=0;j<=pc_space_area_top[pc];j++){//���X��pc���Ŷ��Ŷ�,�M�䴡�J���| 
			if(pc_space_area[pc][j][0]>cost[task][pc]){//�p�G�Ŷ��Ŷ������j 		
				double insert_start_time=pc_space_area[pc][j][2]-cost[task][pc];//insert_start_time���}�l���Ŷ���task���}�l�ɶ�
				if(max_trans_time<=insert_start_time){//�Y�ŦX���J���� 
					max_load=insert_start_time;//��s�̤j���ݮɶ� 
					insert_bool=1;//���������L���J 
					pc_space_area[pc][j][0]-=cost[task][pc];//�Ŷ��k0
					//printf("%f",pc_space_area[pc][j][0]);
					pc_space_area[pc][j][2]=insert_start_time;
					break;
				}
			}
		}
		//printf("%d:%f:%d ",task,max_load,insert_bool);
		st[task]=max_load;//task���}�l�ɶ�
		makeSpan=max_load+cost[task][pc];//makespan�Ȧs
		ft[task]=makeSpan;//task�������ɶ�
		if(insert_bool!=1){//�Y�S���i�洡�J�ʧ@ 
			pc_last_f_Time[pc]=makeSpan;//pc���̱ߤu�@�����ɶ��אּ���u�@�������ɶ�
		}
		check_list[task]=1;//�⧹�����e����1 
	}
	return makeSpan;//�^��makespan 
}

