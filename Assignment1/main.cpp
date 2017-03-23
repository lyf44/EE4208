/*
 * main.cpp
 *
 *  Created on: 21 Feb 2017
 *      Author: luyun
 */
#include <iostream>
#include <fstream>
#include <vector>
#include "../eigen/Eigen/Dense"
#include <bitset>
#include <math.h>
#include <stdio.h>
#define PI 3.1415926

using namespace std;

uint16_t rowNum;
uint16_t colNum;
unsigned int bitNum;
vector<vector<float> > GaussianMask;
uint8_t imageData[243][190];
uint8_t imageData1[243][190];//after Gaussian smoothing
uint8_t imageData2[243][190];//after Sobel Mask_x
uint8_t imageData3[243][190];//after Sobel Mask_y
uint8_t imageData4[243][190];//after combining sobel mask
uint8_t imageData5[243][190];//after thinning the edge
uint8_t imageData6[243][190];//after direction checking
float imageData_edgeDir[243][190]; //edge direction
int8_t sobelMask_y[3][3];
int8_t sobelMask_x[3][3];

void outputImage(const char* fileName,uint8_t imageData[][190]){
    int size = rowNum * colNum;
    char buffer[size-1];
    for (int i=0;i<rowNum;i++){
    	for (int j=0;j<colNum;j++){
    		buffer[i*colNum+j]=imageData[i][j];
    	}
    }

    ofstream myFile (fileName, ios::out | ios::binary);
    myFile.write (buffer, size);
}

void GaussianSmooth(uint8_t imageData[][190], uint8_t imageData_new[][190], vector<vector<float> > &Gaussianmask){ //WARNING, change the function parameter if mask size changes
	int maskSize = 3;
	for (int y=0;y<colNum;y++){
		imageData_new[0][y] = 0;
		imageData_new[242][y] = 0;
	}
	for (int x=0;x<rowNum;x++){
		imageData_new[x][0] = 0;
		imageData_new[x][189] = 0;
	}
    for (int x=1;x<rowNum;x++){
    	for (int y=1;y<colNum;y++){
    		//x and y is where the convolution result will be stored.
    		float sum=0.0f;
    	    for (int i=0;i<maskSize;i++){
    	    	int temp_i = i-(int)(maskSize/2);// if mask size =3, then temp_i will be -1,0,1
    	    	for (int j=0;j<maskSize;j++){
    	    		int temp_j = j-(int)(maskSize/2);// if mask size =3, then temp_j will be -1,0,1
    	    		float product = GaussianMask[i][j] * imageData[x+temp_i][y+temp_j];
    	    		//cout<<(x+temp_i)<<"  "<<(y+temp_j)<<endl;
    	    		sum += product;
    	    	}
    	    }
    	    sum /= 9;
    	    //cout<<"sum= "<<sum<<endl;
    	    imageData_new[x][y] = (uint8_t) sum;
    	}
    }
}

void convolute(uint8_t imageData[][190], uint8_t imageData_new[][190], int8_t mask[][3]){ //WARNING, change the function parameter if mask size changes
	int maskSize = 3;
	for (int y=0;y<colNum;y++){
		imageData_new[0][y] = 0;
		imageData_new[242][y] = 0;
	}
	for (int x=0;x<rowNum;x++){
		imageData_new[x][0] = 0;
		imageData_new[x][189] = 0;
	}
    for (int x=1;x<rowNum;x++){
    	for (int y=1;y<colNum;y++){
    		//x and y is where the convolution result will be stored.
    		float sum=0.0f;
    	    for (int i=0;i<maskSize;i++){
    	    	int temp_i = i-(int)(maskSize/2);// if mask size =3, then temp_i will be -1,0,1
    	    	for (int j=0;j<maskSize;j++){
    	    		int temp_j = j-(int)(maskSize/2);// if mask size =3, then temp_j will be -1,0,1
    	    		float product = mask[i][j] * imageData[x+temp_i][y+temp_j];
    	    		//cout<<(x+temp_i)<<"  "<<(y+temp_j)<<endl;
    	    		sum += product;
    	    	}
    	    }
    	    sum /= 9;
    	    //cout<<"csum= "<<sum<<endl;
    	    imageData_new[x][y] = sum;
    	}
    }
}



