#include <iostream>
#include <thread>
#include <vector>
#include <cstdlib>
#include <map>
#include <array>
#include <iostream>
#include <omp.h>
#include <chrono>

#define ARRAY_SIZE 100000

struct Segment{
    std::pair<int, int> lessSegment;
    std::pair<int, int> moreSegment;
};

Segment sort_(std::array<int, ARRAY_SIZE>& list, int pivotPosition, std::pair<int, int> partition){
    std::vector<int> less;
    std::vector<int> more;
    Segment segment;
    int pivotElement = list.at(pivotPosition);
    
    for (int i = partition.first; i <= partition.second; i++){
        if (i == pivotPosition){
            continue;
        }
        else if (list.at(i) <= pivotElement){
            less.push_back(list.at(i));
        }
        else if (list.at(i) > pivotElement){
            more.push_back(list.at(i));
        }
    }

    for (int i = 0; i < less.size(); i++){
        list[i + partition.first] = less[i];
    }
    list[partition.first + less.size()] = pivotElement;
    for (int i = 0; i < more.size(); i++){
        list[i + partition.first + less.size() + 1] = more[i];
    }

    if (less.size() == 0){
        segment.lessSegment.first = -1;
        segment.lessSegment.second = -1;
    }
    else{
        segment.lessSegment.first = partition.first;
        segment.lessSegment.second = partition.first + less.size() - 1;
    }

    if (more.size() == 0){
        segment.moreSegment.first = -1;
        segment.moreSegment.second = -1;
    }
    else{
        segment.moreSegment.first = partition.first + less.size() + 1;
        segment.moreSegment.second = partition.second;
    }
    return segment;


};

int main(){
    std::array<int, ARRAY_SIZE> list{5, 6, 3, 0, 4 ,1, 9, 5, 6, 8};
    
    std::cout << "Original list" << std::endl;
    for (int i = 0; i < ARRAY_SIZE; i++){
        list[i] =  rand() % 10;
        std::cout << list[i] << ' ';
    }
    std::cout << std::endl;

    auto t0 = std::chrono::high_resolution_clock::now();

    std::vector<std::pair<int, int>> partitions;
    std::pair<int, int> startPartition{0, ARRAY_SIZE - 1};
    partitions.push_back(startPartition);

    std::vector<std::pair<int, int>> partitionsTmp;
    Segment seg;

    int random;
    bool isSorted = false;
    int singlesQuantity = 0;
    while (!isSorted){

#pragma omp parallel for shared(partitions, partitionsTmp, singlesQuantity)
        for (auto segment: partitions){

            if (segment.first == segment.second){
                partitionsTmp.push_back(segment);
                singlesQuantity++;
                continue;
            }

            random = rand() % (segment.second - segment.first + 1) + segment.first;
                
            seg = sort_(list, random, segment);

            if (seg.lessSegment.first != -1){
                partitionsTmp.push_back(seg.lessSegment);
            }
            if (seg.moreSegment.second != -1){
                partitionsTmp.push_back(seg.moreSegment);
            }
        }

        if (singlesQuantity == partitionsTmp.size()){
            isSorted = true;
        }
        singlesQuantity = 0;

        partitions.clear();
        partitions = partitionsTmp;
        partitionsTmp.clear();
        
    }

    auto t1 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> fs = t1 - t0;
    std::chrono::milliseconds d = std::chrono::duration_cast<std::chrono::milliseconds>(fs);

    std::cout << "Sorted list" << std::endl;
    for (auto element: list){
        std::cout << element << ' ';
    }
    std::cout << std::endl;

    std::cout << "Elapsed time: " << d.count() << "ms\n";


    
}