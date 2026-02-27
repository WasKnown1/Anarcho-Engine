C_SRC = src/aeMain.cpp src/aeDraw.cpp src/aeThread.cpp src/aeInput.cpp src/aeGpu.cpp
OBJ = $(C_SRC:src/%.cpp=%.o)
C_ARGS = -Iinclude -I"C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v12.3\include" -L"C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v12.9\lib\x64" -luser32 -lgdi32 -pthread -lOpenCL

all:
	for file in $(C_SRC); do \
		g++ -c $$file -o $$(basename $$file .cpp).o $(C_ARGS); \
	done

	g++ -c test.cpp -o test.o $(C_ARGS) 
	g++ $(OBJ) test.o -o main.exe $(C_ARGS)

clean:
	rm *.o main.exe