#include <iostream>
#include <chrono>

#include <mpi.h>

int main(int argc, char **argv){
    int a[] = {1, 2, 3, 32, 6, 7, 8, 1, 2, 4, 5, 6, 7, 8, 9, 10, 11, 2, 35, 9, 1523, 221, 25321, 87, 45, 56, 7, 8, 123, -5}; 
    int n = sizeof(a) / sizeof(a[0]);
    int sum = 0;
    bool flag = false;

    MPI_Request request;

    int thread, thread_quantity, processor_name_length;
	char* processor_name = new char[MPI_MAX_PROCESSOR_NAME * sizeof(char)];

    MPI_Status status;

    // Инициализируем работу MPI
	MPI_Init(&argc, &argv);
	
	// Получаем имя физического процессора
	MPI_Get_processor_name(processor_name, &processor_name_length);
	
	// Получаем номер конкретного процесса на котором запущена программа
	MPI_Comm_rank(MPI_COMM_WORLD, &thread);
	
	// Получаем количество запущенных процессов
	MPI_Comm_size(MPI_COMM_WORLD, &thread_quantity);

    auto t0 = std::chrono::high_resolution_clock::now();
    for(int i = (1 << n) - 1 - thread; i >=0; i -= thread_quantity){
        for(int j = 0; j < n; j++){
                if (flag){
                    return 0;
                }
                if(i & (1 << j)){
                    sum += a[j];
                }
            }
        if (sum == 0){
            std::cout << "found" << std::endl;
            auto t1 = std::chrono::high_resolution_clock::now();
            std::chrono::duration<float> fs = t1 - t0;
            std::chrono::milliseconds d = std::chrono::duration_cast<std::chrono::milliseconds>(fs);

            std::cout << "Elapsed time: " << d.count() << "ms\n";

            std::cout << "answer is ";
            for(int j = 0; j < n; j++){
                    if(i & (1 << j)){
                        std::cout << a[j] << " ";
                    }
                }

            MPI_Finalize();
            return 0;
        }
        sum = 0; 
    }

}