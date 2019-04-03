#include "cnn.h"
#include "Server_socket.h"
#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>
using namespace std;
using namespace cv;

int main(int argc,char *argv[]){
	omp_init_lock(&lock);
	int listenfd = start_connection();
	printf("finished listening\n");

	// load the cnn model
	cnn trafficSign;
	trafficSign.loadModel("./data/gtsrb.txt");

 	//initial notification
 	int initial_gate = ser_initialrecv();
 	printf("initial gate: %d \n", initial_gate);
 	
 	struct timeval t1_start, t1_end, t2_start, t2_end, t3_start,t3_end,t4_end,t5_end,t6_end,t7_end,t8_end,t9_end,t10_end,t11_end;
	double t1, t2,t3, t4, t5,t6, t7,t8,t9,t10,t11;
	t1=0;
	t2=0;
	t3=0;
	t4=0;
	t5=0;
	t6=0;
	t7=0;
	t8=0;
	t9=0;
	t10=0;
	t11=0;
	Mat frameReference = Mat::zeros(480,640, CV_8UC3);//640*480*3
	Mat grayImage = Mat::zeros(480,640, CV_8UC1);//640*480
	Mat img;//32*32
	for(;;)
	{
	for(int i=0;i<100;i++)
	{
	if(initial_gate == 1)
	{
		char allframes[QUEUE_LINE*640*480*3];
 		memset(allframes,0, QUEUE_LINE*640*480*3);
 		server_recv(allframes, 640*480*3);
 		for(int j=0; j< QUEUE_LINE; j++)
 		{	
 			gettimeofday(&t2_start,NULL);
			memcpy(frameReference.data,&allframes[j*640*480*3],sizeof(char)*640*480*3);
 			//bgr2gray
        	cvtColor(frameReference, grayImage, CV_BGR2GRAY);//640*480
        	gettimeofday(&t2_end,NULL);
        	//resize image
        	resize(grayImage, img, Size(32,32), 0, 0, CV_INTER_LINEAR);
        	//resize(grayImage, img, Size(32,32), 0, 0, CV_INTER_AREA);
        	trafficSign.getImg(img);//push the img to cnn
        	gettimeofday(&t3_end,NULL);
        	
 			trafficSign.forward12();
			gettimeofday(&t4_end,NULL);
			trafficSign.forward3();
			gettimeofday(&t5_end,NULL);
			trafficSign.forward45();
			gettimeofday(&t6_end,NULL);
			trafficSign.forward6();
			gettimeofday(&t7_end,NULL);
			
			trafficSign.forward7();
			trafficSign.forward89();
			gettimeofday(&t9_end,NULL);
			
			trafficSign.forward10();
			int label = trafficSign.labelresult();
			gettimeofday(&t11_end,NULL);
			
			t2 = t2 + 1000000*(t2_end.tv_sec-t2_start.tv_sec)+t2_end.tv_usec-t2_start.tv_usec; //us
			t3 = t3 + 1000000*(t3_end.tv_sec-t2_end.tv_sec)+t3_end.tv_usec-t2_end.tv_usec; //us
			t4 = t4 + 1000000*(t4_end.tv_sec-t3_end.tv_sec)+t4_end.tv_usec-t3_end.tv_usec; //us
			t5 = t5 + 1000000*(t5_end.tv_sec-t4_end.tv_sec)+t5_end.tv_usec-t4_end.tv_usec; //us
			t6 = t6 + 1000000*(t6_end.tv_sec-t5_end.tv_sec)+t6_end.tv_usec-t5_end.tv_usec; //us
			t7 = t7 + 1000000*(t7_end.tv_sec-t6_end.tv_sec)+t7_end.tv_usec-t6_end.tv_usec; //us
			t9 = t9 + 1000000*(t9_end.tv_sec-t7_end.tv_sec)+t9_end.tv_usec-t7_end.tv_usec; //us
			t11 = t11 + 1000000*(t11_end.tv_sec-t9_end.tv_sec)+t11_end.tv_usec-t9_end.tv_usec; //us
 			//cout<<label<<endl;
 		}
	}
	if(initial_gate == 2)
	{
		char allframes[QUEUE_LINE*640*480];
 		memset(allframes,0, QUEUE_LINE*640*480);
 		server_recv(allframes, 640*480);
 		for(int j=0; j< QUEUE_LINE; j++)
 		{	
			memcpy(grayImage.data,&allframes[j*640*480],sizeof(char)*640*480);
        	//resize image
        	resize(grayImage, img, Size(32,32), 0, 0, CV_INTER_LINEAR);
        	//resize(grayImage, img, Size(32,32), 0, 0, CV_INTER_AREA);
        	trafficSign.getImg(img);//push the img to cnn
        	
 			trafficSign.forward12();
			trafficSign.forward3();
			trafficSign.forward45();
			trafficSign.forward6();
			trafficSign.forward7();
			trafficSign.forward89();
			trafficSign.forward10();
			int label = trafficSign.labelresult();
 			cout<<label<<endl;
 		}
	}
	//double img[32][32]
 	if(initial_gate == 3)
 	{
 		//double img[32][32];
 		int imgSize = sizeof(double)*32*32;
 		char allframes[QUEUE_LINE*imgSize];
 		memset(allframes,0, QUEUE_LINE*imgSize);
 		server_recv(allframes, imgSize);
 		for(int j=0; j< QUEUE_LINE; j++)
 		{	
 			memcpy(trafficSign.img, &allframes[j*imgSize], imgSize);
			trafficSign.forward12();
			trafficSign.forward3();
			trafficSign.forward45();
			trafficSign.forward6();
			trafficSign.forward7();
			trafficSign.forward89();
			trafficSign.forward10();
			int label = trafficSign.labelresult();
 			cout<<label<<endl;
 		}
 		
 	}
 	else if (initial_gate == 4)
 	{
 		//double conv1tanh2[16][28][28];
 		int receiveSize= sizeof(double)*16*28*28;
 		char allframes[QUEUE_LINE*receiveSize];
 		memset(allframes,0, QUEUE_LINE*receiveSize);
 		server_recv(allframes, receiveSize);
 		for(int j=0; j< QUEUE_LINE; j++)
 		{	
 			memcpy(trafficSign.conv1tanh2, &allframes[j*receiveSize], receiveSize);
 			trafficSign.forward3();
			trafficSign.forward45();
			trafficSign.forward6();
			trafficSign.forward7();
			trafficSign.forward89();
			trafficSign.forward10();
			int label = trafficSign.labelresult();
			cout<<label<<endl;
 			
 		}
 	}
 	else if (initial_gate == 5)
 	{
 		//double pooling3[16][14][14];
 		int receiveSize= sizeof(double)*16*14*14;
 		char allframes[QUEUE_LINE*receiveSize];
 		memset(allframes,0, QUEUE_LINE*receiveSize);
 		server_recv(allframes, receiveSize);
 		for(int j=0; j< QUEUE_LINE; j++)
 		{	
 			memcpy(trafficSign.pooling3, &allframes[j*receiveSize], receiveSize);
			trafficSign.forward45();
			trafficSign.forward6();
			trafficSign.forward7();
			trafficSign.forward89();
			trafficSign.forward10();
			int label = trafficSign.labelresult();
			cout<<label<<endl;
 			
 		}
 	}
 	else if (initial_gate == 6)
 	{
 		//double conv4tanh5[32][10][10];
 		int receiveSize= sizeof(double)*32*10*10;
 		char allframes[QUEUE_LINE*receiveSize];
 		memset(allframes,0, QUEUE_LINE*receiveSize);
 		server_recv(allframes, receiveSize);
 		for(int j=0; j< QUEUE_LINE; j++)
 		{	
 			memcpy(trafficSign.conv4tanh5, &allframes[j*receiveSize], receiveSize);
			trafficSign.forward6();
			trafficSign.forward7();
			trafficSign.forward89();
			trafficSign.forward10();
			int label = trafficSign.labelresult();
			cout<<label<<endl;
 			
 		}
 	}
 	else if (initial_gate == 7)
 	{
 		//double pooling6[32][5][5];	
 		int receiveSize= sizeof(double)*32*5*5;
 		char allframes[QUEUE_LINE*receiveSize];
 		memset(allframes,0, QUEUE_LINE*receiveSize);
 		server_recv(allframes, receiveSize);
 		for(int j=0; j< QUEUE_LINE; j++)
 		{	
 			memcpy(trafficSign.pooling6, &allframes[j*receiveSize], receiveSize);
			trafficSign.forward7();
			trafficSign.forward89();
			trafficSign.forward10();
			int label = trafficSign.labelresult();
			cout<<label<<endl;
 			
 		}
 	}
 	else if (initial_gate == 8)
 	{
 		//double reshape7[800];	
 		int receiveSize= sizeof(double)*800;
 		char allframes[QUEUE_LINE*receiveSize];
 		memset(allframes,0, QUEUE_LINE*receiveSize);
 		server_recv(allframes, receiveSize);
 		for(int j=0; j< QUEUE_LINE; j++)
 		{	
 			memcpy(trafficSign.reshape7, &allframes[j*receiveSize], receiveSize);
			trafficSign.forward89();
			trafficSign.forward10();
			int label = trafficSign.labelresult();
			cout<<label<<endl;
 			
 		}
 	}
 	else if (initial_gate == 9)
 	{
 		//double linear8tanh9[256];
 		int receiveSize= sizeof(double)*256;
 		char allframes[QUEUE_LINE*receiveSize];
 		memset(allframes,0, QUEUE_LINE*receiveSize);
 		server_recv(allframes, receiveSize);
 		for(int j=0; j< QUEUE_LINE; j++)
 		{	
 			memcpy(trafficSign.linear8tanh9, &allframes[j*receiveSize], receiveSize);
			trafficSign.forward10();
			int label = trafficSign.labelresult();
			cout<<label<<endl;
 			
 		}
 	}
 	else if (initial_gate == 10)
 	{
 		//double linear10[43];
 		int receiveSize= sizeof(double)*43;
 		char allframes[QUEUE_LINE*receiveSize];
 		memset(allframes,0, QUEUE_LINE*receiveSize);
 		server_recv(allframes, receiveSize);
 		for(int j=0; j< QUEUE_LINE; j++)
 		{	
 			memcpy(trafficSign.linear8tanh9, &allframes[j*receiveSize], receiveSize);
			trafficSign.forward10();
			int label = trafficSign.labelresult();
			cout<<label<<endl;
 			
 		}
 	}
 	else if (initial_gate == 11)
 	{
 		//label	
 		int receiveSize= sizeof(int);
 		char allframes[QUEUE_LINE*receiveSize];
 		memset(allframes,0, QUEUE_LINE*receiveSize);
 		server_recv(allframes, receiveSize);
 		for(int j=0; j< QUEUE_LINE; j++)
 		{	
 			memcpy(&trafficSign.classLabel, &allframes[j*receiveSize], receiveSize);
 			//cout<<trafficSign.classLabel<<endl;
 		}
 	}
 	else
 	{
 		sleep(1);
 	}
 	}
 	if(initial_gate == 1)
 	{
 	printf("gate %d, 100 images t2 used time:%f us\n",initial_gate, t2);
	printf("gate %d, 100 images t3 used time:%f us\n",initial_gate, t3); 
	printf("gate %d, 100 images t4 used time:%f us\n",initial_gate, t4);
	printf("gate %d, 100 images t5 used time:%f us\n",initial_gate, t5);
	printf("gate %d, 100 images t6 used time:%f us\n",initial_gate, t6); 
	printf("gate %d, 100 images t7 used time:%f us\n",initial_gate, t7);
	//printf("gate %d, 100 images t8 used time:%f us\n",initial_gate, t8); 
	printf("gate %d, 100 images t9 used time:%f us\n",initial_gate, t9);
	//printf("gate %d, 100 images t10 used time:%f us\n",initial_gate, t10); 
	printf("gate %d, 100 images t11 used time:%f us\n",initial_gate, t11);
	printf("\n");
	t2=0;
	t3=0;
	t4=0;
	t5=0;
	t6=0;
	t7=0;
	t8=0;
	t9=0;
	t10=0;
	t11=0;
	}
 	}
 	omp_destroy_lock(&lock);
 	close(listenfd);
 }
 