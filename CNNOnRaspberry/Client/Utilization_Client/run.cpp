#include "cnn.h"
#include "Client.h"
#include "cpu_util.h"
#include <time.h>

struct stats_cpu cur,prev1;
using namespace std;
using namespace cv;

int main(int argc, char* argv[])
{
	omp_init_lock(&lock);
	// load the cnn model
	cnn trafficSign;
	trafficSign.loadModel("./data/gtsrb.txt");

	// forward the cnn
	stringstream ss;
	string filename;
	Mat img;
	
	//connect to server
	int sock_fd = connect_server();
	cout<<"********successfully connected************"<<endl;
	
	
	//initial notification
	int gate = 11;
	int int_size = sizeof(int);
    char sendline1[int_size];
    memset(sendline1,0,int_size); 
    memcpy(sendline1, &gate, int_size);
    client_send(sock_fd, (void*)sendline1, int_size);
    
	int generate_iteration =0;
	int iteration = 0;
	int cpu_id =-1;
	double utilization1= 0.0;
    double utilization2= 0.0;
    double utilization3= 0.0;
    double utilization4= 0.0;
    double utilization5= 0.0;
	double utilization6= 0.0;
    double utilization7= 0.0;
    double utilization8= 0.0;
    double utilization9= 0.0;
	VideoCapture captRefrnc(0);
    if (!captRefrnc.isOpened())
    {
        cout  << "Could not open reference "  << endl;
        return -1;
    }
    Size refS = Size((int) captRefrnc.get(CAP_PROP_FRAME_WIDTH),
                     (int) captRefrnc.get(CAP_PROP_FRAME_HEIGHT));
    int Fps = (int) captRefrnc.get(CAP_PROP_FPS);
    cout << "Reference frame resolution: Width=" << refS.width << "  Height=" << refS.height
         << " of Fps :# " << captRefrnc.get(CAP_PROP_FPS) << endl;
    
	int sampling =3;
	
	Mat frameReference;
	Mat grayImage;
	for(; ;)
	{
		for(int idx=1; idx <= 100; ++idx)
    	{
        	read_stat_cpu(&prev1, cpu_id);
       		captRefrnc >> frameReference;
        	if (frameReference.empty())
        	{
            	break;
        	}
        	read_stat_cpu(&cur, cpu_id);
			utilization1 = (utilization1 * generate_iteration + calc_util_perc(prev1, cur))/(generate_iteration+1);
			generate_iteration++;
        	//generate images
        	
        	//ss.clear();
			//ss<<"./data/png/"<<setw(5)<<setfill('0')<<idx<<".png";
			//ss<<"./data/jpg/00001.jpg";
			//ss>>filename;
			//frameReference = imread(filename);
        	
        	//generated image:640*480*3 channels
        	
        	if(gate ==1)
        	{
        		//Send Frames
        		client_send(sock_fd, (void *)frameReference.data, 640*480*3);
        	}
        	//bgr2gray image:640*480
        	else if(gate ==2)
        	{
        		//bgr2gray
        		cvtColor(frameReference, grayImage, CV_BGR2GRAY);//640*480
        		//Send Frames
        		client_send(sock_fd, (void *)grayImage.data, 640*480);
        		
        	}
        //Resized image:32*32
		if(gate == 3)
		{
			//bgr2gray
        	cvtColor(frameReference, grayImage, CV_BGR2GRAY);//640*480
        	//resize image
        	resize(grayImage, img, Size(32,32), 0, 0, CV_INTER_LINEAR);
        	trafficSign.getImg(img);//push the img to cnn
        	
			//double img[32][32];
			int send_size= sizeof(double)*32*32;
			char sendline[send_size];
			char* moving = sendline;
    		memcpy(moving, &trafficSign.img, send_size);//convert struct into char array
			client_send(sock_fd, (void*)sendline, send_size);
		}
		else if(gate ==4)
		{
			//bgr2gray
        	cvtColor(frameReference, grayImage, CV_BGR2GRAY);//640*480
        	//resize image
        	resize(grayImage, img, Size(32,32), 0, 0, CV_INTER_LINEAR);
        	//resize(grayImage, img, Size(32,32), 0, 0, CV_INTER_AREA);
        	trafficSign.getImg(img);//push the img to cnn
        	
			//double conv1tanh2[16][28][28];
			trafficSign.forward12();
			int send_size= sizeof(double)*16*28*28;
			char sendline[send_size];
			char* moving = sendline;
    		memcpy(moving, &trafficSign.conv1tanh2, send_size);//convert struct into char array
			client_send(sock_fd, (void*)sendline, send_size);
		}
		else if(gate ==5)
		{
			//bgr2gray
        	cvtColor(frameReference, grayImage, CV_BGR2GRAY);//640*480
        	//resize image
        	resize(grayImage, img, Size(32,32), 0, 0, CV_INTER_LINEAR);
        	//resize(grayImage, img, Size(32,32), 0, 0, CV_INTER_AREA);
        	trafficSign.getImg(img);//push the img to cnn
        	
			//double pooling3[16][14][14];
			trafficSign.forward12();
			trafficSign.forward3();
			int send_size= sizeof(double)*16*14*14;
			char sendline[send_size];
			char* moving = sendline;
    		memcpy(moving, &trafficSign.pooling3, send_size);//convert struct into char array
			client_send(sock_fd, (void*)sendline, send_size);
		}
		else if(gate ==6)
		{
			//bgr2gray
        	cvtColor(frameReference, grayImage, CV_BGR2GRAY);//640*480
        	//resize image
        	resize(grayImage, img, Size(32,32), 0, 0, CV_INTER_LINEAR);
        	//resize(grayImage, img, Size(32,32), 0, 0, CV_INTER_AREA);
        	trafficSign.getImg(img);//push the img to cnn
        	
			//double conv4tanh5[32][10][10];
			trafficSign.forward12();
			trafficSign.forward3();
			trafficSign.forward45();
			int send_size= sizeof(double)*32*10*10;
			char sendline[send_size];
			char* moving = sendline;
    		memcpy(moving, &trafficSign.conv4tanh5, send_size);//convert struct into char array
			client_send(sock_fd, (void*)sendline,  send_size);
		}
		else if(gate ==7)
		{
			//bgr2gray
        	cvtColor(frameReference, grayImage, CV_BGR2GRAY);//640*480
        	//resize image
        	resize(grayImage, img, Size(32,32), 0, 0, CV_INTER_LINEAR);
        	//resize(grayImage, img, Size(32,32), 0, 0, CV_INTER_AREA);
        	trafficSign.getImg(img);//push the img to cnn
        	
			//double pooling6[32][5][5];
			trafficSign.forward12();
			trafficSign.forward3();
			trafficSign.forward45();
			trafficSign.forward6();
			int send_size= sizeof(double)*32*5*5;
			char sendline[send_size];
			char* moving = sendline;
    		memcpy(moving, &trafficSign.pooling6, send_size);//convert struct into char array
			client_send(sock_fd, (void*)sendline,  send_size);
		}
		else if(gate ==8)
		{
			//bgr2gray
        	cvtColor(frameReference, grayImage, CV_BGR2GRAY);//640*480
        	//resize image
        	resize(grayImage, img, Size(32,32), 0, 0, CV_INTER_LINEAR);
        	//resize(grayImage, img, Size(32,32), 0, 0, CV_INTER_AREA);
        	trafficSign.getImg(img);//push the img to cnn
        	
			//double reshape7[800];
			trafficSign.forward12();
			trafficSign.forward3();
			trafficSign.forward45();
			trafficSign.forward6();
			trafficSign.forward7();
			int send_size= sizeof(double)*800;
			char sendline[send_size];
			char* moving = sendline;
    		memcpy(moving, &trafficSign.reshape7, send_size);//convert struct into char array
			client_send(sock_fd, (void*)sendline,  send_size);
		}
		else if(gate ==9)
		{
			//bgr2gray
        	cvtColor(frameReference, grayImage, CV_BGR2GRAY);//640*480
        	//resize image
        	resize(grayImage, img, Size(32,32), 0, 0, CV_INTER_LINEAR);
        	//resize(grayImage, img, Size(32,32), 0, 0, CV_INTER_AREA);
        	trafficSign.getImg(img);//push the img to cnn
        	
			//double linear8tanh9[256];
			trafficSign.forward12();
			trafficSign.forward3();
			trafficSign.forward45();
			trafficSign.forward6();
			trafficSign.forward7();
			trafficSign.forward89();
			int send_size= sizeof(double)*256;
			char sendline[send_size];
			char* moving = sendline;
    		memcpy(moving, &trafficSign.linear8tanh9, send_size);//convert struct into char array
			client_send(sock_fd, (void*)sendline,  send_size);
		}
		else if(gate ==10)
		{
			//bgr2gray
        	cvtColor(frameReference, grayImage, CV_BGR2GRAY);//640*480
        	//resize image
        	resize(grayImage, img, Size(32,32), 0, 0, CV_INTER_LINEAR);
        	//resize(grayImage, img, Size(32,32), 0, 0, CV_INTER_AREA);
        	trafficSign.getImg(img);//push the img to cnn
        	
			//double linear10[43];
			trafficSign.forward12();
			trafficSign.forward3();
			trafficSign.forward45();
			trafficSign.forward6();
			trafficSign.forward7();
			trafficSign.forward89();
			trafficSign.forward10();
			int send_size= sizeof(double)*43;
			char sendline[send_size];
			char* moving = sendline;
    		memcpy(moving, &trafficSign.linear10, send_size);//convert struct into char array
			client_send(sock_fd, (void*)sendline,  send_size);
		}
		else if(gate == 11)
		{
			//gettimeofday(&t2_start,NULL);
			//bgr2gray
			read_stat_cpu(&prev1, cpu_id);
        	cvtColor(frameReference, grayImage, CV_BGR2GRAY);//640*480
        	read_stat_cpu(&cur, cpu_id);
			utilization2 = (utilization2 * iteration + calc_util_perc(prev1, cur))/(iteration+1);
			copy_struct(cur, &prev1);
        	//gettimeofday(&t2_end,NULL);
        	//resize image
        	resize(grayImage, img, Size(32,32), 0, 0, CV_INTER_LINEAR);
        	//resize(grayImage, img, Size(32,32), 0, 0, CV_INTER_AREA);
        	trafficSign.getImg(img);//push the img to cnn
        	read_stat_cpu(&cur, cpu_id);
        	utilization3 = (utilization3 * iteration + calc_util_perc(prev1, cur))/(iteration+1);
        	copy_struct(cur, &prev1);
        	//gettimeofday(&t3_end,NULL);
        	
			trafficSign.forward12();
			read_stat_cpu(&cur, cpu_id);
        	utilization4 = (utilization4 * iteration + calc_util_perc(prev1, cur))/(iteration+1);
        	copy_struct(cur, &prev1);
			//gettimeofday(&t4_end,NULL);
			
			trafficSign.forward3();
			read_stat_cpu(&cur, cpu_id);
        	utilization5 = (utilization5 * iteration + calc_util_perc(prev1, cur))/(iteration+1);
        	copy_struct(cur, &prev1);
			//gettimeofday(&t5_end,NULL);
			
			trafficSign.forward45();
			read_stat_cpu(&cur, cpu_id);
        	utilization6 = (utilization6 * iteration + calc_util_perc(prev1, cur))/(iteration+1);
        	copy_struct(cur, &prev1);
			//gettimeofday(&t6_end,NULL);
			
			trafficSign.forward6();
			read_stat_cpu(&cur, cpu_id);
        	utilization7 = (utilization7 * iteration + calc_util_perc(prev1, cur))/(iteration+1);
        	copy_struct(cur, &prev1);
			//gettimeofday(&t7_end,NULL);
			
			trafficSign.forward7();//reshape
			//gettimeofday(&t8_end,NULL);
			trafficSign.forward89();//dense layer #1
			//gettimeofday(&t9_end,NULL);
			read_stat_cpu(&cur, cpu_id);
        	utilization8 = (utilization8 * iteration + calc_util_perc(prev1, cur))/(iteration+1);
        	copy_struct(cur, &prev1);
        	
			trafficSign.forward10();
			//gettimeofday(&t10_end,NULL);
			int label = trafficSign.labelresult();
			//gettimeofday(&t11_end,NULL);
			read_stat_cpu(&cur, cpu_id);
        	utilization9 = (utilization9 * iteration + calc_util_perc(prev1, cur))/(iteration+1);
        	
			char sendline[int_size];
			char* moving = sendline;
    		memcpy(moving, &label, int_size);//convert struct into char array
			client_send(sock_fd, (void*)sendline,  int_size);
			
			iteration++;
		}
		}
	printf("gate %d, 100 images t1 utilization:%f \n",gate, utilization1); 
	printf("gate %d, 100 images t2 utilization:%f \n",gate, utilization2);
	printf("gate %d, 100 images t3 utilization:%f \n",gate, utilization3); 
	printf("gate %d, 100 images t4 utilization:%f \n",gate, utilization4);
	printf("gate %d, 100 images t5 utilization:%f \n",gate, utilization5);
	printf("gate %d, 100 images t6 utilization:%f \n",gate, utilization6); 
	printf("gate %d, 100 images t7 utilization:%f \n",gate, utilization7);
	printf("gate %d, 100 images t8 utilization:%f \n",gate, utilization8); 
	printf("gate %d, 100 images t9 utilization:%f \n",gate, utilization9);
	printf("\n");
	utilization1=0;
	utilization2=0;
	utilization3=0;
	utilization4=0;
	utilization5=0;
	utilization6=0;
	utilization7=0;
	utilization8=0;
	utilization9=0;
	iteration=0;
	generate_iteration=0;
	}
	
	return 0;
}
