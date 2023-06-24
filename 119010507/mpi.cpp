#include "asg2.h"
#include <stdio.h>
#include <mpi.h>
#include <cstddef>

static int rank;
static int world_size;

void ParallelCompute(Point *my_elements, const int num_of_elements) {
	size_t index = 0;
	while (index < num_of_elements) {
		compute(my_elements);
		my_elements++;
		index++;
	}
}

int main(int argc, char *argv[]) {
	if ( argc == 4 ) {
		X_RESN = atoi(argv[1]);
		Y_RESN = atoi(argv[2]);
		max_iteration = atoi(argv[3]);
	} else {
		X_RESN = 1000;
		Y_RESN = 1000;
		max_iteration = 100;
	}

	if (rank == 0) {
		#ifdef GUI
		glutInit(&argc, argv);
		glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
		glutInitWindowSize(500, 500); 
		glutInitWindowPosition(0, 0);
		glutCreateWindow("MPI");
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glMatrixMode(GL_PROJECTION);
		gluOrtho2D(0, X_RESN, 0, Y_RESN);
		glutDisplayFunc(plot);
		#endif
	}

	/* computation part begin */
	MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	
	total_size = X_RESN * Y_RESN;

	if (rank == 0){
		initData();
	}

	const int num_fields = 3;
	MPI_Aint mpi_disps[num_fields];		
	MPI_Aint start_address;
	MPI_Aint address;
	MPI_Datatype types[] = {MPI_INT, MPI_INT, MPI_FLOAT};

	const int blocklens[] = {1, 1, 1};
	mpi_disps[0] = offsetof(Point, x);
	mpi_disps[1] = offsetof(Point, y);
	mpi_disps[2] = offsetof(Point, color);

	MPI_Datatype MPI_POINT;
	MPI_Type_create_struct(num_fields, blocklens, mpi_disps, types, &MPI_POINT);
	MPI_Type_commit(&MPI_POINT);

    int *displace_array = new int[world_size]; 
    int *tally_array = new int[world_size]; 


    int remainder = total_size % world_size; 

	int localNumber;
	if (rank < remainder){
		localNumber = total_size / world_size + 1;
	} else{
		localNumber = (total_size / world_size);
	}
	Point *my_elements = new Point[localNumber];


    int store; 
	size_t i = 0;
    while (i < world_size) {
		if (i < remainder){
			displace_array[i] = total_size / world_size + 1;
		} else {
			displace_array[i] = total_size / world_size;
		}
        tally_array[i] = store;
        store += displace_array[i]; 
		i++;
    }

	MPI_Barrier(MPI_COMM_WORLD);
	if (rank == 0) t1 = std::chrono::high_resolution_clock::now();

	MPI_Scatterv(data, displace_array, tally_array, MPI_POINT, my_elements, localNumber, MPI_POINT, 0, MPI_COMM_WORLD);

	/* All processes compute for the Madelbrot Set */
	ParallelCompute(my_elements, localNumber);
    MPI_Gatherv(my_elements, localNumber, MPI_POINT, data, displace_array, tally_array, MPI_POINT, 0, MPI_COMM_WORLD); 

	if (rank == 0){
		t2 = std::chrono::high_resolution_clock::now();  
		time_span = t2 - t1;
	}

	MPI_Barrier(MPI_COMM_WORLD);

	if (rank == 0){
		printf("Student ID: 119010507\n"); // replace it with your student id
		printf("Name: Ryan Christopher\n"); // replace it with your name
		printf("Assignment 2 MPI\n");
		printf("Run Time: %f seconds\n", time_span.count());
		printf("Problem Size: %d * %d, %d\n", X_RESN, Y_RESN, max_iteration);
		printf("Process Number: %d\n", world_size);
	}

	delete[] my_elements;
	delete[] displace_array;
	delete[] tally_array;
	
	/* computation part end */

	if (rank == 0){
		#ifdef GUI
		glutMainLoop();
		#endif
	}

	delete[] data;
	MPI_Finalize();
	return 0;
}

