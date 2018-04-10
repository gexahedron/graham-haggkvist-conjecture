/*
 * File:   regular_digraph_decomposition
 * Author: Nikolay Ulyanov
 *
 * Created sometime between May and June 2014
 */

#include <random>
#include <iostream>
#include <vector>
#include <ctime>
#include <algorithm>
#include <set>
#include <sstream>
#include "freetree.h"

using namespace std;

const int maxn = 30;
const int NONE = -1;

clock_t beginClock, endClock;
vector<pair<int, int>> piiEdges;
bool doTakePair[maxn*maxn];
int degIn[maxn];
int degOut[maxn];

int dege[maxn][maxn];
bool usedEdge[maxn][maxn];
int whose_edge[maxn][maxn];

bool used_copy[maxn][maxn];
int copy_count[maxn];

int cnt;

vector<int> gr[2][maxn]; // 0 - From, 1 - To

int n;
const int treeSize = 5; // number of edges
const int min_n = treeSize + 3; // min_n > treeSize + 1
const int max_n = min_n;
const int max_iter = 20;

int neib[maxn][treeSize + 1];
bool isn[maxn][treeSize + 1];
int vPart[treeSize + 1];
int curTree, curVertex;
// TODO: fix all this treeSize + 1 to treeSize; don't mess with indices

bool genEdge(int iter, int v) {
  if (iter == treeSize + 1) {
    if (v == n - 1) {
      // TODO: check that each vertex is incident to
      // treeSize+1 number of trees
      bool regular_count = true;
      for (int v1 = 0; v1 < n; ++v1) {
        set<int> trees;
        for (int v2 = 0; v2 < n; ++v2) {
          if (usedEdge[v1][v2]) {
            trees.insert(whose_edge[v1][v2]);
          }
          if (usedEdge[v2][v1]) {
            trees.insert(whose_edge[v2][v1]);
          }
        }
        if (trees.size() != treeSize + 1) {
          regular_count = false;
          break;
        }
      }
      if (!regular_count) {
        return false;
      }
      ++cnt;
      return true; // use false for counting, true for detecting 0
    }
    else {
      if (genEdge(1, v + 1)) {
        return true;
      }
    }
    return false;
  }

  int vInTree = treeDatabase[curTree][iter - 1 + 2];
  int curV = neib[v][vInTree];
  int dir = vPart[vInTree];

  vector<tuple<int, int, int, int>> forSort;
  for (int i = 0; i < treeSize; ++i) {
    int u = gr[dir][curV][i];
    if (isn[u][iter]) {
      continue;
    }

    int v1 = curV;
    int v2 = u;
    if (dir == 1) {
      swap(v1, v2);
    }
    if (usedEdge[v1][v2]) {
      continue;
    }

    // TODO: add description what isBad here
    bool isBad = false;
    for (int j = 0; j < iter - 1; ++j) {
      isBad = (u == neib[v][j]);
      if (isBad) {
        break;
      }
    }
    if (isBad) {
      continue;
    }

    isn[u][iter] = true;
    neib[v][iter] = u;
    usedEdge[v1][v2] = true;

    whose_edge[v1][v2] = v;
    const bool prev_used_copy_v1 = used_copy[v1][v];
    const bool prev_used_copy_v2 = used_copy[v2][v];
    if (!used_copy[v1][v]) {
        used_copy[v1][v] = true;
        ++copy_count[v1];
    }
    if (!used_copy[v2][v]) {
        used_copy[v2][v] = true;
        ++copy_count[v2];
    }

    if (copy_count[v1] <= treeSize + 1 && copy_count[v2] <= treeSize + 1) {
      if (genEdge(iter + 1, v)) {
        return true;
      }
    }

    if (!prev_used_copy_v1) {
        used_copy[v1][v] = false;
        --copy_count[v1];
    }
    if (!prev_used_copy_v2) {
        used_copy[v2][v] = false;
        --copy_count[v2];
    }

    usedEdge[v1][v2] = false;
    isn[u][iter] = false;
  }
  return false;
}

int main(int argc, char** argv) {
  //random_device rd;
  mt19937 g(42);//rd());

  beginClock = clock();
  bool wasWTF = false;
  int nomIter = 0;

  freeTree(treeSize + 1);
  cerr << "totalAmount=" << totalAmount << endl;

  while (!wasWTF) {
    cerr << "iter=" << nomIter << "; ";
    for (n = min_n; n <= max_n; ++n) {
      cerr << "n=" << n << ": ";
      if (wasWTF) {
        break;
      }

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
        shuffle(piiEdges.begin(), piiEdges.end(), g);
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
        for (int i = 0; i < n; ++i) {
          allIsGood = allIsGood && (degIn[i] == treeSize) && (degOut[i] == treeSize);
        }
        if (allIsGood) {
          isRegular = true;
        }
      }

      for (int i = 0; i < n; ++i) {
        gr[0][i].clear();
        gr[1][i].clear();
      }
      for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
          dege[i][j] = 0;
        }
      }

      for (int i = 0; i < (int) piiEdges.size(); ++i) {
        int v1, v2;
        v1 = piiEdges[i].first;
        v2 = piiEdges[i].second;
        if (doTakePair[i]) {
          dege[v1][v2] = 1;
        }
      }

      for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
          if (dege[i][j] != 0) {
            gr[0][i].push_back(j);
            gr[1][j].push_back(i);
          }
        }
      }
      cerr << "tree#=";
      for (curTree = 0; curTree < totalAmount - 1; ++curTree) {
        cerr << curTree << ",";
        for (int curType = 0; curType < 2; ++curType) {
          vPart[0] = curType;
          for (int i = 0; i < treeSize; ++i) {
            vPart[i + 1] = 1 - vPart[treeDatabase[curTree][i + 2]];
          }

          for (int i = 0; i < n; ++i) {
            for (int j = 0; j <= treeSize; ++j) {
              isn[i][j] = false;
            }
          }

          for (int i = 0; i < n; ++i) {
            copy_count[i] = 0;
            for (int j = 0; j < n; ++j) {
              usedEdge[i][j] = false;
              whose_edge[i][j] = NONE;
              used_copy[i][j] = false;
            }
          }

          for (int i = 0; i < n; ++i) {
            neib[i][0] = i;
          }

          cnt = 0;
          bool result = genEdge(1, 0);

          if (!cnt) {
            cout << "WTF" << endl;
            wasWTF = true;
            cout << "iter#=" << nomIter << "; tree#=" << curTree << "; type#=" << curType << endl;
            cout << "tree: ";
            for (int i = 0; i <= treeSize; ++i) {
              cout << treeDatabase[curTree][i + 1] << " ";
            }
            cout << endl;
            cout << "parts: ";
            for (int i = 0; i <= treeSize; ++i) {
              cout << vPart[i] << " ";
            }
            cout << endl;
            for (int i = 0; i < n; ++i) {
              cout << i << ": ";
              for (int j = 0; j < n; ++j) {
                if (dege[i][j] > 0) {
                  cout << j << " ";
                }
              }
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

