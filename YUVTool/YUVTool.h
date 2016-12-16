typedef unsigned char uint8_t;
typedef signed int         int32_t  ;
typedef unsigned int       uint32_t ;
typedef unsigned short     uint16_t ;

#define Y_R_IN			0.257
#define Y_G_IN			0.504
#define Y_B_IN			0.098
#define Y_ADD_IN		16

#define U_R_IN			0.148
#define U_G_IN			0.291
#define U_B_IN			0.439
#define U_ADD_IN		128

#define V_R_IN			0.439
#define V_G_IN			0.368
#define V_B_IN			0.071
#define V_ADD_IN		128

#define SCALEBITS_IN    8
#define FIX_IN(x)		((uint16_t) ((x) * (1L<<SCALEBITS_IN) + 0.5))
#define FIX_ROUND		(1L<<(SCALEBITS_IN-1))

#define WELS_RGB_Y(P_SIZE, ROW_NUM, UV, D1,D2,D3,D4)	\
r##UV += r = x_ptr[(ROW_NUM)*x_stride+(D1)];						\
g##UV += g = x_ptr[(ROW_NUM)*x_stride+(D2)];						\
b##UV += b = x_ptr[(ROW_NUM)*x_stride+(D3)];						\
y_ptr[(ROW_NUM)*y_stride] =									\
(uint8_t) ((FIX_IN(Y_R_IN) * r + FIX_IN(Y_G_IN) * g +	\
            FIX_IN(Y_B_IN) * b + FIX_ROUND) >> SCALEBITS_IN) + Y_ADD_IN;	\
r##UV += r = x_ptr[(ROW_NUM)*x_stride+(P_SIZE)+(D1)];				\
g##UV += g = x_ptr[(ROW_NUM)*x_stride+(P_SIZE)+(D2)];				\
b##UV += b = x_ptr[(ROW_NUM)*x_stride+(P_SIZE)+(D3)];				\
y_ptr[(ROW_NUM)*y_stride+1] =									\
(uint8_t) ((FIX_IN(Y_R_IN) * r + FIX_IN(Y_G_IN) * g +	\
            FIX_IN(Y_B_IN) * b + FIX_ROUND) >> SCALEBITS_IN) + Y_ADD_IN;

#define WELS_RGB_UV(ROW_NUM,UV)	\
u_ptr[(ROW_NUM)*uv_stride] =														\
(uint8_t) ((-FIX_IN(U_R_IN) * r##UV - FIX_IN(U_G_IN) * g##UV +			\
            FIX_IN(U_B_IN) * b##UV + 4*FIX_ROUND) >> (SCALEBITS_IN + 2)) + U_ADD_IN;	\
v_ptr[(ROW_NUM)*uv_stride] =														\
(uint8_t) ((FIX_IN(V_R_IN) * r##UV - FIX_IN(V_G_IN) * g##UV -			\
            FIX_IN(V_B_IN) * b##UV + 4*FIX_ROUND) >> (SCALEBITS_IN + 2)) + V_ADD_IN;

#define WELS_RGB_TO_YV12(P_SIZE,D1,D2,D3,D4)	\
uint32_t r, g, b, r0, g0, b0;		\
r0 = g0 = b0 = 0;					\
WELS_RGB_Y(P_SIZE, 0, 0, D1,D2,D3,D4)	\
WELS_RGB_Y(P_SIZE, 1, 0, D1,D2,D3,D4)	\
WELS_RGB_UV(     0, 0)


void	rgb_to_i420_c(uint8_t * x_ptr, int32_t x_stride, uint8_t * y_ptr, uint8_t * u_ptr, uint8_t * v_ptr,
                    int32_t y_stride, int32_t uv_stride, int32_t width, int32_t height, int32_t vflip);
void	bgr_to_i420_c(uint8_t * x_ptr, int32_t x_stride,	uint8_t * y_ptr, uint8_t * u_ptr, uint8_t * v_ptr,
                    int32_t y_stride, int32_t uv_stride,	int32_t width, int32_t height, int32_t vflip);
void	nv12_to_i420_c(uint8_t * x_ptr, int32_t x_stride,	uint8_t * y_ptr, uint8_t * u_ptr, uint8_t * v_ptr,
                     int32_t y_stride, int32_t uv_stride,	int32_t width, int32_t height, int32_t vflip);


