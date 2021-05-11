#include <iostream>
#include <string>
#include <iomanip>
#include <sstream>
#include <vector>
#include <fstream>

using namespace std;

void print2dVector(const vector<vector<int>>&);
void printVector(const vector<int>&);
void file_to_vector(vector<vector<int>>&, char * file); // every line of the file is made into an element of a vector that is passed in.
bool int_in_vector(int, const vector<int>&); 
void initial_partition(const vector<int>&, vector<int>&, vector<int>&);
void initial_cost(const vector<int>&, const vector<vector<int>>&, const vector<vector<int>>&, vector<int>&);
void update_cut_set(const vector<int>&, const vector<int>&, const vector<vector<int>>&, vector<vector<int>>&, vector<vector<int>>&);
int nodes_connected(int, int, const vector<vector<int>>&);
int index_of_value(int, vector<int>);
void KL(const vector<int>&, const vector<vector<int>>&);

// Below 3 are only for preprocessing, kept running into bug where there was empty line at end of text file and causes error
void remove_blank_lines(const char*);
int num_lines(const char*);
void del_line(const char*, int);


int main(int argc, char *argv[])
{
	// Checking to make sure arguments where passed correctly
	if (argc != 2)
	{
		printf("Please provide the command line arguments correctly \n");
		printf("Format: executable data.txt \n");
		return 0;
	}
	for(int i=1;i<argc;i++)
	{
		printf("Your input is %s\n", argv[i]);
		cout << endl;
		cout << endl;
	}
	
	// vectors needed for algorithm
	vector<vector<int>> parsed_vect;
	vector<vector<int>> E;
	vector<int> V;
	
	// Remove any blank lines in input file as they cause errors to parser
	remove_blank_lines(argv[1]);
	del_line(argv[1], num_lines(argv[1]));
	
	// Parse through text file input
	file_to_vector(parsed_vect, argv[1]);
	
	// Populate Initital Node vector
	for (int i = 1; i < parsed_vect[0][0] + 1; i++)
	{
		V.push_back(i);
	}
	
	// Populate Initital Edges vector
	for (int i = 1; i < parsed_vect.size(); i++)
	{
		E.push_back(parsed_vect[i]);
	}
	
	// Start Optimization Process
	KL(V, E);
	return 0;
}

