#ifndef DFA_H
#define DFA_H
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
using namespace std;

class DFAclass
{
	public:
		vector<string> states;
		vector<string> transitions;
		bool flag;
		string statediff;

		DFAclass()
		{
			flag = false;
			statediff = "";
		}

		DFAclass(int size)
		{
			states.resize(size);
			transitions.resize(size);
			flag = false;
			statediff = "";
		}
};

#endif // !DFA_H



