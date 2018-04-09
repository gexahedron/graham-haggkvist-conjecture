#include <iostream>
#include <cstdio>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <set>
#include <sstream>
#include <random>
#include "freetree.h"

using namespace std;

// TODO: rename everything so it makes sense

const int maxn = 30;

clock_t beginClock, endClock;
vector<pair<int, int>> piiEdges;
bool doTakePair[maxn*maxn];
int degIn[maxn];
int degOut[maxn];

int dege[maxn][maxn];
bool usedEdge[maxn][maxn];
//int kind[maxn][maxn];

int cnt;

vector<int> gr[2][maxn]; // 0 - From, 1 - To

int n;
const int treeSize = 6;
const int min_n = treeSize + 3; // min_n > treeSize + 1 // TODO use camelCase everywhere
const int max_n = min_n;
const int max_iter = 20;

int neib[maxn][treeSize + 1];
bool isn[maxn][treeSize + 1];
int curTree, curVertex;
// TODO: fix all this treeSize + 1 to treeSize; don't mess with indices


int treeIndices[maxn];

/*
int n, k;
vector<vector<int>> g;
vector<int> mt;
vector<char> used;
 
bool try_kuhn(int v) {
    if (used[v]) return false;
    used[v] = true;
    for (size_t i=0; i<g[v].size(); ++i) {
        int to = g[v][i];
        if (mt[to] == -1 || try_kuhn(mt[to])) {
            mt[to] = v;
            return true;
        }
    }
    return false;
}

int main() {
    mt.assign(k, -1);
    vector<char> used1(n);
    for (int i=0; i<n; ++i) {
        for (size_t j=0; j<g[i].size(); ++j) {
            if (mt[g[i][j]] == -1) {
                mt[g[i][j]] = i;
                used1[i] = true;
                break;
            }
        }
    }
    for (int i=0; i<n; ++i) {
        if (used1[i]) continue;
        used.assign(n, false);
        try_kuhn(i);
    }
 
    for (int i=0; i<k; ++i) {
        if (mt[i] != -1) {
            printf("%d %d\n", mt[i]+1, i+1);
        }
    }
}
*/

bool genEdge(int iter, int v) {
	if (iter == treeSize + 1) {
		if (v == n - 1) {
            ++cnt;
			return true; // use false for counting, true for detecting 0
		}
		else {
			if (genEdge(1, v + 1))
				return true;
		}
		return false;
	}

	int vInTree = treeDatabase[curTree][iter - 1 + 2];
	int curV = neib[v][vInTree];
    //for (int dir = 0; dir <= 1; ++dir) {
	int dir = 0;// TODO both directions could work

    vector<tuple<int, int, int, int>> forSort;
	for (int i = 0; i < treeSize; ++i) {
		int u = gr[dir][curV][i];
		if (isn[u][iter])
			continue;

		int v1 = curV;
		int v2 = u;
		if (dir == 1)
			swap(v1, v2);
		if (usedEdge[v1][v2])
			continue;

        // TODO: add description what isBad here
		bool isBad = false;
		for (int j = 0; j < iter - 1; ++j) {
			isBad = (u == neib[v][j]);
            if (isBad)
				break;
		}
		if (isBad)
			continue;

        isn[u][iter] = true;
		neib[v][iter] = u;
		usedEdge[v1][v2] = true;
		//kind[v1][v2] = iter;
			
		if (genEdge(iter + 1, v))
			return true;

		usedEdge[v1][v2] = false;
		isn[u][iter] = false;
	}
	return false;
}

