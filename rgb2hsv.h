#ifndef __RGB2HSV_H__
#define __RGB2HSV_H__


#include <stdint.h>
#include <float.h>

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

typedef struct RGBcolor {
	uint8_t R;
	uint8_t G;
	uint8_t B;
}RGBcolor;

typedef struct HSVcolor {
	float H;
	float S;
	float V;//luminosity
}HSVcolor;


static HSVcolor rgb2hsv(RGBcolor rgbColor) {
	//R, G and B input range = 0 � 255
	//H, S and V output range = 0 � 1.0
	HSVcolor hsvColor;
	float del_R, del_G, del_B;

	float var_R = ((float)rgbColor.R / 255.0f);
	float var_G = ((float)rgbColor.G / 255.0f);
	float var_B = ((float)rgbColor.B / 255.0f);

	float var_Min = MIN(MIN(var_R, var_G), var_B);    //Min. value of RGB
	float var_Max = MAX(MAX(var_R, var_G), var_B);    //Max. value of RGB
	float del_Max = var_Max - var_Min;             //Delta RGB value

	hsvColor.V = var_Max;
	
		
	if (fabs(del_Max) < FLT_EPSILON)                     //This is a gray, no chroma...
	{
		hsvColor.H = 0.0f;
		hsvColor.S = 0.0f;
	}
	else                                    //Chromatic data...
	{
		hsvColor.S = del_Max / var_Max;

		del_R = (((var_Max - var_R) / 6.0f) + (del_Max / 2.0f)) / del_Max;
		del_G = (((var_Max - var_G) / 6.0f) + (del_Max / 2.0f)) / del_Max;
		del_B = (((var_Max - var_B) / 6.0f) + (del_Max / 2.0f)) / del_Max;

		if (var_R == var_Max) hsvColor.H = del_B - del_G;
		else if (var_G == var_Max) hsvColor.H = (1.0f / 3.0f) + del_R - del_B;
		else if (var_B == var_Max) hsvColor.H = (2.0f / 3.0f) + del_G - del_R;

		if (hsvColor.H < 0.0f) hsvColor.H += 1.0f;
		if (hsvColor.H > 1.0f) hsvColor.H -= 1.0f;
	}

	return hsvColor;
}


inline uint8_t rgb2greyscale(RGBcolor rgbColor) {
	return (uint8_t)(((float)rgbColor.R * 0.299f) + ((float)rgbColor.G * 0.587f) + ((float)rgbColor.B * 0.114f));
}

#endif // !__RGB2HSV_H__
