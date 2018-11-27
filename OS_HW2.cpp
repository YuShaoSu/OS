#include "bmpReader.h"
#include <stdio.h>
#include <math.h>
#include <sys/time.h>
#include <pthread.h>

const int BILINEAR_RATIO = 7;
float ROTATION_ANGLE = 45;
float SHEAR_VERTICAL_DEGREE = 0.45;
float SHEAR_HORIZONTAL_DEGREE = 0.45;

const char* INPUT_IMAGE = "input.bmp";
const char* OUT_IMAGE[5] = {"output1.bmp", "output2.bmp", "output3.bmp", "output4.bmp", "output5.bmp"};


BmpImage bilinear(BmpImage pic, int nw, int nh);
BmpImage rotation(BmpImage pic, int nw,int nh, double angle_ar);
BmpImage shear_vertical(BmpImage pic, int nw, int nh, double sv);
BmpImage shear_horizontal(BmpImage pic, int nw, int nh, double sh);
BmpReader* bmpReader = new BmpReader();

// TODO: add your pthread codes to speed up the program
struct para {
	BmpImage pic;
	int nw;
	int nh;
	double fun_ro;
	double fun_sv;
	double fun_sh;
};

pthread_t p[15];
BmpImage rotation_pic_p, shear_ver_pic_p, shear_hor_pic_p, bilinear_pic;

void *bilinear_write_p(void *arg){
	bmpReader->WriteBMP(OUT_IMAGE[0], bilinear_pic);
}

void *rotation_p(void *arg){
	para arg_use = *(para *) arg;
	bmpReader -> WriteBMP(OUT_IMAGE[1], rotation(arg_use.pic, arg_use.nw, arg_use.nh, arg_use.fun_ro));
	pthread_exit(0);
}

void *shear_ver_p(void * arg){
	para *arg_use = (para *) arg;
	bmpReader -> WriteBMP(OUT_IMAGE[2], shear_vertical(arg_use -> pic, arg_use -> nw, arg_use -> nh, arg_use->fun_sv));
	pthread_exit(0);
}

void *shear_hor_p(void * arg){
	para *arg_use = (para *) arg;
	bmpReader -> WriteBMP(OUT_IMAGE[3], shear_horizontal(arg_use -> pic, arg_use -> nw, arg_use -> nh, arg_use->fun_sh));
	pthread_exit(0);
}

void *mix_p(void *arg){
	para *arg_use = (para *) arg;
	shear_ver_pic_p = shear_vertical(arg_use -> pic, arg_use -> nw, arg_use -> nh, arg_use -> fun_sv);
	rotation_pic_p = rotation(shear_ver_pic_p, arg_use -> nw, arg_use -> nh, arg_use -> fun_ro);
	bmpReader -> WriteBMP(OUT_IMAGE[4], shear_horizontal(rotation_pic_p, arg_use -> nw, arg_use -> nh, arg_use -> fun_sv));
	pthread_exit(0);
}

