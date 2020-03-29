```bash
$ docker pull spmallick/opencv-docker:opencv-3.4.3
$ docker run -ti --rm spmallick/opencv-docker:opencv-3.4.3 /bin/bash

$ g++ -g ./main.cpp -std=c++11 -o out.o $(pkg-config opencv --libs --cflags)

$ INPUT=./image/s2.png \
IFFT_OUPUT=./image/ifft_out.jpg \
TEXT="test" \
RESULT_SPATIAL=./image/result_s.jpg \
RESULT_FFT=./image/result_fft.jpg \
./out.o
```
