#include <mpi.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <numeric>
#include <chrono>

using namespace std;
using namespace chrono;

// Функция для вычисления среднего по строке
double calculateRowAverage(const vector<int>& row) {
    double sum = accumulate(row.begin(), row.end(), 0.0);
    return sum / row.size();
}

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    vector<vector<int>> matrix;
    int rows, cols;

    if (rank == 0) {
        // Главный процесс читает данные
        ifstream inputFile("matrix_input.txt");
        if (!inputFile.is_open()) {
            cerr << "Ошибка: не удалось открыть файл для чтения данных." << endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        inputFile >> rows >> cols;
        matrix.resize(rows, vector<int>(cols));
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                inputFile >> matrix[i][j];
            }
        }
        inputFile.close();
    }

    // Передача размера матрицы
    MPI_Bcast(&rows, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&cols, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Делим матрицу между процессами
    int rowsPerProcess = rows / size;
    int extraRows = rows % size;
    int startRow = rank * rowsPerProcess + min(rank, extraRows);
    int endRow = startRow + rowsPerProcess + (rank < extraRows ? 1 : 0);

    vector<vector<int>> localMatrix(endRow - startRow, vector<int>(cols));
    if (rank == 0) {
        for (int i = 0; i < size; ++i) {
            int start = i * rowsPerProcess + min(i, extraRows);
            int end = start + rowsPerProcess + (i < extraRows ? 1 : 0);
            if (i == 0) {
                localMatrix = vector<vector<int>>(matrix.begin() + start, matrix.begin() + end);
            } else {
                MPI_Send(matrix[start].data(), (end - start) * cols, MPI_INT, i, 0, MPI_COMM_WORLD);
            }
        }
    } else {
        MPI_Recv(localMatrix[0].data(), localMatrix.size() * cols, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    // Вычисление среднего для каждой строки
    auto start = high_resolution_clock::now();

    vector<double> localAverages(localMatrix.size());
    for (int i = 0; i < localMatrix.size(); ++i) {
        localAverages[i] = calculateRowAverage(localMatrix[i]);
    }

    auto end = high_resolution_clock::now();
    duration<double> elapsed = end - start;

    // Сбор данных
    vector<double> averages;
    if (rank == 0) {
        averages.resize(rows);
    }

    MPI_Gather(localAverages.data(), localAverages.size(), MPI_DOUBLE,
               averages.data(), localAverages.size(), MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        ofstream outFile("averages.txt");
        if (outFile.is_open()) {
            for (double avg : averages) {
                outFile << avg << endl;
            }
            outFile << "\n--- Информация о вычислениях ---" << endl;
            outFile << "Размер матрицы: " << rows << "x" << cols << endl;
            outFile << "Время выполнения: " << elapsed.count() << " секунд" << endl;

            outFile.close();
        } else {
            cerr << "Ошибка записи в файл." << endl;
        }
    }

    MPI_Finalize();
    return 0;
}