int main(){
	
	struct timeval  tv1, tv2;
	
	// timing function 1
	gettimeofday(&tv1, NULL);
	
	BmpImage input_pic = bmpReader->ReadBMP(INPUT_IMAGE);

	int newWidth = (int)(input_pic.width * BILINEAR_RATIO);
	int newHeight = (int)(input_pic.height * BILINEAR_RATIO);

	//all image processing
	bilinear_pic = bilinear(input_pic, newWidth, newHeight);
	BmpImage shearVert_pic = shear_vertical(bilinear_pic, newWidth, newHeight, 0.3);
	BmpImage rotation_pic = rotation(bilinear_pic, newWidth, newHeight, ROTATION_ANGLE);
	BmpImage shearHori_pic = shear_horizontal(bilinear_pic, newWidth, newHeight, 0.6);

	BmpImage tmp1_pic = shear_vertical(bilinear_pic, newWidth, newHeight, 0.45);
	BmpImage tmp2_pic = rotation(tmp1_pic, newWidth, newHeight, 60);
	BmpImage mix_pic = shear_horizontal(tmp2_pic, newWidth, newHeight, 0.45);

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
	free(tmp1_pic.data);
	free(tmp2_pic.data);
	free(mix_pic.data);

	// timing function 2
	gettimeofday(&tv2, NULL);
	
	// TODO: modify (ID) to your student ID
	printf("0516016 %f seconds\n", (double)(tv2.tv_usec - tv1.tv_usec) / 1000000 + (double)(tv2.tv_sec - tv1.tv_sec)); 
	
	// TODO: add your pthread codes to speed up the program
	gettimeofday(&tv1, NULL);

	input_pic = bmpReader->ReadBMP(INPUT_IMAGE);
	bilinear_pic = bilinear(input_pic, newWidth, newHeight);

	para arg1, arg2;
	arg1.pic = bilinear_pic;
	arg1.nw = newWidth;
	arg1.nh = newHeight;	
	arg1.fun_ro = 45;
	arg1.fun_sv = 0.3;
	arg1.fun_sh = 0.6;

	arg2.pic = bilinear_pic;
	arg2.nw = newWidth;
	arg2.nh = newHeight;
	arg2.fun_ro = 60;
	arg2.fun_sv = 0.45;
	arg2.fun_sh = 0.45;
	

	pthread_create(&p[0], NULL, bilinear_write_p, (void*) &arg1);
	pthread_create(&p[1], NULL, rotation_p, (void*) &arg1 );
	pthread_create(&p[2], NULL, shear_ver_p, (void*) &arg1 );
	pthread_create(&p[3], NULL, shear_hor_p, (void*) &arg1 );
	pthread_create(&p[4], NULL, mix_p, (void*) &arg2 );

	pthread_join(p[0], NULL);
	pthread_join(p[1], NULL);
	pthread_join(p[2], NULL);
	pthread_join(p[3], NULL);
	pthread_join(p[4], NULL);

	gettimeofday(&tv2, NULL);

	printf("0516016 %f seconds\n", (double)(tv2.tv_usec - tv1.tv_usec) / 1000000 + (double)(tv2.tv_sec - tv1.tv_sec)); 
	
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
				
				pixel = ((relevant_w + 1 - i) * (relevant_h + 1 -j) * reference_points[0]) + (( i - relevant_w) * (relevant_h + 1 -j) * reference_points[1]);
				pixel += ((relevant_w + 1 - i) * (j - relevant_h) * reference_points[2]) + (( i - relevant_w) * (j - relevant_h) * reference_points[3]);
				
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

BmpImage rotation(BmpImage pic,int nw,int nh, double angle_ar)
{
	printf("rotation\n");
	unsigned char* data	= (unsigned char*)malloc(3 * nw * nh * sizeof(unsigned char));
	
	int w_offset = (int)(nw / 2);
	int h_offset = (int)(nh / 2);
	
	for(int i = 0 ; i < nw; i++){
		for(int j = 0; j < nh; j++){
			for(int k = 0 ; k < 3 ; k++){
				
				double angle = (double)angle_ar * M_PI / 180.0;
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


BmpImage shear_vertical(BmpImage pic, int nw, int nh, double sv)
{
	printf("shear vertical\n");
	
	unsigned char* data	= (unsigned char*)malloc(3 * nw * nh * sizeof(unsigned char));

	for(int i = 0 ; i < nw ; i++){
		for(int j = 0 ; j < nh ; j++){
			//k is RGB color, R = 2, G = 1, B = 0
			for(int k = 0;k < 3;k++){
				int pixel = 0;
				int h_offset = (int)(nw * sv / 2);
				int trans_w = i;
				int trans_h = (int)(j + i * sv) - h_offset;
				
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

BmpImage shear_horizontal(BmpImage pic, int nw, int nh, double sh)
{
	printf("shear horizontal\n");
	
	unsigned char* data	= (unsigned char*)malloc(3 * nw * nh * sizeof(unsigned char));

	for(int i = 0 ; i < nw ; i++){
		for(int j = 0 ; j < nh ; j++){
			//k is RGB color, R = 2, G = 1, B = 0
			for(int k = 0;k < 3;k++){
				int pixel = 0;
				
				// TODO: refer to shear_vertical, try to write shear_horizontal on your own
				int w_offset = (int)(nh * sh / 2);
				int trans_w = (int)(i + j * sh) - w_offset;
				int trans_h = j;

				if (trans_w >= 0 && trans_w < nw){
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

