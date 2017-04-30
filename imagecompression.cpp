#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <omp.h>
#include <sys/time.h>

using namespace cv;
using namespace std;

unsigned char* encode(unsigned char *src)
{     
  int rLen;
  char count[50];
  int len = strlen((char*)src);
   
  unsigned char *dest = new unsigned char[len*2+1];
  
  int i, j = 0, k;
 
  for(i = 0; i < len; i++)
  {
    
    dest[j++] = src[i];
     
    rLen = 1;     
    while(i + 1 < len && src[i] == src[i+1])
    {
      rLen++;
      i++;
    }   
     
    sprintf(count, "%d", rLen);
 
    for(k = 0; *(count+k); k++, j++)
    { 
      dest[j] = count[k]; 
    } 
  }  
  dest[j] = '\0';
  return dest;
}     

int main( int argc, char** argv )
{

    struct timeval start, end;
    gettimeofday(&start, NULL);
	
    if( argc != 2)
    {
     cout <<" Usage: display_image ImageToLoadAndDisplay" << endl;
     return -1;
    }

    Mat image;
    image = imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);   

    if(! image.data )                              
    {
        cout <<  "Could not open or find the image" << std::endl ;
        return -1;
    }
    
    ofstream file;
    file.open("output.txt");
    
   
   unsigned char pixels[image.rows][image.cols+1]  ;
   unsigned char* fin[image.rows] ;
   
   
#pragma omp parallel for
   for(int i=0;i<image.rows;++i){
   	for(int j=0;j<image.cols;++j){
   		pixels[i][j] = image.at<uchar>(i,j);
   		//cout<<pixels[i][j];	
   }	
   
#pragma omp parallel for schedule(dynamic)
   for(int i=0;i<image.rows;i++){
   	fin[i] = encode((unsigned char*)pixels[i]);
   	//printf("Done by thread :%d\n",omp_get_thread_num());
   }
   
   for(int i=0;i<image.rows;i++)
   	file<<fin[i]<<endl;
   
   
   file.close();  	
     	
   gettimeofday(&end, NULL);

   double delta = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;  
   cout<<"Execution time :"<<delta<<" seconds."<<endl;	
   
   
   return 0;
}
