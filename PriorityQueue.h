#pragma once
#include "TreeNode.h"


//Any class that implements this Data Structure must define the comparison operators 

template<typename T>
class PriorityQueue
{
private:
	TreeNode<T>* arr;
	int queueSize;
	const int maxElements;

	void swap(int i1, int i2); //swaps two nodes based on index
	int leftIndex(int i); //returns index of left child
	int rightIndex(int i); //returns index of right child
	int parentIndex(int i); //returns index of parent
	
	void heapifyUp(int i); //checks a node towards up
	void heapifyDown(int i); //checks a node towards down
	void buildMaxHeap();
	
	
public:
	PriorityQueue();  //initializes array size to 100 by default
	PriorityQueue(int n); //initializes array size to n
	PriorityQueue(TreeNode<T>* A, int size); //takes already existing array
	

	bool isEmpty() const;
	bool insert(const T& p, int priority); //inserts a new node in tree

	bool peekMax(T& max) const;
	bool dequeueMax(T& max);

	int size() const;
	const T* toArray(int& count) const;

	
	void printArr(); //used for testing purposes only;

	

	
};

template <typename T>
PriorityQueue<T>::PriorityQueue() : PriorityQueue(100)
{
	//initializes the queue with a default size 100
}



template<typename T>
PriorityQueue<T>::PriorityQueue(int n) : maxElements(n)
{
	arr = new TreeNode<T>[n + 1];
	queueSize = 0;
	 
}


template<typename T>
PriorityQueue<T>::PriorityQueue(TreeNode<T>* A, int size) : PriorityQueue(size - 1) //location at 0 is ignored
{
	queueSize = maxElements;
	arr = A;
	buildMaxHeap();
}

template<typename T>
bool PriorityQueue<T>::isEmpty() const
{
	if (queueSize == 0)
		return true;
	else
		return false;
}

template<typename T>
bool PriorityQueue<T>::insert(const T& t, int priority)
{
	if (queueSize >= maxElements)
		return false;

	TreeNode<T>* newNode = new TreeNode<T>(t, priority);
	arr[++queueSize] = *newNode;
	heapifyUp(queueSize);
	return true;
}

template<typename T>
bool PriorityQueue<T>::peekMax(T& max) const
{
	if (isEmpty())
		return false;
	max = arr[1].getVal();
	return true;
}

template<typename T>
bool PriorityQueue<T>::dequeueMax(T& max)
{
	if (isEmpty())
		return false;
	max = arr[1].getVal();
	arr[1] = arr[queueSize--];
	heapifyDown(1);
	return true;
}

template<typename T>
void PriorityQueue<T>::buildMaxHeap()
{
	for (int i = queueSize/2; i > 0; i--)
	{
		heapifyDown(i);
	}}

template<typename T>
int PriorityQueue<T>::size() const
{
	return queueSize;
}

template<typename T>
const T* PriorityQueue<T>::toArray(int& count) const
{
	count = queueSize;
	T *array = new T[queueSize];
	for (int i = 0; i < queueSize; i++)
	{
		array[i] = arr[i+1].getVal();
	}
	return array;
}

template<typename T>
void PriorityQueue<T>::printArr()
{
	for (int i = 1; i <= queueSize; i++)
	{
		cout << arr[i].getKey() << " ";
	}
	cout << endl;
}


 template<typename T>
 int PriorityQueue<T>::leftIndex(int i)
 {
	 if (2 * i > queueSize)
		 return 0;
	 else
		 return 2 * i;
 }

 template<typename T>
 int PriorityQueue<T>::rightIndex(int i)
 {
	 if (2 * i + 1 > queueSize)
		 return 0;
	 else
		 return 2 * i + 1;
 }

 template<typename T>
 int PriorityQueue<T>::parentIndex(int i)
 {
	 return i/2;
 }

 template<typename T>
 void PriorityQueue<T>::heapifyUp(int i)
 {
	 while (parentIndex(i) > 0 && arr[i] > arr[parentIndex(i)])
	 {
		 swap(i, parentIndex(i));
		 i = parentIndex(i);
	 }
 }

 template<typename T>
 void PriorityQueue<T>::heapifyDown(int i)
 {
	 int largestIndex = i;
	 if (leftIndex(i) != 0 && arr[leftIndex(i)] > arr[largestIndex])
	 {
		 largestIndex = leftIndex(i);
	 }
	 if (rightIndex(i) != 0 && arr[rightIndex(i)] > arr[largestIndex])
	 {
		 largestIndex = rightIndex(i);
	 }

	 if (i == largestIndex)
		 return;

	 swap(i, largestIndex);
	 heapifyDown(largestIndex);

 }

 template<typename T>
 void PriorityQueue<T>::swap(int i1, int i2)
 {
	 if (i1 == i2)
		 return;
	 TreeNode<T> temp = arr[i1];
	 arr[i1] = arr[i2];
	 arr[i2] = temp;
 }
