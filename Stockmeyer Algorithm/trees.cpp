#include <iostream>
#include <algorithm>
#include <queue>
#include <vector>
#include <stdio.h>
#include <ctype.h>
#include <iostream>
#include "trees.h"

using namespace std;


// makes a node with NULL values
Node* createNode(char label)
{
	
	Node *node = new Node();
	node->label = label;
	node->left = NULL;
	node->right =NULL;
}


// Given a nodes label and the tree root, returns the node. Only used in debugging
Node* search(Node* root, char label)
{
	if (root == NULL) return NULL;
	
	queue<Node*> q;
	q.push(root);
	Node* out = NULL;
	
	while(!q.empty())
	{
		Node* temp = q.front(); // get Node in queue and then remove it from queue
		q.pop();
		
		if(temp->label == label) out = temp; // if node has desired key, make out that node
		if(temp->left != NULL) q.push(temp->left); // Put left and right pointers of current nodes in queue
		if(temp->right != NULL) q.push(temp->right);
	}
	return out;
}


// Given a polish expression, constructs a slicing tree
Node* PEToTree(const vector<char>& polish_expression)
{
	//cout << "Started P-T conversion" << endl;
	vector<Node*> q; // makeshift stack
	Node *root = NULL;
	int start = 1;
	// Iterate over polish expression backwards
	for (int i = polish_expression.size()-1; i>-1; i--)
	{
		//cout << polish_expression[i] << endl << endl;
		if (start == 1) // create root node and add to stack
		{
			start = 0;
			root = createNode(polish_expression[i]);
			q.push_back(root);

		}
		else
		{
			Node* temp = q.back();
			if (isalpha(temp->label)) // is a letter?
			{
				temp->right = createNode(polish_expression[i]); // Traversing Right
				q.push_back(temp->right);

			}
			else // is number
			{
				q.pop_back();
				temp = q.back();
				
				if (temp->left == NULL)
				{
				temp->left = createNode(polish_expression[i]); // Traversing left
				q.push_back(temp->left);
				}
				
				else
				{
					int found = 0;
					while (!found) // Traversing Up tree
					{
						if (temp->left == NULL) found = 1;
						
						else
						{
							q.pop_back(); 
							temp = q.back();
						}
					}
					temp-> left = createNode(polish_expression[i]); // Traverse left
					q.push_back(temp->left);
				}
			}
		}
	}
	return root;
}


// Given root node returns vector where each index is the nodes on index+1 level of tree.
vector<vector<Node*>> treeLevels(Node* root)
{
	queue<Node*> q;
	q.push(root);
	
	vector<vector<Node*>> nodes_per_level;
	
	int num_nodes_this_level = 1;
	while(!q.empty())
	{	
		vector<Node*> nodes_on_level;
		
		int num_nodes_next_level = 0;
		for(int i = 0; i<num_nodes_this_level; i++)
		{
			Node* temp = q.front();
			q.pop();
			nodes_on_level.push_back(temp);
			if (isalpha(temp->label))
			{
				if(temp->left != NULL) {num_nodes_next_level++; q.push(temp->left);}
				if(temp->right != NULL) {num_nodes_next_level++; q.push(temp->right);}
			}
		}
		
		nodes_per_level.push_back(nodes_on_level);
		num_nodes_this_level = num_nodes_next_level;
	}
	return nodes_per_level;
}


// Constructing possibilites for area usage, storing data in tree structure
void bottomUp(const vector<vector<Node*>>& leveled_tree, const vector<vector<int>>& block_sizes)
{
	for (int i = leveled_tree.size() - 1; i > -1; i--) // iterate over levels bottom up
	{

		for (int j = 0; j < leveled_tree[i].size(); j++) // iterate over a level
		{
			if (isalpha(leveled_tree[i][j]->label)) // If node is slice
			{	
				leveled_tree[i][j]->data = stockmeyer_combination(leveled_tree[i][j]);
			}
			else //If node is block
			{
				int label = leveled_tree[i][j]->label - '0';
				vector<vector<int>> poss_dim;
				poss_dim.push_back(block_sizes[label - 1]); //Original Orientation
				
				vector<int> temp;
				temp.push_back(block_sizes[label-1][1]);
				temp.push_back(block_sizes[label-1][0]);
				poss_dim.push_back(temp); // Flipped Orientation
				
				leveled_tree[i][j]->data = poss_dim; // Set nodes values to its possible orientations	
	
			}
		}
	}
}


vector<vector<int>> stockmeyer_combination(Node* n)
{
	vector<vector<int>> L = n->left->data;
	vector<vector<int>> R = n->right->data;
	
	// Right output here to check if L and R are being filled
	// its First verticy to fail, only one that has slices as its inputs.

	int L_count = 0;
	int R_count = 0;
	
	
	vector<vector<int>> data;
	
	while(L_count < L.size() && R_count < R.size())
	{
		vector<int> temp;
		if (n->label == 'V') // Node is a verticle slice
		{
			L = sortIncreasingWidth(L);
			R = sortIncreasingWidth(R);
			
			int wL = L[L_count][0];
			int hL = L[L_count][1];
			int wR = R[R_count][0];
			int hR = R[R_count][1];
			
			int wN = wL + wR;
			int hN = max_int(hL, hR);
			
			temp.push_back(wN);
			temp.push_back(hN);
			
			
			if(hN == hR) R_count++;
			if(hN == hL) L_count++;
			
		}
		
		else if (n->label == 'H') // Node is Horizontal slice
		{
			
			L = sortDecreasingWidth(L);
			R = sortDecreasingWidth(R);
			
			int wL = L[L_count][0];
			int hL = L[L_count][1];
			int wR = R[R_count][0];
			int hR = R[R_count][1];
			
			int wN = max_int(wL, wR);
			int hN = hL + hR;
			
			temp.push_back(wN);
			temp.push_back(hN);
			
			if(wN == wR) R_count++;
			if(wN == wL) L_count++;
			
		}
		
		else 
		{
			cout << "Something is Wrong!" << endl;
 		}
		
		data.push_back(temp);
	}
	return data;
}


