#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <fstream>
using namespace std;

int main(){
	int stage =5;
	double weight1 =0.27;
	double weight2=0.73;
	int P_adjust=1294;
	int bandwdith = 4;
	
	int Power_new[]={1990, 2033, 2031, 2783, 3604};//mW
	//int Power_local[]={1747, 1842, 1908, 2151, 1903}; //mW
	int Power_local1[]={1908, 1943, 2022, 2078, 1946};//mW
	int Power_local2[]={1824, 2018, 2000, 2222, 2036};//mW
	int Power_local3[]={1673, 1985, 1982, 2099, 1981};//mW


	int Power_wifi=1860;
	int time_local1[] = {1963031, 23174779, 18315914, 30967, 5446};//us
	int time_local2[] = {5524966, 22790703, 17898814, 31644, 5440};//us
	int time_local3[] = {41907245, 23996596, 17943359, 33505, 4868};//us


	//int time_pc1[] ={1963031, 1668883, 2487866, 1863, 250};
	int time_pc1[] ={1996068, 1668883, 2487866, 1863, 250};
	int time_pc2[] ={5616787, 1663169, 2543309, 1855, 279};
	int time_pc3[] ={11498651, 1727452, 2760690, 2034, 281};
	
	//int tranmiss_size[]={640*480*3*180*3, 640*480*3*180, 180*289*4, 180*289*4, 25*289*4};
	//int tranmiss_size[]={640*480*3*180*6, 640*480*3*180, 180*289*4, 180*289*4, 25*289*4};
	int tranmiss_size[]={640*480*3*180*12, 640*480*3*180, 180*289*4, 180*289*4, 25*289*4};
	
	int sum_localtime =0;
	int sum_pctime=0;
	for(int i=0;i<stage; i++)
	{
		//sum_pctime = sum_pctime +time_pc1[i];
		//sum_pctime = sum_pctime +time_pc2[i];
		sum_pctime = sum_pctime +time_pc3[i];
	}
	sum_pctime = sum_pctime/1000;//ms
	double current_value = 0.0;
	for(int i=0;i<stage;i++)
	{
		//sum_localtime = sum_localtime + time_local1[i]/1000;//ms
		//sum_localtime = sum_localtime + time_local2[i]/1000;//ms
		sum_localtime = sum_localtime + time_local3[i]/1000;//ms
		
		//sum_pctime = sum_pctime - time_pc1[i]/1000;//ms
		//sum_pctime = sum_pctime - time_pc2[i]/1000;//ms
		sum_pctime = sum_pctime - time_pc3[i]/1000;//ms
		int trans= tranmiss_size[i]/(bandwdith*1000);//ms
		printf("transmission time for i= %d: %d\n", i, trans);
		if((sum_localtime + trans)< sum_pctime)
		{
			//current_value =  current_value + (double)Power_new[i]*time_local1[i]*weight1/1000;
			//current_value =  current_value + (double)Power_new[i]*time_local1[i]*weight1/1000;
			//current_value =  current_value + (double)Power_new[i]*time_local2[i]*weight1/1000;
			current_value =  current_value + (double)Power_new[i]*time_local3[i]*weight1/1000;
			double min = current_value + Power_wifi*trans*weight1 + sum_pctime*P_adjust*weight2;
			printf("i: %d, current1: %.0f, cost function: %.0f \n", i, current_value, min);
		}
		else
		{
			//current_value =  current_value + (double)Power_new[i]*time_local1[i]*weight1/1000;
			//current_value =  current_value + (double)Power_new[i]*time_local1[i]*weight1/1000;
			//current_value =  current_value + (double)Power_new[i]*time_local2[i]*weight1/1000;
			current_value =  current_value + (double)Power_new[i]*time_local3[i]*weight1/1000;
			double min = current_value + Power_wifi*trans*weight1 + (sum_localtime + trans)*P_adjust*weight2;
			printf("i: %d, current2: %.0f, cost function: %.0f \n", i,current_value,  min);
		}
	}
	
	
	
}