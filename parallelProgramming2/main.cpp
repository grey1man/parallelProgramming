#include <iostream>

#include <mpi.h>

#define FIRST_THREAD 0

bool check(int i, int n, int* a){
    int sum = 0;
    for(int j = 0; j < n; j++){
            if(i & (1 << j)){
                sum += a[j];
            }
        }
    if (sum == 0)
        return true;
    return false;
}

int main(int argc, char **argv){
    int a[] = {1, 2, 3, -5}; 
    int n = sizeof(a) / sizeof(a[0]);

    int crutch = 0;

    bool isFound = false;

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

    int buff[thread_quantity - 1];
    int buffFullness = 0;

    bool isEnd = false;
    int answThread = -1;
    if (thread == FIRST_THREAD){
        for(int i = (1 << n) - 1; i >=0; i--){

            if (isEnd){
                break;
            }

            buff[buffFullness] = i;
            buffFullness += 1;

            if (buffFullness == thread_quantity - 1){
                for (int threadNum = 1; threadNum < thread_quantity; threadNum++)
                    MPI_Send(&buff[threadNum - 1], 1, MPI_INT, threadNum, 0, MPI_COMM_WORLD);
                
                for (int threadNum = 1; threadNum < thread_quantity; threadNum++){
                    MPI_Recv(&isFound, 1, MPI_CXX_BOOL, threadNum, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
                    if (isFound){
                        isEnd = isFound;
                        answThread = threadNum;
                    }    
                }
                buffFullness = 0;
            }
        }

    for (int threadNum = 1; threadNum <= buffFullness; threadNum++)
        MPI_Send(&buff[threadNum - 1], 1, MPI_INT, threadNum + 1, 0, MPI_COMM_WORLD);
    
    for (int threadNum = 1; threadNum <= buffFullness; threadNum++){
        MPI_Recv(&isFound, 1, MPI_CXX_BOOL, threadNum, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        if (isFound){
            answThread = threadNum;
            break;
        }
    }

    //must think more about that
    for (int threadNum = 1; threadNum < thread_quantity; threadNum++){
        if (answThread == threadNum)
            continue;
        MPI_Send(&crutch, 1, MPI_INT, threadNum, 0, MPI_COMM_WORLD);
    }
    
    if (isFound){
        if (thread == FIRST_THREAD){
        int answ;
        MPI_Recv(&answ, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        std::cout << "answer is ";
        for(int j = 0; j < n; j++){
                if(answ & (1 << j)){
                    std::cout << a[j] << " ";
                }
            }
        }
    }
    else{
        std::cout << "No such subset" << std::endl;
    }
    }
    else{
        int numberForChecking;
        while (!isFound){
            MPI_Recv(&numberForChecking, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

            if (numberForChecking == 0)
                break;

            isFound = check(numberForChecking, n, a);
            MPI_Send(&isFound, 1, MPI_CXX_BOOL, FIRST_THREAD, 0, MPI_COMM_WORLD);
        }

        if (isFound)
            MPI_Send(&numberForChecking, 1, MPI_INT, FIRST_THREAD, 0, MPI_COMM_WORLD);
    }

    

    MPI_Finalize();
	return 0;
}

// for(int j = 0; j < n; j++){
//             if(i & (1 << j)){
//                 std::cout << a[j] << " ";
//             }
//         }