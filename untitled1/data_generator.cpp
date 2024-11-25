#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace std;

void generateData(int sizeInMB, const string& filename) {
    int rows = sizeInMB * 256; // Пример: 256 строк на 1 МБ
    int cols = 256;           // Каждая строка содержит 256 чисел
    ofstream file(filename);

    if (!file.is_open()) {
        cerr << "Ошибка: не удалось открыть файл для записи данных." << endl;
        return;
    }

    file << rows << " " << cols << endl; // Размер матрицы
    srand(time(0));

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            file << rand() % 100 << " "; // Генерация случайных чисел
        }
        file << endl;
    }

    file.close();
    cout << "Данные успешно сгенерированы в файл " << filename << endl;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Использование: " << argv[0] << " <размер_в_МБ> <имя_файла>" << endl;
        return 1;
    }

    int sizeInMB = stoi(argv[1]);
    string filename = argv[2];
    generateData(sizeInMB, filename);
    return 0;
}
