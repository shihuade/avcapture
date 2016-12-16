#include <memory.h>
#include "YUVTool.h"

//
//this file is extracted from wse vp
//
inline void * wels_memcpy( void *dst, const void *src, uint32_t size)
{
  return memcpy(dst, src, size);	// confirmed_safe_unsafe_usage
}

void	rgb_to_i420_c(uint8_t * x_ptr, int32_t x_stride, uint8_t * y_ptr, uint8_t * u_ptr, uint8_t * v_ptr,
                    int32_t y_stride, int32_t uv_stride, int32_t width, int32_t height, int32_t vflip)
{
  int32_t fixed_width = (width + 1) & ~1;
  int32_t x_step = (x_stride << 1) - ((fixed_width <<1) + fixed_width);
  int32_t y_step = (y_stride << 1) - fixed_width;
  int32_t uv_step = uv_stride - (fixed_width >> 1);
  int32_t x, y;
  if (vflip)
  {
    x_ptr += (height - 1) * x_stride;
    x_step = -((fixed_width << 1) + fixed_width) - (x_stride << 1);
    x_stride = -x_stride;
  }
  for (y = 0; y < height; y+=(2))
  {
    for (x = 0; x < fixed_width; x+=(2))
    {
      WELS_RGB_TO_YV12(3,0,1,2,0);
      x_ptr += (2)*(3);
      y_ptr += (2);
      u_ptr += (2)/2;
      v_ptr += (2)/2;
    }
    x_ptr += x_step;
    y_ptr += y_step;
    u_ptr += uv_step;	
    v_ptr += uv_step;	
  }
}


void	bgr_to_i420_c(uint8_t * x_ptr, int32_t x_stride,	uint8_t * y_ptr, uint8_t * u_ptr, uint8_t * v_ptr,
                    int32_t y_stride, int32_t uv_stride,	int32_t width, int32_t height, int32_t vflip)
{
  int32_t fixed_width = (width + 1) & ~1;
  int32_t x_step = (x_stride << 1) - ((fixed_width <<1) + fixed_width);
  int32_t y_step = (y_stride << 1) - fixed_width;
  int32_t uv_step = uv_stride - (fixed_width >> 1);
  int32_t x, y;
  if (vflip)
  {
    x_ptr += (height - 1) * x_stride;
    x_step = -((fixed_width << 1) + fixed_width) - (x_stride << 1);
    x_stride = -x_stride;
  }
  for (y = 0; y < height; y+=(2))
  {
    for (x = 0; x < fixed_width; x+=(2))
    {
      WELS_RGB_TO_YV12(3,2,1,0,0);
      x_ptr += (2)*(3);
      y_ptr += (2);
      u_ptr += (2)/2;
      v_ptr += (2)/2;
    }
    x_ptr += x_step;
    y_ptr += y_step;
    u_ptr += uv_step;	
    v_ptr += uv_step;	
  }
}

void	nv12_to_i420_c(uint8_t * x_ptr, int32_t x_stride,	uint8_t * y_ptr, uint8_t * u_ptr, uint8_t * v_ptr,
                     int32_t y_stride, int32_t uv_stride,	int32_t width, int32_t height, int32_t vflip)
{
  uint8_t* u_src_ptr = x_ptr + (width * height);
  uint8_t* v_src_ptr = u_src_ptr + 1;
  
  int32_t i, j;
  
  for (i=0; i<height; i++)
  {
    wels_memcpy(y_ptr, x_ptr, width);
    y_ptr += y_stride;
    x_ptr += x_stride;
  }
  
  for (i=0; i<height; i+=2)
  {
    for (j=0; j<width; j+=2)
    {
      u_ptr[j/2] = u_src_ptr[j]; //uuuuuu
      v_ptr[j/2] = v_src_ptr[j]; //vvvvvv
    }
    u_ptr += uv_stride;
    v_ptr += uv_stride;
    
    u_src_ptr += width;
    v_src_ptr += width;
  }
}