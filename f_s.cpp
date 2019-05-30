//#define _GNU_SOURCE 
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <wait.h>
#include <sched.h>
#include <vector>
#include <algorithm>

void getRandom(int size, int * arr) {
    srand(time(NULL));
    for (int i = 0; i < size; i++)
        arr[i] = rand() % 100;
}

void getHalf(int * full_array, int * half_array, int start, int finish) {
    int i, j;
    for(i = start, j = 0; i < finish; i++, j++) {
        half_array[j] = full_array[i];
    }
}

void sort(int * array, int array_size) {
    int i = 0;
	int buf;
	char swap_cnt = 0;
	while (i < array_size)
	{
		if (i + 1 != array_size && array[i] > array[i + 1])
		{
			buf = array[i];
			array[i] = array[i + 1];
			array[i + 1] = buf;
			swap_cnt = 1;
		}
		i++;
		if (i == array_size && swap_cnt == 1)
		{
			swap_cnt = 0;
			i = 0;
		}
	}
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

int main() {
	enum { maxSons = 100 };
    int size, i, medium;
	pid_t id[maxSons];
    int arr[100000], half_array1[100000], half_array2[100000];
    std::cout << "enter size: \n";
    std::cin >> size;
    getRandom(size, arr);
    std::cout << "unsorted array: \n";
    for(int i = 0; i < size; i++) {
        std::cout << arr[i] << ' ';
    }
    std::cout << std::endl;
    int sons = 2;
    int fd[maxSons][2];
    int half_size1 = size / 2;
    int half_size2 = size - half_size1;
    for(i = 0; i < sons; i++) {
        pipe(fd[i]);
        if((id[i] = fork()) == 0) {
            cpu_set_t mask;
			CPU_ZERO(&mask);
			CPU_SET(i % 2, &mask);
			if(sched_setaffinity(0, sizeof(mask), &mask ) == -1) {
				printf("WARNING: Could not set CPU Affinity, continuing...\n");
			}
			int half_arr[100];
            if(i == 0) {
                getHalf(arr, half_arr, 0, half_size1);
            } else {
                getHalf(arr, half_arr, half_size1, size);
            }
            if (i == 0) {
                sort(half_arr, half_size1);
			    printf("SON: %d start\n", getpid());
			    close(fd[i][0]);
			    write(fd[i][1], half_arr, half_size1 * sizeof(int));
                close(fd[i][1]);
            } else {
                sort(half_arr, half_size2);
			    printf("SON: %d start\n", getpid());
			    close(fd[i][0]);
			    write(fd[i][1], half_arr, half_size2 * sizeof(int));
                close(fd[i][1]);
            }
			return 0;
        }
		printf("PARENT: %d start\n", id[i]);
    }
    int half_size = size / 2;
    int half_sorted1[100];
    int half_sorted2[100];
	for (i = 0; i < sons; i++) {
		close(fd[i][1]);
		int st;
		wait4(id[i], NULL, st, NULL);
		if(i == 0) {
		    read(fd[i][0], half_sorted1, half_size1 * sizeof(int));
		} else {
		    read(fd[i][0], half_sorted2, half_size2 * sizeof(int));
		}
		printf("%d get from %d (%d): \n", getpid(), i, id[i]);
		close(fd[i][0]);
	}
    int result[100];
    std::cout << std::endl;
    std::cout << "sorted array: \n";
    merge(result, half_sorted1, half_sorted2, size, half_size1, half_size2);
    for(int i = 0; i < size; i++) {
        std::cout << result[i] << ' ';
    }
    std::cout << '\n';
    return 0;
}
