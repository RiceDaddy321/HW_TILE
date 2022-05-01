
#include "tiling.h"
#include "vertex.h"
#include <unordered_map>

using namespace std;


// Finds a (shortest according to edge length) augmenting path
// from s to t in a graph with vertex set V.
// Returns whether there is an augmenting path.
bool augmenting_path(Vertex* s, Vertex* t, 
        unordered_set<Vertex*> V, vector<Vertex*> &P)
{
        // Check that s and t aren't nullptr
        if (s == nullptr || t == nullptr)
	{
		cerr << "augmenting_path() was passed nullptr s or t." << endl;
		abort();
	}

        // Check that s and t are in the graph
        if (V.find(s) == V.end() || V.find(t) == V.end())
	{
		cerr << "augmenting_path() was passed s or t not in V." << endl;
		abort();
	}

	// Check that every vertex has valid neighs/weights.
	for (Vertex* v : V)
		for (Vertex* vn : v->neighs)
			if (v->weights.find(vn) == v->weights.end())
			{
				cerr << "augmenting_path() was passed invalid vertex." << endl;
				abort();
			}

        // Since augmenting paths should have the fewest edges,
	// not the minimum weight, run BFS.
	queue<Vertex*> Q;
	Q.push(s);

	unordered_set<Vertex*> R;
	R.clear(); 
	R.insert(s);

	unordered_map<Vertex*, Vertex*> prev;

	while (!Q.empty())
	{
		Vertex* cur = Q.front();
		Q.pop();

		for (Vertex* nei : cur->neighs)
		{
			// Must have positive edge weight
			if (cur->weights[nei] == 0)
				continue;

			if (R.find(nei) == R.end())
			{
				Q.push(nei);
				R.insert(nei);
				prev[nei] = cur; 
			}
		}
	}      

        // If BFS never reached t
        if (R.find(t) == R.end())
                return false;

        // Reconstruct shortest path backwards
        P.clear();
        P.push_back(t);
        while (P[P.size()-1] != s)
                P.push_back(prev[P[P.size()-1]]);

        // Reverse shortest path
        for (int i = 0; i < P.size()/2; ++i)
		swap(P[i], P[P.size()-1-i]);

        return true;
}

// Returns the maximum flow from s to t in a weighted graph with vertex set V.
// Assumes all edge weights are non-negative.
int max_flow(Vertex* s, Vertex* t, unordered_set<Vertex*> V)
{
	// If s or t is invalid.
        if (s == nullptr || t == nullptr)
	{
		cerr << "max_flow() was passed nullptr s or t." << endl;
		abort(); 
	}
	cout << "debug" << endl; //debug

	// If s or t is not in the vertex set.
        if (V.find(s) == V.end() || V.find(t) == V.end())
	{
		cerr << "max_flow() was passed s or t not in V." << endl;
		abort(); 
	}

	cout << "debug" << endl; //debug

	// Check that every vertex has valid neighs/weights.
	for (Vertex* v : V)
		for (Vertex* vn : v->neighs)
			if (v->weights.find(vn) == v->weights.end())
			{
				cerr << "max_flow() was passed invalid vertex." << endl;
				abort();
			}
	cout << "debug" << endl; //debug

        // Create a deep copy of V to use as the residual graph
        unordered_set<Vertex*> resV;
        unordered_map<Vertex*, Vertex*> C; // Maps vertices in V to copies in resV
        for (Vertex* vp : V)
        {
                Vertex* rp = new Vertex;
                resV.insert(rp);
                C[vp] = rp;
        }
	cout << "debug" << endl; //debug

        for (Vertex* vp : V)
                for (Vertex* np : vp->neighs)
                {
                        C[vp]->neighs.insert(C[np]);
                        C[vp]->weights[C[np]] = vp->weights[np];
                }
	cout << "debug" << endl; //debug

	// Add any missing necessary "back" edges. 
        for (Vertex* vp : V)
                for (Vertex* np : vp->neighs)
		{
			if (C[np]->neighs.find(C[vp]) == C[np]->neighs.end())
			{
				C[np]->neighs.insert(C[vp]);
				C[np]->weights[C[vp]] = 0;
			}
		}
	cout << "debug" << endl; //debug

        // Run Edmonds-Karp
        while (true)
        {
                // Find an augmenting path
                vector<Vertex*> P;
                if (!augmenting_path(C[s], C[t], resV, P))
                        break;  
                // Update residual graph
                for (int i = 0; i < P.size()-1; ++i)
                {
                        --((*(resV.find(P[i])))->weights[P[i+1]]);
                        ++((*(resV.find(P[i+1])))->weights[P[i]]);
                }
        }
	cout << "debug" << endl; //debug


        // Compute actual flow amount
        int flow = 0;
        for (Vertex* snp : C[s]->neighs)
                flow += 1 - C[s]->weights[snp];
	cout << "debug" << endl; //debug

        // Delete residual graph
        for (Vertex* vp : resV)
                delete vp;

        return flow;
}


