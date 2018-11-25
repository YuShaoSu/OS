#include "bmpReader.h"
#include <stdio.h>
#include <math.h>
#include <sys/time.h>

const int BILINEAR_RATIO = 7;
float ROTATION_ANGLE = 45;
float SHEAR_VERTICAL_DEGREE = 0.45;
float SHEAR_HORIZONTAL_DEGREE = 0.45;

const char* INPUT_IMAGE = "input.bmp";
const char* OUT_IMAGE[5] = {"output1.bmp", "output2.bmp", "output3.bmp", "output4.bmp", "output5.bmp"};

BmpImage bilinear(BmpImage pic, int nw, int nh);
BmpImage rotation(BmpImage pic, int nw,int nh);
BmpImage shear_vertical(BmpImage pic, int nw, int nh);
BmpImage shear_horizontal(BmpImage pic, int nw, int nh);

// TODO: add your pthread codes to speed up the program
pthread_t p[15];
int main(){
	
	struct timeval  tv1, tv2;
	
	// timing function 1
	gettimeofday(&tv1, NULL);
	
	BmpReader* bmpReader = new BmpReader();
	BmpImage input_pic = bmpReader->ReadBMP(INPUT_IMAGE);

	int newWidth = (int)(input_pic.width * BILINEAR_RATIO);
	int newHeight = (int)(input_pic.height * BILINEAR_RATIO);
	
	//all image processing
	BmpImage bilinear_pic = bilinear(input_pic, newWidth, newHeight);
	BmpImage rotation_pic = rotation(bilinear_pic, newWidth, newHeight);
	BmpImage shearVert_pic = shear_vertical(bilinear_pic, newWidth, newHeight);
	BmpImage shearHori_pic = shear_horizontal(bilinear_pic, newWidth, newHeight);

	BmpImage tmp_pic = rotation(shearVert_pic, newWidth, newHeight);
	BmpImage mix_pic = shear_horizontal(tmp_pic, newWidth, newHeight);
	
	//Write bmp files
	bmpReader->WriteBMP(OUT_IMAGE[0], bilinear_pic);
	bmpReader->WriteBMP(OUT_IMAGE[1], rotation_pic);
	bmpReader->WriteBMP(OUT_IMAGE[2], shearVert_pic);
	bmpReader->WriteBMP(OUT_IMAGE[3], shearHori_pic);
	bmpReader->WriteBMP(OUT_IMAGE[4], mix_pic);
	
	//free memory
	free(input_pic.data);
	free(bilinear_pic.data);
	free(rotation_pic.data);
	free(shearVert_pic.data);
	free(shearHori_pic.data);
	free(tmp_pic.data);
	free(mix_pic.data);	
	
	// timing function 2
	gettimeofday(&tv2, NULL);
	
	// TODO: modify (ID) to your student ID
	printf("0516016%f seconds\n", (double)(tv2.tv_usec - tv1.tv_usec) / 1000000 + (double)(tv2.tv_sec - tv1.tv_sec)); 
	
	// TODO: add your pthread codes to speed up the program
	pid_t p[16];
	pthread_create(p[0], NULL, bilinear, para);
	
	return 0;
} 

BmpImage bilinear(BmpImage pic, int newWidth, int newHeight)
{
	printf("bilinear\n");
	unsigned char* data	= (unsigned char*)malloc(3 * newWidth * newHeight * sizeof(unsigned char));

	for(int i = 0 ; i < newWidth; i++){
		for(int j = 0;j < newHeight; j++){
			//k is RGB color, R = 2, G = 1, B = 0
			for(int k = 0;k < 3;k++){
				int pixel = 0;
				int relevant_w = (int)(i / BILINEAR_RATIO);
				int relevant_h = (int)(j / BILINEAR_RATIO);
				int reference_points[4];
				
				// the four reference points of this pixel
				reference_points[0] = pic.data[3 * (relevant_h * pic.width + relevant_w) + k];
				reference_points[1] = pic.data[3 * (relevant_h * pic.width + (relevant_w + 1)) + k];
				reference_points[2] = pic.data[3 * ((relevant_h + 1) * pic.width + relevant_w) + k];
				reference_points[3] = pic.data[3 * ((relevant_h + 1) * pic.width + (relevant_w + 1)) + k];
				
				// TODO: assign a new value to this pixel
				// pixel = 
				
				if (pixel < 0) pixel = 0;
				if (pixel > 255) pixel = 255;	
				
				data[3 * (j * newWidth + i) + k] = pixel;
			}
		}
	}

	BmpImage newBmpImage;

	newBmpImage.data = data;
	newBmpImage.width = newWidth;
	newBmpImage.height = newHeight;
	
	return newBmpImage;
}

