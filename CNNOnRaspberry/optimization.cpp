#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <fstream>
using namespace std;

int main(){
	//CNN added sampling 3
	int stage =9;
	double weight1 =0.5;
	double weight2=0.5;
	int P_adjust=1294;
	int bandwdith = 4;
	int Power_local1[]={2099,2846, 1570,3238,1596,3920,1495,2068,1495};//mW


	int Power_wifi=1860;
	//int time_local1[] = {6653596,0, 660601,45262,733636,66563,2759707,17082,259089,15036};//us
	int time_local1[] = {363147,662857,45781,731369,64555,2746728,16794,254771,14981};//us
	//int time_local2[] = {5524966, 22790703, 17898814, 31644, 5440};//us
	//int time_local3[] = {41907245, 23996596, 17943359, 33505, 4868};//us

	//int time_pc1[] ={1210572,0, 81048,6805,270069,52615,656520,13099,52328,7608};
	int time_pc1[] ={476869,73404,6385,242302,43050,612809,11544,45161,6762};
	//int time_pc2[] ={5616787, 1663169, 2543309, 1855, 279};
	//int time_pc3[] ={11498651, 1727452, 2760690, 2034, 281};
	
	int tranmiss_size[]={640*480*3*100, 640*480*100, 100*32*32*sizeof(double), 100*sizeof(double)*16*28*28, 100*sizeof(double)*16*14*14, 100*sizeof(double)*32*10*10, 100*sizeof(double)*32*5*5, 100*sizeof(double)*256, 100*sizeof(int)};
	
	int sum_localtime =0;
	int sum_pctime=0;
	for(int i=0;i<stage; i++)
	{
		sum_pctime = sum_pctime +time_pc1[i];
		//sum_pctime = sum_pctime +time_pc2[i];
		//sum_pctime = sum_pctime +time_pc3[i];
	}
	sum_pctime = sum_pctime/1000;//ms
	double current_value = 0.0;
	for(int i=0;i<stage;i++)
	{
		sum_localtime = sum_localtime + time_local1[i]/1000;//ms
		//sum_localtime = sum_localtime + time_local2[i]/1000;//ms
		//sum_localtime = sum_localtime + time_local3[i]/1000;//ms
		
		sum_pctime = sum_pctime - time_pc1[i]/1000;//ms
		//sum_pctime = sum_pctime - time_pc2[i]/1000;//ms
		//sum_pctime = sum_pctime - time_pc3[i]/1000;//ms
		int trans= tranmiss_size[i]/(bandwdith*1000);//ms
		printf("transmission time for i= %d: %d\n", i, trans);
		if((sum_localtime + trans)< sum_pctime)
		{
			current_value =  current_value + (double)Power_local1[i]*time_local1[i]*weight1/1000;
			//current_value =  current_value + (double)Power_new[i]*time_local1[i]*weight1/1000;
			//current_value =  current_value + (double)Power_new[i]*time_local2[i]*weight1/1000;
			//current_value =  current_value + (double)Power_new[i]*time_local3[i]*weight1/1000;
			double min = current_value + Power_wifi*trans*weight1 + sum_pctime*P_adjust*weight2;
			printf("i: %d, current1: %.0f, cost function: %.0f \n", i, current_value, min);
		}
		else
		{
			current_value =  current_value + (double)Power_local1[i]*time_local1[i]*weight1/1000;
			//current_value =  current_value + (double)Power_new[i]*time_local1[i]*weight1/1000;
			//current_value =  current_value + (double)Power_new[i]*time_local2[i]*weight1/1000;
			//current_value =  current_value + (double)Power_new[i]*time_local3[i]*weight1/1000;
			double min = current_value + Power_wifi*trans*weight1 + (sum_localtime + trans)*P_adjust*weight2;
			printf("i: %d, current2: %.0f, cost function: %.0f \n", i,current_value,  min);
		}
	}
	
	
	
	
}