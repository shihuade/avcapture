YUVTool.app : YUVTool.o i420_dst_copy.o
	cc -o YUVTool.app YUVTool.o i420_dst_copy.o

YUVTool.o : YUVTool.cpp
	cc -c YUVTool.cpp

i420_dst_copy.o : i420_dst_copy.cpp
	cc -c i420_dst_copy.cpp

clean :
	rm ./*.o
	rm YUVTool.app
