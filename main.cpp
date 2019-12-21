#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "sort.h"

int quickSort(){
    int arr[] = {4,7,6,5,3,2,8,1};
    int len = sizeof(arr)/ sizeof(int);
    QuickSort(arr,0,len-1 );

    for (int i = 0; i < len-1; ++i) {
        std::cout<<arr[i];
    }


    std::cout<<std::endl;

    int arr2[] = {4,7,6,5,3,2,8,1};
    int len2 = sizeof(arr2)/ sizeof(int);
    QuickSortWithStack(arr2,0,len2-1 );

    for (int i = 0; i < len2-1; ++i) {
        std::cout<<arr2[i];
    }
}


void merge() {
    int arr[] = {4,7,6,5,3,2,8,1};
    int len = sizeof(arr)/ sizeof(int);
    mergeSort(arr,0,len-1);
    for (int i = 0; i < len; ++i) {
        std::cout<<arr[i];
    }
}

int main()
{
    merge();
}