BmpImage rotation(BmpImage pic,int nw,int nh)
{
	printf("rotation\n");
	unsigned char* data	= (unsigned char*)malloc(3 * nw * nh * sizeof(unsigned char));
	
	int w_offset = (int)(nw / 2);
	int h_offset = (int)(nh / 2);
	
	for(int i = 0 ; i < nw; i++){
		for(int j = 0; j < nh; j++){
			for(int k = 0 ; k < 3 ; k++){
				
				double angle = (double)ROTATION_ANGLE * M_PI / 180.0;
				double cos_theta = cos(angle);    
				double sin_theta = sin(angle);
				int relevant_w = i - w_offset;
				int relevant_h = j - h_offset;
				int trans_w = (int)(relevant_w * cos_theta - relevant_h * sin_theta) + w_offset;
				int trans_h = (int)(relevant_h * cos_theta + relevant_w * sin_theta) + h_offset;
				
				int pixel;
				
				if (trans_w >= 0 && trans_w < w_offset * 2 && trans_h >= 0 && trans_h < h_offset * 2){
					pixel = pic.data[3 * (trans_h * 2 * w_offset + trans_w) + k];
				}else{
					pixel = 0;
				}
				
				data[3 * (j * nw + i) + k] = pixel;
			}
		}
	}
	
	BmpImage newBmpImage;

	newBmpImage.data = data;
	newBmpImage.width = nw;
	newBmpImage.height = nh;
	
	return newBmpImage;
}


BmpImage shear_vertical(BmpImage pic, int nw, int nh)
{
	printf("shear vertical\n");
	
	unsigned char* data	= (unsigned char*)malloc(3 * nw * nh * sizeof(unsigned char));

	for(int i = 0 ; i < nw ; i++){
		for(int j = 0 ; j < nh ; j++){
			//k is RGB color, R = 2, G = 1, B = 0
			for(int k = 0;k < 3;k++){
				int pixel = 0;
				int h_offset = (int)(nw * SHEAR_VERTICAL_DEGREE / 2);
				int trans_w = i;
				int trans_h = (int)(j + i * SHEAR_VERTICAL_DEGREE) - h_offset;
				
				if (trans_h >= 0 && trans_h < nh){
					pixel = pic.data[3 * (trans_h * nw + trans_w) + k];
				}else{
					pixel = 0;
				}
				
				data[3 * (j * nw + i) + k] = pixel;
			}
		}
	}
	
	BmpImage newBmpImage;

	newBmpImage.data = data;
	newBmpImage.width = nw;
	newBmpImage.height = nh;

	return newBmpImage;
}

BmpImage shear_horizontal(BmpImage pic, int nw, int nh)
{
	printf("shear horizontal\n");
	
	unsigned char* data	= (unsigned char*)malloc(3 * nw * nh * sizeof(unsigned char));

	for(int i = 0 ; i < nw ; i++){
		for(int j = 0 ; j < nh ; j++){
			//k is RGB color, R = 2, G = 1, B = 0
			for(int k = 0;k < 3;k++){
				int pixel = 0;
				
				// TODO: refer to shear_vertical, try to write shear_horizontal on your own
				
				data[3 * (j * nw + i) + k] = pixel;
			}
		}
	}
	
	BmpImage newBmpImage;

	newBmpImage.data = data;
	newBmpImage.width = nw;
	newBmpImage.height = nh;

	return newBmpImage;
}

