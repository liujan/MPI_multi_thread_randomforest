#include "iostream"
#include "fstream"
#include "math.h"
#include "stdio.h"
#include "string"
#include "RandomForest.h"
#include "stdlib.h"
#include "RandomForest.h"
#include "mpi.h"
using namespace std;

extern const int numOfTypes; //类别数
extern const int numOfFeatures; 
extern const int numOfSample;
extern const int numOfTest;

struct Sample{
    int result[numOfTest][numOfTypes+1];
};

void new_type(MPI_Datatype *ctype) {
    struct Sample sample;

    int blockcounts[2];
    MPI_Datatype oldtypes[2];

    oldtypes[0] = MPI_INT;
    oldtypes[1] = MPI_DOUBLE;

    MPI_Type_vector(numOfTest, numOfTypes+1, numOfTypes + 1, MPI_INT, ctype);
    MPI_Type_commit(ctype);
}

int main(int argc,  char * argv[]) {
	// insert code here...
	std::cout << "Hello, World!\n";

	int mypid, size, namelen;
    char name[MPI_MAX_PROCESSOR_NAME];
    MPI_Status status;
    MPI_Init(&argc, &argv);

    MPI_Datatype ctype;

    new_type(&ctype);

    MPI_Comm_rank(MPI_COMM_WORLD, &mypid); 
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Get_processor_name(name, &namelen);
    cout << mypid << " of process " << size << " from " << name << endl;   


    if (mypid == 0){
        double result[numOfTest][numOfTypes+1];
        memset(result, 0, sizeof(result));
        ofstream out("result.csv");
        out << "Id,label" << "\n";

        for (int i = 1; i < size; i++) {
            struct Sample *sample = new Sample();
            MPI_Recv(sample, 1, ctype, i, 1, MPI_COMM_WORLD, &status);

            for (int j = 0; j < numOfTest; j++){
                for (int k = 0; k <= numOfTypes; k++){
                    result[j][k] += sample->result[j][k];
                }
            }
            delete sample;
        }
        cout << "writing to file result.csv of " << name << " ..."<< endl;
        for (int i = 0; i < numOfTest; i++){
            int max_value = -10, index = 0;
            for (int j = 0; j <= numOfTypes; j++){
                if (result[i][j] >= max_value){
                    max_value = result[i][j];
                    index = j;
                }
            }
            out << i << "," << index << "\n";
        }
        cout << "writing over" << endl;
        out.close();

        int *tmp = new int;
        *tmp = 23;
        for (int i = 1; i < size; i++)
            MPI_Send(tmp, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
    }
    else{
        struct Sample *sample = new Sample();

        RandomForest forest;
        forest.readTrainSample("train.csv");
		forest.readTestSample("test.csv");
	    forest.buildTree(1200);
	    int **result = forest.predict();

        for (int i = 0; i < numOfTest; i++) {
            for (int j = 0; j <= numOfTypes; j++){
                sample->result[i][j] = result[i][j];
            }
        }
        
        MPI_Send(sample, 1, ctype, 0, 1, MPI_COMM_WORLD);
        int *tmp = new int;
        MPI_Recv(tmp, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        
    }


	
	return 0;
}
