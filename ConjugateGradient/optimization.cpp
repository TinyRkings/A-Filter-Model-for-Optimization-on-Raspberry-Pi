#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <fstream>
using namespace std;

int main(){
	int stage =3;
	double weight1 =0;
	double weight2=1;
	int P_adjust=1294;
	int bandwdith = 4;
	
	int Power_new[]={2025, 2025, 2025};//mW


	int Power_wifi=1860;
	int time_local1[] = {644572, 2065229, 34011};//us
	int time_local2[] = {786599, 6605913, 122892};//us
	int time_local3[] = {685411, 3535815, 63598};//us
	int time_local4[] = {735952, 5086920, 93298};//us


	int time_pc1[] ={60285, 154528, 2509};
	int time_pc2[] ={100703, 483395, 8983};
	int time_pc3[] ={71647, 259264, 4576};
	int time_pc4[] ={84356, 366240, 6627};
	
	int tranmiss_size1[]={(500*500*0.02*16+500*8+8)*100, (500*500*0.02*16+500*8+500*8+8)*100, 8*100};//0.02
	int tranmiss_size2[]={(500*500*0.08*16+500*8+8)*100, (500*500*0.08*16+500*8+500*8+8)*100, 8*100};//0.08
	int tranmiss_size3[]={(500*500*0.04*16+500*8+8)*100, (500*500*0.04*16+500*8+500*8+8)*100, 8*100};//0.04
	int tranmiss_size4[]={(500*500*0.06*16+500*8+8)*100, (500*500*0.06*16+500*8+500*8+8)*100, 8*100};//0.06
	
	int sum_localtime =0;
	int sum_pctime=0;
	for(int i=0;i<stage; i++)
	{
		//sum_pctime = sum_pctime +time_pc1[i];
		//sum_pctime = sum_pctime +time_pc2[i];
		//sum_pctime = sum_pctime +time_pc3[i];
		sum_pctime = sum_pctime +time_pc4[i];
	}
	sum_pctime = sum_pctime/1000;//ms
	double current_value = 0.0;
	for(int i=0;i<stage;i++)
	{
		//sum_localtime = sum_localtime + time_local1[i]/1000;//ms
		//sum_localtime = sum_localtime + time_local2[i]/1000;//ms
		//sum_localtime = sum_localtime + time_local3[i]/1000;//ms
		sum_localtime = sum_localtime + time_local4[i]/1000;//ms
		
		//sum_pctime = sum_pctime - time_pc1[i]/1000;//ms
		//sum_pctime = sum_pctime - time_pc2[i]/1000;//ms
		//sum_pctime = sum_pctime - time_pc3[i]/1000;//ms
		sum_pctime = sum_pctime - time_pc4[i]/1000;//ms
		
		//int trans= tranmiss_size1[i]/(bandwdith*1000);//ms
		//int trans= tranmiss_size2[i]/(bandwdith*1000);//ms
		//int trans= tranmiss_size3[i]/(bandwdith*1000);//ms
		int trans= tranmiss_size4[i]/(bandwdith*1000);//ms
		printf("transmission time for i= %d: %d\n", i, trans);
		
		if((sum_localtime + trans)< sum_pctime)
		{
			//current_value =  current_value + (double)Power_new[i]*time_local1[i]*weight1/1000;
			//current_value =  current_value + (double)Power_new[i]*time_local2[i]*weight1/1000;
			//current_value =  current_value + (double)Power_new[i]*time_local3[i]*weight1/1000;
			current_value =  current_value + (double)Power_new[i]*time_local4[i]*weight1/1000;
			double min = current_value + Power_wifi*trans*weight1 + sum_pctime*P_adjust*weight2;
			printf("i: %d, current1: %.0f, cost function: %.0f \n", i, current_value, min);
		}
		else
		{
			//current_value =  current_value + (double)Power_new[i]*time_local1[i]*weight1/1000;
			//current_value =  current_value + (double)Power_new[i]*time_local2[i]*weight1/1000;
			//current_value =  current_value + (double)Power_new[i]*time_local3[i]*weight1/1000;
			current_value =  current_value + (double)Power_new[i]*time_local4[i]*weight1/1000;
			double min = current_value + Power_wifi*trans*weight1 + (sum_localtime + trans)*P_adjust*weight2;
			printf("i: %d, current2: %.0f, cost function: %.0f \n", i,current_value,  min);
		}
	}
	
	
	
}