/*calculate gaussianmask*/
void calculateGaussianMask(int maskSize,float sigma){
    for (int i=0;i<maskSize;i++){
    	vector<float> temp;
    	for(int j=0;j<maskSize;j++){
    		//TODO
    		int temp_i = i-(int)(maskSize/2);
    		int temp_j = j-(int)(maskSize/2);
    		//cout<<temp_i<<" "<<temp_j<<endl;
    		float temp2 = exp((temp_i*temp_i+temp_j*temp_j)/(2*sigma*sigma)*(-1));
    		temp.push_back(temp2);
    		//cout<<temp2<<endl;
    	}
    	GaussianMask.push_back(temp);
    }
}

void getSobelMask(){
	sobelMask_y[0][0] = -1;
	sobelMask_y[0][1] = -2;
	sobelMask_y[0][2] = -1;
	sobelMask_y[1][0] = 0;
	sobelMask_y[1][1] = 0;
	sobelMask_y[1][2] = 0;
	sobelMask_y[2][0] = 1;
	sobelMask_y[2][1] = 2;
	sobelMask_y[2][2] = 1;

	sobelMask_x[0][0] = -1;
	sobelMask_x[0][1] = 0;
	sobelMask_x[0][2] = 1;
	sobelMask_x[1][0] = -2;
	sobelMask_x[1][1] = 0;
	sobelMask_x[1][2] = 2;
	sobelMask_x[2][0] = -1;
	sobelMask_x[2][1] = 0;
	sobelMask_x[2][2] = 1;
}


