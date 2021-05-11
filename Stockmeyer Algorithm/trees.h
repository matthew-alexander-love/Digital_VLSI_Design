#ifndef TREES_H
#define TREES_H

#include <vector>

using namespace std;

struct Node
{
	char label;
	vector<int> ori;
	vector<vector<int>> data;
	Node *left;
	Node *right;
};

Node* createNode(char label);

Node* search(Node* root, char label); // Only used in debugging

Node* PEToTree(const vector<char>& polish_expression);

vector<vector<Node*>> treeLevels(Node* root);

void bottomUp(const vector<vector<Node*>>& leveled_tree, const vector<vector<int>>& block_sizes);

vector<vector<int>> topDown(Node* root, const vector<vector<int>>& block_sizes);

vector<vector<int>> stockmeyer_combination(Node* n);

vector<vector<int>> sortIncreasingWidth(vector<vector<int>> n);

vector<vector<int>> sortDecreasingWidth(vector<vector<int>> n);

int max_int(int a, int b);
#endif