//!!!!! This is shit right now, to tired, cant think, REDO!

// Calculates best area from root nodes possibilites and then recursively finds orientation of every block;
// returns {{wT, hT}, {w1, h1}, {w2, h2}, ... , {wn, hn}}
vector<vector<int>> topDown(Node* root, const vector<vector<int>>& block_sizes)
{
	//Initialising the entire thing to size and filling with 0, makes easier to orginise final data
	vector<vector<int>> orientations;
	for (int i=0; i<block_sizes.size()+1; i++)
	{
		vector<int> v(2);
		orientations.push_back(v);
	}
	
	
	//find min area of root nodes w,h pairs and add to return vector
	int min_size = -1;
	vector<int> min_tot_wh_pair;
	for (int i=0; i<root->data.size(); i++)
	{
		int size = root->data[i][0] * root->data[i][1];
		if (size < min_size | min_size < 0) {min_size = size; min_tot_wh_pair = root->data[i];}
	}
	
	// Update return vector and root internal orientation storage
	orientations[0] = min_tot_wh_pair;
	root->ori = min_tot_wh_pair;
	
	queue<Node*> q;
	q.push(root);
	
	while(!q.empty())
	{
		Node* temp = q.front();
		q.pop();
		
		if (isalpha(temp->label)) // is a slice
		{
			// if its a slice put connected nodes on queue
			q.push(temp->left);
			q.push(temp->right);
			vector<vector<int>> L = temp->left->data;
			vector<vector<int>> R = temp->right->data;
			
			// iterate over possible orientations of Nodes conected to temp
			if(temp->label == 'V') // if vertical
			{
				L = sortIncreasingWidth(L);
				R = sortIncreasingWidth(R);
				
				for (int i=0; i<L.size(); i++)
				{
					for (int j=0; j<R.size(); j++)
					{
						int wN = L[i][0] + R[j][0];
						int hN = max_int(L[i][1], R[j][1]);
						
						if (wN==temp->ori[0] && hN==temp->ori[1])
						{
							temp->left->ori = L[i];
							temp->right->ori = R[j];
						}
					}
				}
				
			}
			else // if horizontal
			{
				L = sortDecreasingWidth(L);
				R = sortDecreasingWidth(R);
				
				for (int i=0; i<L.size(); i++)
				{
					for (int j=0; j<R.size(); j++)
					{
						int wN = max_int(L[i][0], R[j][0]);
						int hN = L[i][1] + R[j][1];
						
						if (wN==temp->ori[0] && hN==temp->ori[1])
						{
							temp->left->ori = L[i];
							temp->right->ori = R[j];
						}
					}
				}	
			}	
		}
		else // is a number
		{
			// Orientation is taken care of in above, now just put it into the return vector
			int node_num = temp->label - '0';
			orientations[node_num] = temp->ori;
		}
		
	}
	
	return orientations;
}


// Below are my not so good sorting algorithms
vector<vector<int>> sortIncreasingWidth(vector<vector<int>> n)
{
	vector<vector<int>> sorted;
	vector<int> widths;
	
	for (int i=0; i<n.size(); i++)
	{
		widths.push_back(n[i][0]);
	}
	
	sort(widths.begin(), widths.end());
	
	for (int i=0; i<widths.size(); i++)
	{
		for (int j=0; j<n.size(); j++)
		{
			if (widths[i] == n[j][0]) 
			{
				int in = 0;
				for (int k=0; k<sorted.size(); k++)
				{
					if (sorted[k][0] == n[j][0] && sorted[k][1] == n[j][1]) in = 1;
				}
				if (in == 0) sorted.push_back(n[j]);
			}
		}
	}
	
	return sorted;
}

vector<vector<int>> sortDecreasingWidth(vector<vector<int>> n)
{
	vector<vector<int>> sorted;
	vector<int> widths;
	
	for (int i=0; i<n.size(); i++)
	{
		widths.push_back(n[i][0]);
	}
	
	sort(widths.begin(), widths.end(), greater<int>());
	
	for (int i=0; i<widths.size(); i++)
	{
		for (int j=0; j<n.size(); j++)
		{
			if (widths[i] == n[j][0]) 
			{
				int in = 0;
				for (int k=0; k<sorted.size(); k++)
				{
					if (sorted[k][0] == n[j][0] && sorted[k][1] == n[j][1]) in = 1;
				}
				if (in == 0) sorted.push_back(n[j]);
			}
		}
	}
	
	return sorted;
}


// Simple function. returns max of two ints
int max_int(int a, int b)
{
	if (a > b) return a;
	if (b > a) return b;
	if (a == b) {return a;}
}