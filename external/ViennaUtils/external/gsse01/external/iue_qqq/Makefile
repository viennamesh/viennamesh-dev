SRC     = src/mattools.cpp src/numerics_bicgs.cpp src/numerics_eigen.cpp src/numerics_gmres.cpp src/numerics_lu.cpp src/numerics_param.cpp \
	src/numerics_scale.cpp src/numerics_vector.cpp src/numerics_cmk.cpp src/numerics_eqs.cpp src/numerics_ilu.cpp src/numerics_mcsr.cpp\
	src/numerics_reduce.cpp src/numerics_solve.cpp src/type_funlog.cpp

OBJS    = $(SRC:.cpp=.o)

INCLUDES=-Isrc/

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c  $< -o $@  # -O3 -march=k8 -mtune=k8 -ffast-math


all:    libqqq.a 

libqqq.a:  $(OBJS)
	ar rc lib/libqqq.a $(OBJS)       

testmat:
	g++ -o mattest1 test/mattest1.cpp -Isrc/ -Llib/ -lqqq
	g++ -o mattest2 test/mattest2.cpp -Isrc/ -Llib/ -lqqq
	g++ -o mattest3 test/mattest3.cpp -Isrc/ -Llib/ -lqqq
	g++ -o mattest4 test/mattest4.cpp -Isrc/ -Llib/ -lqqq
	g++ -o mattest5 test/mattest5.cpp -Isrc/ -Llib/ -lqqq
	g++ -o mattest6 test/mattest6.cpp -Isrc/ -Llib/ -lqqq
	g++ -o mattest7 test/mattest7.cpp -Isrc/ -Llib/ -lqqq
	g++ -o mattest8 test/mattest8.cpp -Isrc/ -Llib/ -lqqq
	g++ -o mattest9 test/mattest9.cpp -Isrc/ -Llib/ -lqqq

clean:
	 rm -f lib/libqqq.a  *~ src/*.o mattest?
