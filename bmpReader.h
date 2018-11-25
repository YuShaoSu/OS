/* 
	You can NOT modify anything in this file.
*/
#include <stdio.h>
#include <stdlib.h>

typedef struct BmpImage {
	unsigned char* data;
    int width;
    int height;
} BmpImage;

class BmpReader {
	
public:
	
	int Add(){
		return 1+1;
	}
	
	BmpImage ReadBMP(const char *f){
		
		BmpImage image;
		
		fp_s = NULL;    // source file handler
		image_s = NULL; // source image array
		
		static unsigned char tmp_header[54] = {
			0x42,        // identity : B
			0x4d,        // identity : M
			0, 0, 0, 0,  // file size
			0, 0,        // reserved1
			0, 0,        // reserved2
			54, 0, 0, 0, // RGB data offset
			40, 0, 0, 0, // struct BITMAPINFOHEADER size
			0, 0, 0, 0,  // bmp width
			0, 0, 0, 0,  // bmp height
			1, 0,        // planes
			24, 0,       // bit per pixel
			0, 0, 0, 0,  // compression
			0, 0, 0, 0,  // data size
			0, 0, 0, 0,  // h resolution
			0, 0, 0, 0,  // v resolution 
			0, 0, 0, 0,  // used colors
			0, 0, 0, 0   // important colors
		};
		
		header = tmp_header;
		
		fp_s = fopen(f, "rb");
		
		if (fp_s == NULL) {
			printf("fopen fp_s error\n");
			return image;
		}
		
		// move offset to 10 to find rgb raw data offset
		fseek(fp_s, 10, SEEK_SET);
		fread(&rgb_raw_data_offset, sizeof(unsigned int), 1, fp_s);
		// move offset to 18    to get width & height;
		fseek(fp_s, 18, SEEK_SET); 
		fread(&width,  sizeof(unsigned int), 1, fp_s);
		fread(&height, sizeof(unsigned int), 1, fp_s);
		// get  bit per pixel
		fseek(fp_s, 28, SEEK_SET); 
		fread(&bit_per_pixel, sizeof(unsigned short), 1, fp_s);
		byte_per_pixel = bit_per_pixel / 8;
		// move offset to rgb_raw_data_offset to get RGB raw data
		fseek(fp_s, rgb_raw_data_offset, SEEK_SET);
		
		image_s = (unsigned char *)malloc((size_t)width * height * byte_per_pixel);
		
		if (image_s == NULL) {
			printf("malloc images_s error\n");
			return image;
		}
		
		fread(image_s, sizeof(unsigned char), (size_t)(long)width * height * byte_per_pixel, fp_s);
		
		image.data = image_s;
		image.width = width;
		image.height = height;

		fclose(fp_s);
		
		return image;
	};
		
	int WriteBMP(const char *f,BmpImage bmpImage)
	{
		
		printf("Write %s\n",f);
		
		fp_t = NULL;    // target file handler 
		// write to new bmp
		fp_t = fopen(f, "wb");
		
		if (fp_t == NULL) {
			printf("fopen fname_t error\n");
			return -1;
		}
	
		// file size  
		file_size = bmpImage.width * bmpImage.height * byte_per_pixel + rgb_raw_data_offset;
		header[2] = (unsigned char)(file_size & 0x000000ff);
		header[3] = (file_size >> 8)  & 0x000000ff;
		header[4] = (file_size >> 16) & 0x000000ff;
		header[5] = (file_size >> 24) & 0x000000ff;
	
		// width
		header[18] = bmpImage.width & 0x000000ff;
		header[19] = (bmpImage.width >> 8) & 0x000000ff;
		header[20] = (bmpImage.width >> 16) & 0x000000ff;
		header[21] = (bmpImage.width >> 24) & 0x000000ff;
	
		// height
		header[22] = bmpImage.height & 0x000000ff;
		header[23] = (bmpImage.height >> 8) & 0x000000ff;
		header[24] = (bmpImage.height >> 16) & 0x000000ff;
		header[25] = (bmpImage.height >> 24) & 0x000000ff;
	
		// bit per pixel
		header[28] = bit_per_pixel;
	
		// write header
		fwrite(header, sizeof(unsigned char), rgb_raw_data_offset, fp_t);
		// write image
		fwrite(bmpImage.data, sizeof(unsigned char), (size_t)(long)bmpImage.width * bmpImage.height * byte_per_pixel, fp_t);
	
		fclose(fp_t);
	
		return 0;
	}

	
private:
	FILE          *fp_s;    // source file handler
	FILE          *fp_t;    // target file handler 
	unsigned int  x, y;             // for loop counter
	unsigned int  width, height;   // image width, image height
	unsigned char *image_s; // source image array
	unsigned char *header;
	unsigned int file_size;           // file size
	unsigned int rgb_raw_data_offset; // RGB raw data offset
	unsigned short bit_per_pixel;     // bit per pixel
	unsigned short byte_per_pixel;    // byte per pixel
};
