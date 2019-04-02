#include <time.h>
#include "ParallelKMeans.h"
#include "cpu_util.h"
#include "Client.h"


struct stats_cpu cur,prev1;
using namespace std;
using namespace cv;

void test_histogram(int sock_fd, struct mycentroid * points, int number_points){
	//time variables
    clock_t startTime=0, endTime=0;
    double parallelTime=0;
    
    int int_size= sizeof(int);
	char sendline[int_size+ int_size* number_centers];
	//histogram
	int his[number_centers];
	
	startTime = clock();
	compute_histogram(number_points,points,his);
	int iteration_end = -1;
	memset(sendline,0,sizeof(sendline));    
    memcpy(sendline, &iteration_end, int_size);
   	memcpy(sendline + int_size, his,int_size* number_centers);//convert struct into char array
	client_send(sock_fd, sendline, int_size+ int_size* number_centers);
	
	endTime=clock();
	parallelTime = ((double) (endTime - startTime)) * 1000 / CLOCKS_PER_SEC; //ms
	cout << "Final time: " << parallelTime << endl;
	
	cout << "Final histogram: " << endl;
	for(int i = 0; i< number_centers; i++)
		cout << i<<":	"<<his[i]<<endl;
}

double getPSNR(const Mat& I1, const Mat& I2)
{
    Mat s1;
    absdiff(I1, I2, s1);       // |I1 - I2|
    s1.convertTo(s1, CV_32F);  // cannot make a square on 8 bits
    s1 = s1.mul(s1);           // |I1 - I2|^2

    Scalar s = sum(s1);        // sum elements per channel

    double sse = s.val[0] + s.val[1] + s.val[2]; // sum channels

    if( sse <= 1e-10) // for small values return zero
        return 0;
    else
    {
        double mse  = sse / (double)(I1.channels() * I1.total());
        double psnr = 10.0 * log10((255 * 255) / mse);
        return psnr;
    }
}


int one_image(Mat& image, struct mycentroid *points, int order){
	if(! image.data )	// Check for invalid input
    {
        cout <<  "Could not open or find the image" << std::endl ;
        return -1;
    }
	int channels = image.channels();
    int nRows = image.rows;
    int nCols = image.cols * channels;
    if (image.isContinuous())
    {
        nCols *= nRows;
        nRows = 1;
    }
    //initialize the all buckets to 0 
    for (int q = 0; q < Dimensions; q++)
		points[order].values[q] = 0;
    int i,j;
    uchar* p;
    //#pragma omp parallel for
    for( i = 0; i < nRows; i++)
    {
       	p = image.ptr<uchar>(i);
        for ( j = 0; j < nCols; )
        {
			HSV hsv_value = convertBGRtoHSV(p[j], p[j+1], p[j+2]);
			int bucket = Cal_bucket(hsv_value);
			j = j+3;
			//omp_set_lock(&lock); 
			points[order].values[bucket]++;
			//omp_unset_lock(&lock); 
            //p[j] = table[p[j]];
        }
    }
	return 0; 
}


int image2HSV(Mat& image, struct HSV* OneImage){
	if(! image.data )	// Check for invalid input
    {
        cout <<  "Could not open or find the image" << std::endl ;
        return -1;
    }
	int channels = image.channels();
    int nRows = image.rows;
    int nCols = image.cols * channels;
    int Cols = image.cols;
    if (image.isContinuous())
    {
        nCols *= nRows;
        nRows = 1;
    }
    int i,j;
    uchar* p;
    #pragma omp parallel for
    for( i = 0; i < nRows; ++i)
    {
       	p = image.ptr<uchar>(i);
        for ( j = 0; j < nCols; )
        {
			HSV hsv_value = convertBGRtoHSV(p[j], p[j+1], p[j+2]);
			OneImage[i*Cols+j/3].H = hsv_value.H;
			OneImage[i*Cols+j/3].S = hsv_value.S;
			OneImage[i*Cols+j/3].V = hsv_value.V;
			j = j+3;
        }
    }
	return 0; 
}

