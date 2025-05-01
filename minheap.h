#pragma once
//this is the min heap for scores. will arrange 
#include <iostream>
#include <fstream>
using namespace std;
using namespace sf;


struct score
{
	int id;	//player ID
	int pscore;
	score()
	{
		id = pscore = 0;
	}
};

//heap is 0 based
//2k+1
//2k+2
class minheap
{
	int maxsize = 100;
	int size = 0;
	score* heap;

	void swap(score& a, score& b)
	{
		score temp = a;
		a = b;
		b = temp;
	}

	void printheap()
	{
		cout << "Printing Minheap" << endl;
		for (int i = 0; i < maxsize && heap[i].pscore != -1; i++)
		{
			cout << heap[i].pscore << ' ';
		}
		cout << endl;
	}


public:
	minheap()
	{
		heap=  new score[maxsize];
		for (int i = 0; i < maxsize; i++)
		{
			heap[i].pscore = -1;
		}
	}
	
	void minheapify_up(int index)
	{
		while (index != 0)
		{
			int parent = (index - 1) / 2;
			if (heap[parent].pscore > heap[index].pscore)
			{
				swap(heap[parent], heap[index]);
				index = parent;
			}
			else
				break;
		}
	}

	void insert(int a)
	{
		if (size >= maxsize)
			return;
		if (size == 0)
		{
			heap[size++].pscore = a;
			return;
		}
		heap[size].pscore = a;
		minheapify_up(size);
		size++;
	}

	void minheapify_down(int ind)
	{
		int smallest = ind;
		int left = 2 * ind + 1;;
		int right = 2 * ind + 2;

		if (left < size && heap[left].pscore < heap[smallest].pscore)
			smallest = left;
		if (right < size && heap[right].pscore < heap[smallest].pscore)
			smallest = right;

		if (smallest != ind)
		{
			swap(heap[smallest], heap[ind]);
			minheapify_down(smallest);
		}
	}

	//deletes root 
	void Delete()
	{
		if (size == 0)
			return;
		heap[0] = heap[size--];
		minheapify_down(0);
	}

	//funtion that writes the heap to a file, in a format that it can be read again, in accordance with player IDs and scores
	void writeToFile()
	{
		ofstream outFile("scores.txt");
		if (!outFile.is_open())
		{
			cout << "Failed to open scores.txt for writing.\n";
			return;
		}

		for (int i = 0; i < size; i++)
		{
			outFile << heap[i].pscore << endl;	//NEEDS CHANGING WHEN THE PLAYER PROFILES ARE READY
		}
		outFile.close();
		cout << "Scores written to file.\n";
	}

};