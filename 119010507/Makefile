all: seq seqg mpi mpig pthread pthreadg

seq: sequential.cpp
	g++ sequential.cpp -o seq -O2 -std=c++11

seqg: sequential.cpp
	g++ sequential.cpp -o seqg -I/usr/include -L/usr/local/lib -L/usr/lib -lglut -lGLU -lGL -lm -DGUI -O2 -std=c++11

mpi: mpi.cpp
	mpic++ mpi.cpp -o mpi -std=c++11

mpig: mpi.cpp
	mpic++ mpi.cpp -o mpig -I/usr/include -L/usr/local/lib -L/usr/lib -lglut -lGLU -lGL -lm -DGUI -std=c++11

pthread: pthread.cpp
	g++ pthread.cpp -o pthread -lpthread -O2 -std=c++11

pthreadg: pthread.cpp
	g++ pthread.cpp  -o pthreadg -I/usr/include -L/usr/local/lib -L/usr/lib -lglut -lGLU -lGL -lm -lpthread -DGUI -O2 -std=c++11

clean:
	rm -f seq seqg mpi mpig pthread pthreadg