bool has_tiling(string floor)
{
        //set the conditions
		vector<Vertex*> A;
		vector<Vertex*> B;
		unordered_map<int, Vertex*> VertexSet;
		unordered_set<Vertex*> map;
		unordered_map<Vertex*, bool> isInASet;
		int row_length;

		//find row length
		for(row_length = 0; row_length < floor.size() && floor[row_length] == ' '; row_length++);
	
		//runs in O(s). Adds all vertices
		for(int i = 0; i < floor.size(); i++)
		{
			if(floor[i] == ' ')
			{
				VertexSet[i] = new Vertex();
				map.insert(VertexSet[i]);
			}
		}
		cout << "added vertices" << endl; //debug

		isInASet[VertexSet[0]] = true;
		A.push_back(VertexSet[0]);

		//connect to others
		for(int i=0;i<floor.size();i++)
		{
			if(floor[i] == ' ' && isInASet[VertexSet[i]])
			{
				//check left
				if(i+1 < floor.size() && floor[i+1] == ' ' )
				{
					//make an edge
					VertexSet[i]->neighs.insert(VertexSet[i+1]);
					VertexSet[i]->weights[VertexSet[i+1]]  = 1;
					isInASet[VertexSet[i+1]] = false;
					B.push_back(VertexSet[i+1]);

				}

				//check down
				if(i + row_length < floor.size() && floor[i+row_length] == ' ' )
				{
					//make an edge
					VertexSet[i + row_length]->neighs.insert(VertexSet[i+row_length]);
					VertexSet[i + row_length]->weights[VertexSet[row_length+i]]  = 1;
					isInASet[VertexSet[i+row_length]] = false;
					B.push_back(VertexSet[i+row_length]);
				}
			}
			else if(floor[i] == ' ' && !isInASet[VertexSet[i]])
			{
				//check left
				if(i+1 < floor.size() && floor[i+1] == ' ' )
				{
					//make an edge, backwards
					VertexSet[i+1]->neighs.insert(VertexSet[i]);
					VertexSet[i+1]->weights[VertexSet[i]]  = 1;
					isInASet[VertexSet[i+1]] = true;
					A.push_back(VertexSet[i+1]);
				}

				//check down
				if(i + row_length < floor.size() && floor[i+row_length] == ' ' )
				{
					//make an edge
					VertexSet[i+row_length]->neighs.insert(VertexSet[i]);
					VertexSet[i+row_length]->weights[VertexSet[i]]  = 1;
					isInASet[VertexSet[i+row_length]] = true;
					A.push_back(VertexSet[i+row_length]);
				}
			}
		}

		cout << "connected vertices" << endl; //debug

		//make s
		Vertex* s = new Vertex;
		map.insert(s);
		for(int i= 0; i < A.size(); i++)
		{
			s->neighs.insert(A[i]);
			s->weights[A[i]] = 1;
		}

		cout << "added s" << endl; //debug

		//make t
		Vertex* t = new Vertex;
		map.insert(t);
		for(int i= 0; i < B.size(); i++)
		{
			B[i]->neighs.insert(t);
			B[i]->weights[t] = 1;
		}
		cout << "added t" << endl; //debug

		int max_flow_result = max_flow(s, t, map);

		cout << "ran edmund car[" << endl; //debug

		return max_flow_result == A.size() ? true : false;
}



