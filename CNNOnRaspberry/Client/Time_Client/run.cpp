#include "cnn.h"
#include "Client.h"
#include <time.h>


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
	/*
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
    */ 
	int sampling =3;
	
	Mat frameReference;
	Mat grayImage;
	for(; ;)
	{
		for(int idx=1; idx <= 100; ++idx)
    	{
    		/*
    		captRefrnc >> frameReference;
    		if (frameReference.empty())
        	{
            	break;
        	}
        	*/
        	
        	//generate images
        	gettimeofday(&t1_start,NULL);
        	ss.clear();
			//ss<<"./data/png/"<<setw(5)<<setfill('0')<<idx<<".png";
			ss<<"./data/jpg/00001.jpg";
			ss>>filename;
			frameReference = imread(filename);
			gettimeofday(&t1_end,NULL);
			t1 = t1 + 1000000*(t1_end.tv_sec-t1_start.tv_sec)+t1_end.tv_usec-t1_start.tv_usec; //us
        	
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
			gettimeofday(&t2_start,NULL);
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
			gettimeofday(&t8_end,NULL);
			trafficSign.forward89();
			gettimeofday(&t9_end,NULL);
			trafficSign.forward10();
			gettimeofday(&t10_end,NULL);
			int label = trafficSign.labelresult();
			gettimeofday(&t11_end,NULL);
			char sendline[int_size];
			char* moving = sendline;
    		memcpy(moving, &label, int_size);//convert struct into char array
			client_send(sock_fd, (void*)sendline,  int_size);
			t2 = t2 + 1000000*(t2_end.tv_sec-t2_start.tv_sec)+t2_end.tv_usec-t2_start.tv_usec; //us
			t3 = t3 + 1000000*(t3_end.tv_sec-t2_end.tv_sec)+t3_end.tv_usec-t2_end.tv_usec; //us
			t4 = t4 + 1000000*(t4_end.tv_sec-t3_end.tv_sec)+t4_end.tv_usec-t3_end.tv_usec; //us
			t5 = t5 + 1000000*(t5_end.tv_sec-t4_end.tv_sec)+t5_end.tv_usec-t4_end.tv_usec; //us
			t6 = t6 + 1000000*(t6_end.tv_sec-t5_end.tv_sec)+t6_end.tv_usec-t5_end.tv_usec; //us
			t7 = t7 + 1000000*(t7_end.tv_sec-t6_end.tv_sec)+t7_end.tv_usec-t6_end.tv_usec; //us
			t8 = t8 + 1000000*(t8_end.tv_sec-t7_end.tv_sec)+t8_end.tv_usec-t7_end.tv_usec; //us
			t9 = t9 + 1000000*(t9_end.tv_sec-t8_end.tv_sec)+t9_end.tv_usec-t8_end.tv_usec; //us
			t10 = t10 + 1000000*(t10_end.tv_sec-t9_end.tv_sec)+t10_end.tv_usec-t9_end.tv_usec; //us
			t11 = t11 + 1000000*(t11_end.tv_sec-t10_end.tv_sec)+t11_end.tv_usec-t10_end.tv_usec; //us
		}
		}
	printf("gate %d, 100 images t1 used time:%f us\n",gate, t1); 
	printf("gate %d, 100 images t2 used time:%f us\n",gate, t2);
	printf("gate %d, 100 images t3 used time:%f us\n",gate, t3); 
	printf("gate %d, 100 images t4 used time:%f us\n",gate, t4);
	printf("gate %d, 100 images t5 used time:%f us\n",gate, t5);
	printf("gate %d, 100 images t6 used time:%f us\n",gate, t6); 
	printf("gate %d, 100 images t7 used time:%f us\n",gate, t7);
	printf("gate %d, 100 images t8 used time:%f us\n",gate, t8); 
	printf("gate %d, 100 images t9 used time:%f us\n",gate, t9);
	printf("gate %d, 100 images t8 used time:%f us\n",gate, t10); 
	printf("gate %d, 100 images t9 used time:%f us\n",gate, t11);
	printf("\n");
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
	}
	
	return 0;
}
