#include <mpi.h>
#include <vector>
#include <iostream>
using namespace std;

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv); // Инициализация MPI

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Определяем ранг процесса
    MPI_Comm_size(MPI_COMM_WORLD, &size); // Определяем общее количество процессов

    // Размер матрицы (например, N = 8)
    int N = 8;
    vector<double> global_result; // Итоговый массив для данных от всех процессов
    vector<double> local_result(N / size); // Массив для данных локального процесса

    // Генерация данных локально (здесь пример с фиктивными данными)
    for (int i = 0; i < N / size; ++i) {
        local_result[i] = rank + i; // Пример: ранг + индекс
    }

    if (rank == 0) {
        // Процесс 0 должен иметь массив для хранения всех данных
        global_result.resize(N);
    }

    // Сбор данных от всех процессов
    MPI_Gather(local_result.data(), local_result.size(), MPI_DOUBLE,
               global_result.data(), local_result.size(), MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Печать результатов на процессе 0
    if (rank == 0) {
        cout << "Собранные данные:" << endl;
        for (double val : global_result) {
            cout << val << " ";
        }
        cout << endl;
    }

    MPI_Finalize(); // Завершение MPI
    return 0;
}
