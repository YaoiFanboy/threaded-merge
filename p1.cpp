
// Command to compile: g++ -o p1 p1.cpp -lpthread
// Command to run: ./p1

#include <iostream>
#include <pthread.h>
#include <assert.h>
#include <stdlib.h>
#include <vector>
// time measurement
#include <chrono>
using namespace std;

// 50 million Arr Size
#define ArrSize 50000000
#define thread_size 2
int *array;
int temp_val = 0;

void Merge(int start, int middle, int end)
{
    // temp array maken voor gesoorterde arrays.
    int *newArr = new int[middle - start + 1];
    int *newArr2 = new int[end - middle];
    int temp1 = middle - start + 1;
    int temp2 = end - middle;
    int k = start;
    //copy beide gesorrteerde arrays naar de temp array
    for (int i = 0; i < temp1; i++)
    {
        newArr[i] = array[i + start];
    }
    for (int i = 0; i < temp2; i++)
    {
        newArr2[i] = array[i + middle + 1];
    }

    //  mergen van de 2 sorted arrays in de orginele array
    int i = 0;
    int j = 0;
    //copy data naar de array van de temp array in gesorrteerde volgorde
    while (i < temp1 && j < temp2)
    {
        if (newArr[i] <= newArr2[j])
        {
            array[k++] = newArr[i++];
        }
        else
        {
            array[k++] = newArr2[j++];
        }
    }

    // copy rest elementen van newarray als die er zijn
    while (i < temp1)
    {
        array[k++] = newArr[i++];
    }

    // copy rest elementen van newarray2 als die er zijn
    while (j < temp2)
    {
        array[k++] = newArr2[j++];
    }
    // deleten memory
    delete[] newArr;
    delete[] newArr2;
}

void mergeSort(int start, int end)
{
    int middle = start + (end - start) / 2;
    if (start < end)
    {
        mergeSort(start, middle);
        mergeSort(middle + 1, end);
        Merge(start, middle, end);
    }
}

void *Sorting_Threading(void *arg)
{
    //berekenen start en end index van array van de thread
    int set_val = temp_val++;
    int start = set_val * (ArrSize / thread_size);
    int end = (set_val + 1) * (ArrSize / thread_size) - 1;
    int middle = start + (end - start) / 2;
    //merge soort func aanroepen
    if (start < end)
    {
        mergeSort(start, middle);
        mergeSort(middle + 1, end);
        Merge(start, middle, end);
    }
    // thread exiten
    pthread_exit(NULL);
}

//---------------------------------------------------------------------------
// kijk of array gesorteerd is of niet
// input: array, size
// return: 1 if sorted, 0 if not sorted
//---------------------------------------------------------------------------
int CheckSorted(int *array, int size1)
{
    // Array loopen en order checken , als array[n] > array[n+1] return 0
    for (int i = 0; i < size1 - 1; i++)
        if (array[i] > array[i + 1])
            return 0;
    return 1;
}

//---------------------------------------------------------------------------
// Main function
// input: arguments count(args), array of command line arguments(argv)
// return: 0 success, error if assertion failed
//---------------------------------------------------------------------------
int main(int args, char **argv)
{
    // Initialize the random seed
    srand(time(NULL));
    // memory allocaten want anders kan het mis gaan
    array = new int[ArrSize];
    // array met random values
    for (int i = 0; i < ArrSize; i++)
    {
        array[i] = rand() % 100;
    }

    // timer begin (chrono shit)
    auto start = chrono::high_resolution_clock::now();
    pthread_t threads[thread_size];
    // threads maken
    for (int i = 0; i < thread_size; i++)
    {
        pthread_create(&threads[i], NULL, Sorting_Threading, NULL);
    }
    // wachten threads klaar zijn
    for (int i = 0; i < thread_size; i++)
    {
        pthread_join(threads[i], NULL);
    }

    // array combineren
    // verdelen array in thread stukken
    vector<int> division;
    vector<int> vect2;
    //pushen van start index van de(het?) array naar div vector
    for (int i = 0; i < thread_size + 1; i++)
    {
        division.push_back(i * (ArrSize / thread_size));
    }

    // hier stopt timer

    // array mergen
    while (division.size() > 2)
    {
        for (int i = 0; i < division.size() - 2; i += 2)
        {
            Merge(division[i], division[i + 1] - 1, division[i + 2] - 1);
            vect2.push_back(division[i]);
        }
        vect2.push_back(division[division.size() - 1]);
        // div vector clearen
        division.clear();
        // copy vect2 naar div copy
        division = vect2;
        vect2.clear();
    }
    auto end = chrono::high_resolution_clock::now();


    // Check for correctness of the result
    assert(1 == CheckSorted(array, ArrSize));

    // memory deleten ( rare errors anders)
    delete[] array;

    cout << "The array is sorted, Assertion Passed, time taken " <<  (double)((chrono::duration_cast<chrono::milliseconds>(end - start).count())/1000.0) << " Seconds" << endl;
    return 0;
}