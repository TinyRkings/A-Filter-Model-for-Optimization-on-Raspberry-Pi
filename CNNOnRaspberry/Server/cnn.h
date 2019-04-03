#ifndef CNN_H
#define CNN_H
#include <string>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <fstream>
#include <omp.h>
#include <iostream> // for standard I/O
#include <iomanip>  // for controlling float print precision
#include <sstream>  // string to number conversion
#include <fcntl.h>
#include <unistd.h>

#include <opencv2/core.hpp>     // Basic OpenCV structures (cv::Mat, Scalar)
#include <opencv2/imgproc.hpp>  // Gaussian Blur
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>  // OpenCV window I/O
#include <opencv2/imgcodecs.hpp>
#define num_omp_threads 4
static omp_lock_t lock;
using namespace std;
using namespace cv;

class cnn
{
	public:
		double conv1Weight[16][1][5][5];
		double conv1Bias[16];
		double conv4Weight[32][16][5][5];
		double conv4Bias[32];
		double linear8Weight[256][800];
		double linear8Bias[256];
		double linear10Weight[43][256];
		double linear10Bias[43];

		double img[32][32];
		double conv1tanh2[16][28][28];
		double pooling3[16][14][14];
		double conv4tanh5[32][10][10];
		double pooling6[32][5][5];
		double reshape7[800];
		double linear8tanh9[256];
		double linear10[43];
		int classLabel;

		void forward12();
		void forward3();
		void forward45();
		void forward6();
		void forward7();
		void forward89();
		void forward10();
		int labelresult();

		cnn();
		~cnn();

		bool loadModel(string filename);
        bool getImg(Mat matImg);
		int forward();
		void setimg();
};

int Current_Power(int file);

#endif
