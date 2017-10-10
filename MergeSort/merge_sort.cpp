#include <iostream>
#include <stdlib.h>

using namespace std;


struct mergeSortData
{
    int *arr;
    int low,high;
};

// A function to merge the two half into a sorted data.
void Merge(int *a, int low, int high, int mid)
{
    // We have low to mid and mid+1 to high already sorted.
    int i, j, k, temp[high-low+1];
    i = low;
    k = 0;
    j = mid + 1;

    // Merge the two parts into temp[].
    while (i <= mid && j <= high)
    {
        if (a[i] < a[j])
            temp[k++] = a[i++];
        else
            temp[k++] = a[j++];
    }

    // Insert all the remaining values from i to mid into temp[].
    while (i <= mid)
        temp[k++] = a[i++];

    // Insert all the remaining values from j to high into temp[].
    while (j <= high)
        temp[k++] = a[j++];

    // Assign sorted data stored in temp[] to a[].
    for (i = low; i <= high; i++)
    {
        a[i] = temp[i-low];
    }
}

// A function to split array into two parts.
void MergeSort(int *a, int low, int high)
{
    int mid;
    if (low < high)
    {
        mid=(low+high)/2;
        // Split the data into two half.
        MergeSort(a, low, mid);
        MergeSort(a, mid+1, high);
        // Merge them to get sorted output.
        Merge(a, low, high, mid);
    }
}

void calllMergeSort(int *a, int low, int high,int n, int numThreads,int granularity)
{
    for(register int i=0; i=n/granularity;i++)
    {
        ;       
    }
    
}


int main(int argc, char* argv[])
{
    int n, i;
    n = atoi(argv[1]);

    int arr[n];
    for(i = 0; i < n; i++)
        arr[i]=rand() % 10000;

    MergeSort(arr, 0, n-1);

    // Printing the sorted data.
    cout<<"\nSorted Data ";
    for (i = 0; i < n; i++)
        cout<<"->"<<arr[i];

    return 0;
}