int main(){
	omp_init_lock(&lock);
    int numChanged;
    srand(time(NULL));
    
    //parameters of data
	int number_points = 180;
	int int_size = sizeof(int);
	int centroids_size = sizeof(struct mycentroid)* number_centers;
	int points_size = sizeof(struct mycentroid)* number_points;
	
	//create the points space and K centers space
	struct mycentroid *centroids;
 	centroids = (struct mycentroid *)malloc(centroids_size);
 	
 	struct mycentroid *points;
 	points = (struct mycentroid *)malloc(points_size);
 	
	//connect to server
	int sock_fd = connect_server();
	cout<<"********successfully connected************"<<endl;
	
	//video capture
    int psnrTriggerValue = 60;
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
    int gate = 5;
    int sampling =3;
    
    //initial notification
    char sendline1[int_size];
    memset(sendline1,0,int_size); 
    memcpy(sendline1, &gate, int_size);
    client_send(sock_fd, (void*)sendline1, int_size);
    
    if (gate == 5 || gate == 4)
    {
    	//receive initial centroids
    	char sendline[centroids_size];
		char* moving = sendline;
		char* recv;
		memset(sendline,0,centroids_size); 
		recv = client_recv(sock_fd, sendline, centroids_size);
		memcpy(centroids, recv, centroids_size);
		printf("received initial centroids \n");
    }
	
    Mat frameReference;
    Mat LastEffective = Mat::zeros(480,640, CV_8UC3);
	double psnrV;
	
	int collected = 0;
	int generate_iteration =0;
	int filter_iteration = 0;
	int features_iteration =0;
	int assign_iteration = 0;
	int accumulate_iteration = 0;
	int wifi_iteration = 0;
	char sendline[centroids_size+int_size];
	char* moving = sendline;
	memset(sendline,0,sizeof(sendline));
	char sendline2[points_size];
    char* moving2 = sendline2;
    memset(sendline2,0,sizeof(sendline2));
    char* recv;	
    int cpu_id =-1;
    double utilization1= 0.0;
    double utilization2= 0.0;
    double utilization3= 0.0;
    double utilization4= 0.0;
    double utilization5= 0.0;
    for(;;) //Show the image captured in the window and repeat
    {
    	for(int frameNum=0; frameNum < number_points * sampling; frameNum++)
    	{
    		
    		///////////////////// generate frames ////////////////////
		if(frameNum%3==0)
    			read_stat_cpu(&prev1, cpu_id);
       		captRefrnc >> frameReference;
       		int imgSize = frameReference.total()*frameReference.elemSize();
        	if (frameReference.empty())
        	{
            		break;
        	}
		if(frameNum%3==0)
		{
        		read_stat_cpu(&cur, cpu_id);
			utilization1 = (utilization1 * generate_iteration + calc_util_perc(prev1, cur))/(generate_iteration+1);
			generate_iteration++;
		}
        	///////////////////// generate frames ////////////////////
        	if(gate == 1) //add a placement
        	{
        		//Send Frames
        		client_send(sock_fd, (void *)frameReference.data, imgSize);
        	}
        	else if (gate == 2) //frames filter: sampling and distinct
        	{	
        		if(frameNum%sampling == 0)
        		{	
        			psnrV = getPSNR(frameReference,LastEffective);// lower, more different
        			//cout << setiosflags(ios::fixed) << setprecision(3) << psnrV << "dB";
        			if (psnrV < psnrTriggerValue && psnrV)
        			{	
        				LastEffective = frameReference.clone();
        				client_send(sock_fd, (void *)frameReference.data, imgSize);
        			}
        			/*
        			memcpy(moving, frameReference.data, imgSize);//convert struct into char array
    				moving = moving + imgSize;
    				collected++ ;
    				if(collected == number_points)
    					break;
    				*/
        		}        		
        	}
        	else if (gate == 3) //HSV histogram
        	{
        		if(frameNum%sampling == 0)
        		{	
        			psnrV = getPSNR(frameReference,LastEffective);// lower, more different
        			//cout << setiosflags(ios::fixed) << setprecision(3) << psnrV << "dB";
        			if (psnrV < psnrTriggerValue && psnrV)
        			{	
        				LastEffective = frameReference.clone();
        				one_image(frameReference, points, collected);
        				collected++ ;
        				if(collected == number_points)
    					{
    						memcpy(moving2, points, points_size);//convert struct into char array
    						client_send(sock_fd, (void*)sendline2, points_size);
    						break;
    					}
        			}
        			
    			}
        	}
        	else if (gate == 4) // assigned points
        	{
        		if(frameNum%sampling == 0)
        		{	
        			psnrV = getPSNR(frameReference,LastEffective);// lower, more different
        			if (psnrV < psnrTriggerValue && psnrV)
        			{	
        				LastEffective = frameReference.clone();
        				one_image(frameReference, points, collected);
        				collected++ ;
        				if(collected == number_points)
    					{
    						numChanged = assignPointsToNearestClusterParallel(number_points, points, centroids);
    						memcpy(moving2, points, points_size);//convert struct into char array
    						client_send(sock_fd, (void*)sendline2, points_size);
    						
    						//receive new centroids from the master
							memset(sendline,0,sizeof(sendline));
							recv = client_recv(sock_fd, sendline, centroids_size);
							memcpy(centroids, recv, centroids_size);
    						break;
    					}
        			}
        			
    			}
        	}
        	else  if (gate == 5)//frames filter, vector features and accumulated distances
        	{
        		if(frameNum%sampling == 0)
        		{
        			///////////////////// frames filters ////////////////////
        			read_stat_cpu(&prev1, cpu_id);
        			psnrV = getPSNR(frameReference,LastEffective);// lower, more different
        			//cout << setiosflags(ios::fixed) << setprecision(2) << psnrV << "dB ";
        			read_stat_cpu(&cur, cpu_id);
				utilization2 = (utilization2 * filter_iteration + calc_util_perc(prev1, cur))/(filter_iteration+1);
				filter_iteration++;
        			///////////////////// frames filters ////////////////////
        			
        			if (psnrV < psnrTriggerValue && psnrV)
        			{	
        				LastEffective = frameReference.clone();
        				///////////////////// frames filters ////////////////////
        				
        				///////////////////// histogram features ////////////////////
        				read_stat_cpu(&prev1, cpu_id);
        				one_image(frameReference, points, collected);
        				collected++ ;
        				read_stat_cpu(&cur, cpu_id);
					utilization3 = (utilization3 * features_iteration + calc_util_perc(prev1, cur))/(features_iteration+1);
					features_iteration++;
        				///////////////////// histogram features ////////////////////

        				if(collected == number_points)
						{
							///////////////////// assigned points ////////////////////
							read_stat_cpu(&prev1, cpu_id);
        					numChanged = assignPointsToNearestClusterParallel(number_points, points, centroids);
        					read_stat_cpu(&cur, cpu_id);
							utilization4 = (utilization4 * assign_iteration + calc_util_perc(prev1, cur))/(assign_iteration+1);
							copy_struct(cur, &prev1);
							assign_iteration++;
        					///////////////////// assigned points ////////////////////
        					
        					///////////////////// local clusters ////////////////////
        					//read_stat_cpu(&prev1, cpu_id);
							accumulatedDistanceParallel(number_points, points, centroids);
							read_stat_cpu(&cur, cpu_id);
							utilization5 = (utilization5 * accumulate_iteration + calc_util_perc(prev1, cur))/(accumulate_iteration+1);
							accumulate_iteration++;
							///////////////////// local clusters ////////////////////
							
							printf("gate %d, %d points generating frames utilization:%f us\n",gate, number_points, utilization1); 
							printf("gate %d, %d points filter utilization:%f us\n",gate, number_points, utilization2);
							printf("gate %d, %d points image processing utilization:%f us\n",gate, number_points, utilization3); 
							printf("gate %d, %d points assigned points utilization:%f us\n",gate, number_points, utilization4);
							printf("gate %d, %d points local clusters utilization:%f us\n",gate, number_points, utilization5);
							
							//send new centroids and number of points to the master
    						memcpy(moving, &number_points, sizeof(int));
    						moving = moving + sizeof(int);
    						memcpy(moving, centroids, centroids_size);//convert struct into char array
							client_send(sock_fd, (void*)sendline, centroids_size+ sizeof(int));
			
							//receive new centroids from the master
							memset(sendline,0,sizeof(sendline));
							recv = client_recv(sock_fd, sendline, centroids_size);
							memcpy(centroids, recv, centroids_size);
							collected = 0;
							generate_iteration =0;
	 						filter_iteration = 0;
	 						features_iteration =0;
	 						assign_iteration = 0;
	 						accumulate_iteration = 0;
							utilization1= 0.0;
     							utilization2= 0.0;
     							utilization3= 0.0;
     							utilization4= 0.0;
     							utilization5= 0.0;
							break;
						}
        				
        			}
        			
    				
        		}
        	}
        }
    }
	test_histogram(sock_fd, points, number_points);
	
	free(centroids);
	free(points);
	//close_socket(sock_fd);
	omp_destroy_lock(&lock);
    return 0;
}
