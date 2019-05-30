#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <pthread.h>
#include <iostream>

#define ERROR_CREATE_THREAD -11
#define ERROR_JOIN_THREAD   -12
#define BAD_MESSAGE         -13
#define SUCCESS               0
 
typedef struct someArgs_tag {
    int size;
    int * array;
} someArgs_t;

void getHalf(int * full_array, int * half_array, int start, int finish) {
    int i, j;
    for(i = start, j = 0; i < finish; i++, j++) {
        half_array[j] = full_array[i];
    }
}

void getRandom(int size, int * arr) {
    srand(time(NULL));
    for (int i = 0; i < size; i++)
        arr[i] = rand() % 10000;
}

void* sort_array(void *args) {
    someArgs_t *arg = (someArgs_t*) args;
    int i = 0;
	int buf;
	int array_size = arg -> size;
	char swap_cnt = 0;
	while (i < array_size)
	{
		if (i + 1 != array_size && arg -> array[i] > arg -> array[i + 1]) {
			buf = arg -> array[i];
			arg -> array[i] = arg -> array[i + 1];
			arg -> array[i + 1] = buf;
			swap_cnt = 1;
		}
		i++;
		if (i == array_size && swap_cnt == 1)
		{
			swap_cnt = 0;
			i = 0;
		}
	}
    std::cout << '\n';
    return SUCCESS;
}

void merge(int * array, int * half_array1, int * half_array2, int size, int half_size1, int half_size2) {
    int iter = 0;
    int i = 0, j = 0;
    while(size) {
        if(i < half_size1 && j < half_size2) {
            if(half_array1[i] < half_array2[j]) {
                array[iter++] = half_array1[i++];
            } else {
                array[iter++] = half_array2[j++];
            }
        } else {
            if(i >= half_size1) {
                array[iter++] = half_array2[j++];
            } else {
                array[iter++] = half_array1[i++];                
            }
        }
        size--;
    }
}

#define NUM_THREADS 2
 
int main() {
    int size;
    std::cout << "enter size: \n";
    std::cin >> size;
    int medium = size / 2, i;
    int arr[100000], half_array1[100000], half_array2[100000];
    getRandom(size, arr);
    std::cout << "unsorted array: \n";
    for(int i = 0; i < size; i++) {
        std::cout << arr[i] << ' ';
    }
    getHalf(arr, half_array1, 0, medium);
    getHalf(arr, half_array2, medium, size);
    pthread_t threads[NUM_THREADS];
    int status;
    int status_addr;
    someArgs_t args[NUM_THREADS];
    args[0].array = half_array1;
    args[1].array = half_array2;
    args[0].size = size / 2;
    args[1].size = size - size / 2;
    for (i = 0; i < NUM_THREADS; i++) {
        status = pthread_create(&threads[i], NULL, sort_array, (void*) &args[i]);
        if (status != 0) {
            printf("main error: can't create thread, status = %d\n", status);
            exit(ERROR_CREATE_THREAD);
        }
    }
    for (i = 0; i < NUM_THREADS; i++) {
        status = pthread_join(threads[i], (void**)&status_addr);
        if (status != SUCCESS) {
            printf("main error: can't join thread, status = %d\n", status);
            exit(ERROR_JOIN_THREAD);
        }
    }
    int result[100000];
    int half_size1, half_size2;
    half_size1 = size / 2;
    half_size2 = size - half_size1;
    merge(result, args[0].array, args[1].array, size, half_size1, half_size2);
    std::cout << "sorted array: \n";
    for(i = 0; i < size; i++) {
        std::cout << result[i] << ' ';
    }
    return 0;
}
