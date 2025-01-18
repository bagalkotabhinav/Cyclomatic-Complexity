#include <stdio.h>
int linearSearch(int arr[], int n, int key) {
    for (int i = 0; i < n; i++) {
        if (arr[i] == key) {
            return i; // Return index if key is found
        }
    }
    return -1; // Key not found
}

int binarySearch(int arr[], int low, int high, int key) {
    while (low <= high) {
        int mid = low + (high - low) / 2; // Avoids overflow
        if (arr[mid] == key) {
            return mid; // Key found
        }
        if (arr[mid] < key) {
            low = mid + 1; // Search in the right half
        } else {
            high = mid - 1; // Search in the left half
        }
    }
    return -1; // Key not found
}

int main() {
    int a,b;
    int arr[5]={9,2,5,0,4};
    a=linearSearch(arr, 5, 0);
    b=binarySearch(arr,0,4,0);
    return 0;
}
