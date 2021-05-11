#include <iostream>
#include <vector>
#include <fstream>

#include "trees.h"
using namespace std;

void file_to_data(vector<char>&, vector<vector<int>>&, char*);
// Loads Data from file into usable form:
// Inputs: Empty Polish Expression, Empty Block Sizes, File Name

int main(int argc, char*argv[])
{
	// Checking to make sure arguments where passed correctly
	if (argc != 2)
	{
		cout << "Please provide the command line argument correctly." << endl;
		cout << "Format: executable data.txt" << endl;
		return 0;
	}
	// Printing out argument that was passed
	for(int i=1; i<argc; i++)
	{
		cout << "Your input is " << argv[i] << endl;
		cout << endl;
		cout << endl;
	}
	
	// Vectors for Holding Input Data
	vector<char> polish_expression;
	vector<vector<int>> block_sizes;
	

	
	// Filling in Data
	file_to_data(polish_expression, block_sizes, argv[1]);
	
	
	// Making tree and storing pointer to root node
	Node* root = PEToTree(polish_expression);

	
	// Making vector where every entry is all the nodes on that level of tree
	vector<vector<Node*>> leveled_tree = treeLevels(root);
		
		
	// Iterating over the levels bottom up to calculate the different possible w,h pairs
	bottomUp(leveled_tree, block_sizes);
	
	// Top Down, getting every blocks orientation, block_sizes input is only for vector sizing information
	vector<vector<int>> orientations = topDown(root, block_sizes);
	
	
	// Output for grader
	cout << endl;
	cout << endl;
	
	cout << "New Total Size and Orientation of all Blocks in New Configuration: " << endl;
	for (int i=0; i<orientations.size(); i++)
	{
		if (i==0) 
		{
			cout << "Total sizing reduced to {w, h}: ";
		}
		else 
		{
			cout << "Node " << i << " orientation: ";
		}
		
		for (int j=0; j<orientations[i].size(); j++)
		{
			cout << orientations[i][j] << " ";
		}
		cout << endl;
	}

}


// Getting Data
void file_to_data(vector<char>& polish_expression, vector<vector<int>>& block_sizes, char * file)
{
	string p_e;
	ifstream coeff(file); //open file
	if (coeff.is_open())
	{
		string one;
		string two;
		int first_line = 0;
		while(!coeff.eof())
		{
			if (first_line == 0) // if on first line of document get Polish Expression
			{
				getline(coeff, p_e, '\n');
				first_line++;
			}
			else // get the dimensions of the block sizes
			{
				vector<int> temp;
				getline(coeff, one, ' ');
				temp.push_back(stoi(one));
				getline(coeff, two, '\n');
				temp.push_back(stoi(two));
				block_sizes.push_back(temp);
			}
		}
		coeff.close();
	}
	
	// Parse through p_e to get easier to use form for globaly accesible polish expression
	for (int i=0; i<p_e.size(); i++)
	{
		if (p_e[i] != '-')
		{
			polish_expression.push_back(p_e[i]);
		}
	}
	
	// print out the block sizes and polish expression to console for debugging
	cout << "Input Polish Expression: " << endl;
	for (unsigned int i=0; i<polish_expression.size(); i++)
	{
		cout << polish_expression[i] << " ";
	}
	cout << endl;
	cout << endl;
	cout << "Block Sizes: " << endl;
	for (int i=0; i<block_sizes.size(); i++)
	{
		cout << "Block ";
		cout <<	i+1;
		cout << ": ";
		for (int j=0; j<block_sizes[i].size(); j++)
		{
			cout << block_sizes[i][j] << " ";
		}
		cout << endl;
	}
}
