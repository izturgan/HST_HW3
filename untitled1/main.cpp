#include <mpi.h>
#include <vector>
#include <iostream>
#include <fstream>
using namespace std;

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc < 2) {
        if (rank == 0) {
            cerr << "Ошибка: необходимо указать имя входного файла." << endl;
        }
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    string inputFileName = argv[1]; // Имя входного файла передаётся как аргумент
    int N = 0;
    vector<double> global_data;
    vector<double> global_result;
    vector<double> local_result;

    if (rank == 0) {
        ifstream inputFile(inputFileName);
        if (!inputFile.is_open()) {
            cerr << "Ошибка: не удалось открыть файл " << inputFileName << "." << endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        inputFile >> N;
        global_data.resize(N);
        for (int i = 0; i < N; ++i) {
            inputFile >> global_data[i];
        }
        inputFile.close();

        if (N % size != 0) {
            cerr << "Ошибка: размер данных не делится на количество процессов." << endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        global_result.resize(N);
    }

    MPI_Bcast(&N, 1, MPI_INT, 0);

    local_result.resize(N / size);
    MPI_Scatter(global_data.data(), N / size, MPI_DOUBLE,
                local_result.data(), N / size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    for (double& val : local_result) {
        val += rank;
    }

    MPI_Gather(local_result.data(), N / size, MPI_DOUBLE,
               global_result.data(), N / size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        ofstream outputFile("result.txt");
        if (!outputFile.is_open()) {
            cerr << "Ошибка: не удалось открыть файл result.txt для записи." << endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        for (double val : global_result) {
            outputFile << val << " ";
        }
        outputFile << endl;
        outputFile.close();
    }

    MPI_Finalize();
    return 0;
}
