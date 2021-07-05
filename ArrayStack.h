#pragma once
//	This is an updated version of code originally
//  created by Frank M. Carrano and Timothy M. Henry.
//  Copyright (c) 2017 Pearson Education, Hoboken, New Jersey.

/** ADT stack: Array-based implementation.
 @file ArrayStack.h */

#ifndef ARRAY_STACK_
#define ARRAY_STACK_

#include "StackADT.h"

 //Unless spesificed by the stack user, the default size is 100
template<typename T>
class ArrayStack : public StackADT<T>
{
private:
	T* items;		// Array of stack items
	int top;                   // Index to top of stack
	const int STACK_SIZE;
	int count=0;

public:

	ArrayStack() : STACK_SIZE(2000)
	{
		items = new T[STACK_SIZE];
		top = -1;
	}  // end default constructor

	bool isEmpty() const
	{
		return top == -1;
	}  // end isEmpty

	bool push(const T& newEntry)
	{
		if (top == STACK_SIZE - 1) return false;	//Stack is FULL

		top++;
		items[top] = newEntry;
		count++;
		return true;
	}  // end push

	bool pop(T& TopEntry)
	{
		if (isEmpty()) return false;

		TopEntry = items[top];
		top--;
		count--;
		return true;
	}  // end pop

	bool peek(T& TopEntry) const
	{
		if (isEmpty()) return false;

		TopEntry = items[top];
		return true;
	}  // end peek

	int getCount()
	{
		return count;
	}

	const T* toArray(int& count)
	{
		T* arr = new T[this->count];
		if (isEmpty())
			return nullptr;
		else
		{
			for (int i = 0; i < count; i++)
			{
				arr[i] = items[i];
			}
		}
		return arr;

	}

}; // end ArrayStack

#endif