int main(int argc, char* argv[])
{
    //random_device rd;
    //mt19937 gen(rd());
    mt19937 gen(123123);

	beginClock = clock();
	bool wasWTF = false;
	int nomIter = 0;

    freeTree(treeSize + 1);
    cerr << "totalAmount=" << totalAmount << endl;

	while (!wasWTF) {
		cerr << "iter=" << nomIter << "; ";
		for (n = min_n; n <= max_n; ++n) {
			cerr << "n=" << n << ": ";
			if (wasWTF)
				break;

            // random regular graph generator
			piiEdges.clear();

			for (int i = 0; i < n; ++i) {
				for (int j = 0; j < n; ++j) {
					if (i != j) {
						piiEdges.push_back(make_pair(i,j));
                    }
                }
            }

            bool isRegular = false;
            while (!isRegular) {
                shuffle(piiEdges.begin(), piiEdges.end(), gen);
                for (int i = 0; i < n; ++i) {
                    degIn[i] = 0;
                    degOut[i] = 0;
                }
                for (int i = 0; i < (int) piiEdges.size(); ++i) {
                    doTakePair[i] = false;
                    int v1, v2;
                    v1 = piiEdges[i].first;
                    v2 = piiEdges[i].second;
                    if ((degOut[v1] < treeSize) && (degIn[v2] < treeSize)) {
                        doTakePair[i] = true;
                        ++degOut[v1];
                        ++degIn[v2];
                    }
                }
                bool allIsGood = true;
                for (int i = 0; i < n; ++i)
                    allIsGood = allIsGood && (degIn[i] == treeSize) && (degOut[i] == treeSize);
                if (allIsGood)
                    isRegular = true;
            }

            for (int i = 0; i < n; ++i) {
                gr[0][i].clear();
                gr[1][i].clear();
            }
            for (int i = 0; i < n; ++i)
                for (int j = 0; j < n; ++j) {
                    dege[i][j] = 0;
                    //kind[i][j] = 0;
                }

            for (int i = 0; i < (int) piiEdges.size(); ++i) {
                int v1, v2;
                v1 = piiEdges[i].first;
                v2 = piiEdges[i].second;
                if (doTakePair[i])
                    dege[v1][v2] = 1;
            }

            for (int i = 0; i < n; ++i) {
                for (int j = 0; j < n; ++j) {
                    if (dege[i][j] != 0) {
                        gr[0][i].push_back(j);
                        gr[1][j].push_back(i);
                    }
                }
            }

            // TODO: create additional bipartite version of the graph

            // random tree set generator (TODO maybe also a loop?)
            uniform_int_distribution<> dis(0, totalAmount - 1);
            for (int i = 0; i < n; ++i) {
                treeIndices[i] = dis(gen);
            }

            // TODO: loop of 2-factorizations
            // TODO: maximum random bipartite matching function
            // random shuffle of vertices
            // and then just try to put all trees

            cerr << "tree#=";
            for (curTree = 0; curTree < totalAmount - 1; ++curTree) {
                cerr << curTree << ",";
                for (int curType = 0; curType < 2; ++curType) {
                    for (int i = 0; i < n; ++i)
                        for (int j = 0; j <= treeSize; ++j)
                            isn[i][j] = false;

                    for (int i = 0; i < n; ++i)
                        for (int j = 0; j < n; ++j)
                                usedEdge[i][j] = false;

                    for (int i = 0; i < n; ++i)
                        neib[i][0] = i;

                    cnt = 0;
                    bool result = genEdge(1, 0);
            
                    if (!cnt) {
                        cout << "WTF" << endl;
                        wasWTF = true;
                        cout << "iter#=" << nomIter << "; tree#=" << curTree << "; type#=" << curType << endl;
                        cout << endl;
                        for (int i = 0; i < n; ++i) {
                            cout << i << ": "; 
                            for (int j = 0; j < n; ++j)
                                if (dege[i][j] > 0)
                                    cout << j << " ";
                            cout << endl;
                        }
                        cout << endl;
                        cout << endl;
                    }
                }
            }
		}
		++nomIter;
		endClock = clock();
		double elapsed_secs = double(endClock - beginClock) / CLOCKS_PER_SEC;
		beginClock = endClock;
		cerr << " time: " << elapsed_secs << "s" << endl;
	}

	endClock = clock();
	double elapsed_secs = double(endClock - beginClock) / CLOCKS_PER_SEC;
	cout << "Time: " << elapsed_secs << "s" << endl;
	return 0;
}