void KL(const vector<int>& V, const vector<vector<int>>& E)
{
	// variables needed
	int g_max = -1;
	vector<int> A;
	vector<int> B;
	vector<vector<int>> cut_set;
	vector<vector<int>> internal_edges;
	// initial partition
	initial_partition(V, A, B);
	// Output for grader and debugging
	cout << "Initial Partition: " << endl;
	cout << "Group A:" << endl;
	printVector(A);
	cout << endl;
	cout << "Group B:" << endl;
	printVector(B);
	cout << endl;
	
	do
	{
		vector<int> gv;
		vector<int> av;
		vector<int> bv;
		vector<int> cost;
		vector<int> nfA = A;
		vector<int> nfB = B;
		
		// Initialise Cost
		update_cut_set(A, B, E, cut_set, internal_edges);
		initial_cost(V, internal_edges, cut_set, cost);
		
		// Output for grader and debugging
		cout << "Start of loop: " << cut_set.size() << " edges cut by the partition. These edges are: " <<endl;
		print2dVector(cut_set);
		cout << endl;
		
		for (int i=0; i < (V.size()/2); i++)
		{
			// Calculate Max gain and nodes needed
			vector<vector<int>> gain;
			int mgain = -9999;
			int nfA_ind = -1;
			int nfB_ind = -1;
			for (int j = 0; j < nfA.size(); j++)
			{
				for (int k = 0; k <nfB.size(); k++)
				{	
					
					int gain = cost[nfA[j] - 1] + cost[nfB[k] - 1] - (2*nodes_connected(nfA[j], nfB[k], E));
					if (gain > mgain)
					{
						mgain = gain;
						nfA_ind = j;
						nfB_ind = k;
					}
				}
			}
			
			// Storing nodes
			int a = nfA[nfA_ind];
			int b = nfB[nfB_ind];
			
			// add max_gain to gv, add g1 node to g1v, add g2 node to g2v
			gv.push_back(mgain);
			av.push_back(a);
			bv.push_back(b);
			
			// Remove a and b from consideration rest of this pass
			nfA.erase(nfA.begin() + nfA_ind);
			nfB.erase(nfB.begin() + nfB_ind);
			
			// Update D values 
			vector<int> temp_cost;
			for (int j=0; j<nfA.size(); j++)
			{
				int new_cost = cost[nfA[j] - 1] + (2*nodes_connected(nfA[j], a, E)) - (2*nodes_connected(nfA[j], b, E));
				cost.erase(cost.begin() + (nfA[j] - 1));
				cost.insert(cost.begin() + (nfA[j] - 1), new_cost);
			}
			for (int k=0; k<nfB.size(); k++)
			{
				int new_cost = cost[nfB[k] - 1] + (2*nodes_connected(nfB[k], b, E)) - (2*nodes_connected(nfB[k], a, E));
				cost.erase(cost.begin() + (nfB[k] - 1));
				cost.insert(cost.begin() + (nfB[k] -1), new_cost);
			}
			// debugging:
			//cout << "av: " << endl;
			//printVector(av);
			//cout << endl;
			//cout << "bv: " << endl;
			//printVector(bv);
			//cout << endl;
			//cout << "gv: " << endl;
			//printVector(gv);
			//cout << endl;
		}
		// finding k value
		int amount_of_swaps;
		vector<int> sums;
		int sum = 0;
		for (int i=0; i<gv.size(); i++)
		{
			sum = sum + gv[i];
			sums.push_back(sum);
		}
		g_max = -9999;
		for (int i=0; i<sums.size(); i++)
		{
			if (sums[i] > g_max)
			{
				g_max = sums[i];
				amount_of_swaps = i+1;
			}
		}
		
		//debugging
		//cout << "g_max = " << g_max << endl;
		//cout << "k = " << amount_of_swaps << endl;
		
		//Exchange all the nodes
		if (g_max > 0)
		{
			for(int j=0; j<amount_of_swaps; j++)
			{
				int A_ind = index_of_value(av[j], A);
				A.erase(A.begin() + A_ind);
				int B_ind = index_of_value(bv[j], B);
				B.erase(B.begin() + B_ind);
				
				A.push_back(bv[j]);
				B.push_back(av[j]);
			}
		}
		
		// update the cut_set after swapping all the nodes
		update_cut_set(A, B, E, cut_set, internal_edges);
		
		// output for grader and debugging
		cout << "///////////////////////////////////////////////////////////////////////////////////////////" << endl;
		cout << "Changed List: " << endl;
		cout << "New A: " << endl;
		printVector(A);
		cout << endl;
		cout << "New B: " << endl;
		printVector(B);
		cout << endl;
		cout << "End of loop: : " << cut_set.size() << " edges cut by the partition. These edges are" << endl;
		print2dVector(cut_set);
		cout << endl;
	}
	while(g_max <=0);
	
}

int index_of_value(int a, vector<int> vect)
{
	for (int i=0; i<vect.size(); i++)
	{
		if (vect[i] == a)
		{
			return i;
		}
	}
	return -1;
}

int nodes_connected(int a, int b, const vector<vector<int>>& E)
{
	int connected = 0;
	for (int i=0; i < E.size(); i++)
	{
		if (int_in_vector(a, E[i]) && int_in_vector(b, E[i]))
		{
			connected = 1;
			return connected;
		}
	}
	return connected;
}


void update_cut_set(const vector<int>& group1, const vector<int>& group2, const vector<vector<int>>& edges, vector<vector<int>>& cut_set, vector<vector<int>>& internal_edges)
{
	cut_set.clear();
	internal_edges.clear();
	for(int i = 0; i < edges.size(); i++)
	{
		bool first;
		bool second;
		first = int_in_vector(edges[i][0], group1);
		second = int_in_vector(edges[i][1], group1);
		
		if (first != second)
		{
			cut_set.push_back(edges[i]);
		}
		else
		{
			internal_edges.push_back(edges[i]);
		}
	}
}


