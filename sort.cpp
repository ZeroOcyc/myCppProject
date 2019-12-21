//
// Created by Administrator on 2019/12/20.
//
#include <stack>
#include <memory>
#include "sort.h"

int partition(int arr[], int start, int end);

int QuickSort(int arr[],int start, int end) {
    if (arr == nullptr) {
        return 0;
    }

    if (start >= end) {
        return 0;
    }

    int index = partition(arr,start,end);
    QuickSort(arr,start,index-1);
    QuickSort(arr,index+1,end);
}

typedef struct {
    int start;
    int end;
}info;

int QuickSortWithStack(int arr[],int start,int end) {
    if (arr == nullptr) {
        return 0;
    }

    std::stack<info> stack;

    info node;
    node.start = start;
    node.end = end;
    stack.push(node);

    while (!stack.empty()) {
        info cur = stack.top();
        stack.pop();

        if (cur.start>=cur.end) {
            break;
        }

        int index = partition(arr,cur.start,cur.end);

        if (cur.start < index){
            info newNode;
            newNode.start = cur.start;
            newNode.end = index-1;
            stack.push(newNode);
        }

        if (cur.end > index ){
            info newNode;
            newNode.start = index+1;
            newNode.end = cur.end;
            stack.push(newNode);
        }
    }

}

int partition(int arr[], int start, int end) {
    int pivot = arr[start];
    int left = start;
    int right = end;

    while (left != right) {
        while (arr[right] < pivot && left < right){
            right--;
        }
        while (arr[left] >= pivot && left < right){
            left++;
        }
        if (left < right) {
            int p = arr[left];
            arr[left] = arr[right];
            arr[right] = p;
        }
    }

    arr[start] = arr[left];
    arr[left] = pivot;
    return left;
}


// ==========merge==========

void merge(int arr[],int start,int mid,int end){
    int p1 = start;
    int p2 = mid+1;
    int p = 0;



    std::shared_ptr<int[]> tmpArr (new int[end-start+1]);

    while (p1 <= mid && p2 <= end) {
        if (arr[p1] <= arr[p2]) {
            tmpArr[p++] = arr[p1++];
        } else {
            tmpArr[p++] = arr[p2++];
        }
    }

    while (p1 <= mid) {
        tmpArr[p++] = arr[p1++];
    }
    while (p2 <= end) {
        tmpArr[p++] = arr[p2++];
    }


    for (int i = 0; i < (end - start + 1); ++i) {
        arr[start+i] = tmpArr[i];
    }
}

int mergeSort(int arr[],int start,int end) {
    if (arr == nullptr) {
        return  0;
    }

    if (start < end) {
        int mid = (start+end)/2;
        mergeSort(arr,start,mid);
        mergeSort(arr,mid+1,end);
        merge(arr,start,mid,end);
    }
}