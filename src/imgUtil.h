#pragma once
#include <set>
 
#include "framework.h"
#include <d3d11.h>
  

 
class ImgUtil {
public:
	static bool getBinaryDataByImgHandle(HBITMAP& in_hbitmap, byte** out_data, int* out_length, int* out_width, int* out_height);
	static bool getBinaryDataByIconHandle(HICON& in_hIcon, byte** out_data, int* out_length, int* out_width, int* out_height);

};