int main (){
	cout<<"Hello World"<<endl;
	char buff[5];
    FILE * imageFile;
    imageFile = fopen("image/leaf.raw","rb");
    /*
	ifsream in("image/leaf.raw", ios::in | ios::binary);
	//vector<uint8_t> header(5);
	char buffer[5];
	in.read(buffer,5);
	//rowNum = (unsigned int) buffer[1];
    rowNum = ((unsigned int)buffer[1]<<8) | (unsigned int)buffer[0];
    colNum = (((unsigned int)buffer[3]<<8) | (unsigned int)buffer[2]);
    bitNum = (unsigned int)buffer[4];
    cout<<"rowNum: "<<rowNum<<"colNum£º "<<colNum<<"bitNum: "<<bitNum<<endl;
	*/

    /*get Header  -> rowNum, colNum and BitperPixel*/
    fscanf(imageFile,"%c",&buff[0]);
    fscanf(imageFile,"%c",&buff[1]);
    colNum = ((unsigned int)buff[1]<<8) + (unsigned int)(buff[0]&0xff);

    fscanf(imageFile,"%c",&buff[2]);
    fscanf(imageFile,"%c",&buff[3]);
    rowNum = ((unsigned int)buff[3]<<8) + (unsigned int)(buff[2]&0xff);
    fscanf(imageFile,"%c", &buff[4]);
    bitNum = (unsigned int)(buff[4]&0x08);
    cout<<"rowNum: "<<rowNum<<"colNum£º "<<colNum<<"bitNum: "<<bitNum<<endl;
    //std::cout<<std::bitset<8>(buff[4])<<std::endl;


    for (int i =0; i<rowNum;i++){
    	//vector<unsigned int> temp;
    	for (int j =0;j<colNum;j++){
    		char pixbuff;
    		fscanf(imageFile,"%c", &pixbuff);
    		uint8_t temp = (uint8_t)pixbuff;
    		imageData[i][j] = temp;
    		//temp.push_back((unsigned int)pixbuff);
    	}
    	//imageData.push_back(temp);
    }

    /* test output*/
    string outputFileName_str = "test/leaf1.raw";
    const char* outputFileName = outputFileName_str.c_str();
    outputImage(outputFileName,imageData);

    float sigma = 1.0f;
    int maskSize = (int)3*sigma;
    calculateGaussianMask(maskSize,sigma);

    //Gaussian smoothing
    GaussianSmooth(imageData,imageData1,GaussianMask);
    outputFileName_str = "test/leaf2.raw";
    outputFileName = outputFileName_str.c_str();
    outputImage(outputFileName,imageData1);

    //get Sobel Masks
    getSobelMask();
    convolute(imageData1,imageData2,sobelMask_x);
    convolute(imageData1,imageData3,sobelMask_y);

    //get edge magnitude and direction
    for (int i=0;i<rowNum;i++){
    	for(int j=0;j<colNum;j++){
    		imageData4[i][j] = (uint8_t) sqrt(imageData2[i][j]*imageData2[i][j]+imageData3[i][j]*imageData3[i][j]);
    		//printf("haha");
    		//printf("%d \n",imageData4[i][j]);//imageData4[i][j]<<endl;
    		if (imageData4[i][j]>255){
    			imageData4[i][j]=255;
    			cout<<"OhOh"<<endl;
    		}
    		if ((imageData3[i][j] != 0) && (imageData4[i][j] != 0)){
    			float temp = atan2(imageData3[i][j],imageData2[i][j])/PI*180;
    			if (temp>-45 && temp<=45){
    				temp = 0;
    			}else if (temp>45 && temp<=135){
    				temp = 90;
    			}else if ((temp>135 && temp<=180) || (temp<=-135 && temp>=-180)){
    				temp = -180;
    			}else if (temp>-135 && temp<=-45){
    				temp = -90;
    			}
    			imageData_edgeDir[i][j] = temp;
    		}else{
    			imageData_edgeDir[i][j] = -1000;
    		}
    	}
    }

    //output image test
    outputFileName_str = "test/leaf3.raw";
    outputFileName = outputFileName_str.c_str();
    outputImage(outputFileName,imageData4);

    //thinning edge

    bool flag_keep = false;
    for (int i=0;i<rowNum;i++){
    	for(int j=0;j<colNum;j++){
    		flag_keep = true;
    		//check against neighbouring pixel magnitude
    		for (int a=i-1;a<=i+1;a++){
    			if (flag_keep){
    				for (int b=j-1;b<=j+1;b++){
    					if (imageData4[i][j] < imageData4[a][b]){
    						imageData5[i][j] = 0;
    						flag_keep = false;
    						break;
    					}
    				}
    			}
    		}

    		if (flag_keep){
    			imageData5[i][j] = imageData4[i][j];
    		}

    	}
    }

    //output image test
    outputFileName_str = "test/leaf4.raw";
    outputFileName = outputFileName_str.c_str();
    outputImage(outputFileName,imageData5);

    //edge direction check
    int threshold_high = 200;
    int threshold_low = 100;
    for (int i=0;i<rowNum;i++){
    	for(int j=0;j<colNum;j++){
    		flag_keep = false;
    		if (imageData5[i][j]<=threshold_low){
    			imageData6[i][j] = 0;
    			continue;
    		}

    		if (imageData5[i][j]>threshold_high){
    			imageData6[i][j] = imageData5[i][j];
    			continue;
    		}
    		//check against neighbouring pixel direction, only keep if direction agrees
    		for (int a=i-1;a<=i+1;a++){
    			if (flag_keep){
    				for (int b=j-1;b<=j+1;b++){
    					if ((imageData_edgeDir[i][j] == imageData_edgeDir[a][b]) && (imageData_edgeDir[i][j] != (-1000))){
    						imageData6[i][j] = imageData5[i][j];
    						flag_keep = true;
    						break;
    					}
    				}
				}
			}
    		if (!flag_keep){
    			imageData6[i][j] = 0;
    		}
		}
	}

    //output image test
    outputFileName_str = "test/leaf_final.raw";
    outputFileName = outputFileName_str.c_str();
    outputImage(outputFileName,imageData6);



	return 0;
}



