#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "YUVTool.h"


enum ToolMode {
  RGBTOYUV420 = 0,
  BGRTOYUV420 = 1,
  NV12TOYUV420 = 2,
  YUV444TOYUV420 = 3,
};

void WriteFrame (const uint8_t* pSrc,  const int32_t iSrcWidth, const int32_t iTargetWidth, const int32_t iTargetHeight,
                 FILE* fp) {
  for (int j = 0; j < iTargetHeight; j++) {
    fwrite (pSrc + j * iSrcWidth, 1, iTargetWidth, fp);
  }
}


void Processing (int iMode, char* src_yuv, char* dst_yuv, int iWidth, int iHeight, int iEndFrames) {
  FILE* fpSrc = fopen (src_yuv, "rb");
  FILE* fpDst = fopen (dst_yuv, "wb");
  printf ("begin Processing\n");

  if (fpSrc == NULL) {
    printf ("Open Source file failed!\n");
    return;
  }
  if (fpDst == NULL) {
    printf ("Open Target file failed! %s\n", dst_yuv);
    return;
  }

  int32_t i, j, k;
  int32_t iFrameNum = 0;
  const int32_t iCWidth = iWidth >> 1;
  const int32_t iCHeight = iHeight >> 1;
  const int32_t iLumaSize = iWidth * iHeight;
  const int32_t iChromaSize = iCWidth * iCHeight;
  uint8_t* pDstY = (uint8_t*)malloc (iLumaSize);
  uint8_t* pDstU = (uint8_t*)malloc (iLumaSize);
  uint8_t* pDstV = (uint8_t*)malloc (iLumaSize);


  printf ("Start conversion %d\n", iMode);
  uint8_t* pSrcY = (uint8_t*)malloc (3 * iLumaSize);

  long long iFileLen = 0;

  if (fpSrc && !fseek (fpSrc, 0, SEEK_END)) {
    iFileLen = ftell (fpSrc);
    fseek (fpSrc, 0, SEEK_SET);
  }

  j = 0;
  int iFrameSize;
  long long iLeftLength = iFileLen;
  //rewrite each frame
  while (EOF != ftell (fpSrc) && iLeftLength) {
    switch (iMode) {
    case RGBTOYUV420:
      iFrameSize = iLumaSize * 3;
      fread (pSrcY, 3, iLumaSize, fpSrc);
      rgb_to_i420_c (pSrcY, iWidth * 3, pDstY, pDstU, pDstV,
                     iWidth, iWidth >> 1, iWidth, iHeight, 0);
      break;
    case BGRTOYUV420:
      iFrameSize = iLumaSize * 3;
      fread (pSrcY, 3, iLumaSize, fpSrc);
      bgr_to_i420_c (pSrcY, iWidth * 3, pDstY, pDstU, pDstV,
                     iWidth, iWidth >> 1, iWidth, iHeight, 0);
      break;
    case NV12TOYUV420:
      iFrameSize = iLumaSize + iChromaSize + iChromaSize;
      fread (pSrcY, 1, iFrameSize, fpSrc);
      nv12_to_i420_c (pSrcY, iWidth, pDstY, pDstU, pDstV,
                      iWidth, iWidth >> 1, iWidth, iHeight, 0);
      break;
    default:
      break;
    }

    WriteFrame (pDstY, iWidth, iWidth, iHeight, fpDst);
    WriteFrame (pDstU, iWidth >> 1, iWidth >> 1, iHeight >> 1, fpDst);
    WriteFrame (pDstV, iWidth >> 1, iWidth >> 1, iHeight >> 1, fpDst);

    iLeftLength -= iFrameSize;
    j++;
    iFrameNum ++;
    
    if (iEndFrames > 0 && j==iEndFrames) {
      break;
    }
  }

  printf ("Completed conversion, frame num is: %d\n", iFrameNum);
  fclose (fpDst);
  fclose (fpSrc);

  free (pSrcY);


  free (pDstY);
  free (pDstU);
  free (pDstV);
}
int main (int argc, char* argv[]) {
  if (argc != 6) {
    printf ("...the colorspace conversion is from wse vp...");
    printf ("usage: mode(0) Src.yuv Dst.yuv iWidth iHeight ...");
    printf ("usage: mode(0) RGBTOYUV420 mode(1) BGRTOYUV420");
    printf ("usage: mode(2) NV12TOYUV420");
  }

  const int32_t iMode = atoi (argv[1]);
  const int32_t iWidth = atoi (argv[4]);
  const int32_t iHeight = atoi (argv[5]);
  
  if (argc == 7) {
  const int32_t iEndFrames = atoi (argv[6]);
  printf ("input is: mode(%d) in out iWidth=%d iHeight=%d\n", iMode, iWidth, iHeight);
  Processing (iMode, argv[2], argv[3], iWidth, iHeight, iEndFrames);
  } else {
    Processing (iMode, argv[2], argv[3], iWidth, iHeight, 0);
  }

  fprintf (stdout, "Finish dealing!\n");
  return 0;
}

