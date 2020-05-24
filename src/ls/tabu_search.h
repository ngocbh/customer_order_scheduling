#ifndef __TABU_SEARCH_H__
#define __TABU_SEARCH_H__

#include "../model/utils.h"

struct TabuTable {
	TabuTable(int _tabu_size) : tabu_size(_tabu_size) {}

	bool is_find(int node) const
	{
		return tabu_hash.find(node) != tabu_hash.end();
	}

	void push(int node)
	{
		if(is_find(node)) return;

		tabu_hash.insert(node);
		tabu_queue.push(node);

		if(tabu_hash.size() > tabu_size) pop();
	}

	void pop()
	{
		auto top = tabu_queue.front();
		tabu_queue.pop();
		tabu_hash.erase(top);
	}

	unordered_set<int> tabu_hash;
	queue<int> tabu_queue;
	int tabu_size;
};

#endif