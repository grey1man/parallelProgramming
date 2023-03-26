#include <set>
#include <vector>
#include <iostream>
#include <omp.h>
#include <chrono>
#include <bitset>

#define SIZE_OF_SET 29

long long unsigned int factorial(int value){
    long long unsigned int answ = 1;
    for (int i = 1; i <= value; i++){
        answ *= i;
    }
    return answ;
}

int main(){
    std::set<int> setOfValues;

    int randElement;

    
    for (int i = 0; i < SIZE_OF_SET; i++){
        randElement = rand() % (SIZE_OF_SET + SIZE_OF_SET + 1) - SIZE_OF_SET;
        if (randElement == 0){
            randElement = 1;
        }
        setOfValues.insert(randElement);
        
    }
    
    std::cout << "Set: ";
    for (auto element: setOfValues){
        std::cout << element << " ";
    }
    std::cout << std::endl;

    long long unsigned int combinationQuantity = 0;
    for (int i = 1; i < setOfValues.size(); i++){
        combinationQuantity += factorial(setOfValues.size()) / (factorial(i) * factorial(setOfValues.size() - i));
    }

    std::bitset<SIZE_OF_SET> setMask;
    long long int tmpValue = 0;
    bool isFoundSolution = false;
    std::vector<int> solution;

    auto t0 = std::chrono::high_resolution_clock::now();

#pragma omp parallel for shared(setOfValues, combinationQuantity, isFoundSolution, solution) private(setMask, tmpValue)
    for (int i = 1; i <= combinationQuantity; i++){
        if (!isFoundSolution){
            setMask = i;
            for (int j = 0; j < setMask.size(); j++){
                if (setMask[j] == 1)
                    tmpValue += *(std::next(setOfValues.begin(), j));
            }
            if (tmpValue == 0){
                isFoundSolution = true;
                for (int j = 0; j < setMask.size(); j++)
                    if (setMask[j] == 1)
                        solution.push_back(*(std::next(setOfValues.begin(), j)));
            }
            tmpValue = 0;   
        } 
    }

    auto t1 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> fs = t1 - t0;
    std::chrono::milliseconds d = std::chrono::duration_cast<std::chrono::milliseconds>(fs);

    std::cout << "Solution is: ";
    for (auto element: solution)
        std::cout << element << " ";
    
    std::cout << std::endl;
    std::cout << "Elapsed time: " << d.count() << "ms\n";
}