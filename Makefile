LIBS=-lavformat -lavcodec -lavutil
INCS=-I/usr/local/openresty/luajit/include/luajit-2.1/

main: ffmpeg.o
	gcc ffmpeg.o  -o video2jpeg.so -shared -fPIC -O2 ${LIBS} ${INCS} video2jpeg.c 
ffmpeg.o: ffmpeg.c ffmpeg.h
	gcc -c ffmpeg.c  -fPIC
clean:
	rm -rf *.so *.o