void initial_cost(const vector<int>& nodes, const vector<vector<int>>& internal_edges, const vector<vector<int>>& cut_set, vector<int>& cost)
{
	cost.clear();
	for (int i = 0; i < nodes.size(); i++)
	{
		int plus = 0;
		int minus = 0;
		// Check how many time in cutset
		for (int k = 0; k < cut_set.size(); k++) 
		{ 
			if (int_in_vector(nodes[i], cut_set[k]))
			{
				plus++;
			}
		}
		// Check how many internal edges
		for (int k = 0; k < internal_edges.size(); k++)
		{
			if (int_in_vector(nodes[i], internal_edges[k]))
			{
				minus++;
			}
		}
		int c = plus - minus;
		cost.push_back(c);
	}
}


void  initial_partition(const vector<int>& nodes, vector<int>& group1, vector<int>& group2)
{
	if (nodes.size()%2 == 0) // if there an even number of nodes -> split group down middle
	{
		for (unsigned int i = 0; i < nodes.size(); i++)
		{
			if (i < nodes.size()/2)
			{
				group1.push_back(nodes[i]);
			}
			else
			{
				group2.push_back(nodes[i]);
			}
		}
	}
	else
	{
		for (unsigned int i = 0; i < nodes.size(); i++)
		{
			if (i <= (nodes.size()+1)/2)
			{
				group1.push_back(nodes[i]);
			}
			else
			{
				group2.push_back(nodes[i]);
			}
		}
	}
}


bool int_in_vector(int n, const vector<int>& vect)
{
	bool is_in = 0; 
	
	for (unsigned int i = 0; i < vect.size(); i++)
	{
		if (n == vect[i])
		{
			is_in = 1;
		}
	}
	return is_in;
}


void remove_blank_lines(const char *file_name)
{   
	ifstream fin(file_name); // open original file 
	
	ofstream fout;                
	fout.open("temp.txt", ios::out); // make new file
	string str;
	while(!fin.eof()) // while original is not done
	{
		getline(fin, str);
		if (str.length() != 0) // if the line isnt empty place it in the new document
		{
			fout << str << endl;
		}
	}
	fout.close();  
	fin.close();  
	remove(file_name); // remove old file   
	rename("temp.txt", file_name); // give new file old name
}


int num_lines(const char *file_name)
{
	ifstream fp(file_name);
	int num_lines = 0;
	string dummy;
	while(!fp.eof())
	{	
		getline(fp, dummy);
		num_lines++;
	}
	fp.close();
	return num_lines;
}


void del_line(const char *file_name, int n)   
{    
  ifstream fin(file_name);    
  ofstream fout;                
  fout.open("temp.txt", ios::out); 
  
  char ch; 
  int line = 1;            
  while(fin.get(ch)) 
  {      
    if(ch == '\n') 
      line++; 
     
    if(line != n)      // content not to be deleted 
      fout<<ch; 
  } 
  fout.close();  
  fin.close();   
  remove(file_name);        
  rename("temp.txt", file_name);  
}


void file_to_vector(vector<vector<int>>& parsed_vect, char * file) // made input vecotor a nested vector so can properly set pairs
{
	ifstream coeff(file); // open file
	if (coeff.is_open()) // if the file was successfully opened
	{
		string one;
		string two;
		while(!coeff.eof()) //while the end of file is not reached
		{
			vector<int> temp;
			getline(coeff, one, ' ');
			temp.push_back(stoi(one));
			getline(coeff, two, '\n');
			temp.push_back(stoi(two));
			parsed_vect.push_back(temp);
		}
		coeff.close();
	}
	else // if the file was not opened
	{
		printf("The file provided could not be opened. \n");
	}
}


void printVector(const vector<int>& newMyVector)
{	cout << "Vector: ";
	for (unsigned int i = 0; i<newMyVector.size(); i++)
	{
		cout << newMyVector[i] << " ";
	}
	cout << endl;
}


void print2dVector(const vector<vector<int>>& newMyVector)
{
	for (int i = 0; i < newMyVector.size(); i++) { 
		for (int j = 0; j < newMyVector[i].size(); j++) 
			cout << newMyVector[i][j] << " "; 
		cout << endl; 
    } 
}