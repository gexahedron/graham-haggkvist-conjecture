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
#include <functional>
#include "freetree.h"

using namespace std;

const int maxn = 30;
const int NONE = -1;

clock_t begin_clock, end_clock;
vector<pair<int, int>> pii_edges;
bool take_pair[maxn * maxn];
int deg_in[maxn];
int deg_out[maxn];

int edge_deg[maxn][maxn];
bool used_edge[maxn][maxn];

bool used_copy[maxn][maxn];

int cnt;

vector<int> gr[2][maxn]; // 0 - from, 1 - to

int n;
const int tree_size = 6; // number of edges
const int min_n = tree_size + 3; // min_n >= tree_size + 1
const int max_n = min_n;
const int max_tree_size = maxn;

int neib[maxn][max_tree_size];
bool neibed[maxn][maxn];
bool is_n[maxn][max_tree_size];
int v_part[max_tree_size];
int cur_tree;

bool AllGood() {
  ++cnt;
  // use false for counting solutions
  // use true for detecting that we have no solutions
  return true;
}

bool GenerateNextEdge(int iter, int tree_copy,
    const function<bool(int, int)>& generator,
    int next_iter, int next_tree_copy)
{
  const int v_in_tree = NFreeTree::tree_database[cur_tree][iter + 1];
  const int cur_v = neib[tree_copy][v_in_tree];
  const int dir = v_part[v_in_tree];

  for (int i = 0; i < tree_size; ++i) {
    int u = gr[dir][cur_v][i];
    if (is_n[u][iter]) {
      continue;
    }

    int v1 = cur_v;
    int v2 = u;
    if (dir == 1) {
      swap(v1, v2);
    }
    if (used_edge[v1][v2]) {
      continue;
    }

    if (neibed[tree_copy][u]) {
      continue;
    }

    is_n[u][iter] = true;
    neib[tree_copy][iter] = u;
    neibed[tree_copy][u] = true;
    used_edge[v1][v2] = true;

    if (generator(next_iter, next_tree_copy)) {
      return true;
    }

    neibed[tree_copy][u] = false;
    used_edge[v1][v2] = false;
    is_n[u][iter] = false;
  }
  return false;
}

bool GenByTreeByEdge(int iter, int tree_copy) {
  if (iter == tree_size + 1) {
    if (tree_copy == n - 1) {
      return AllGood();
    } else {
      if (GenByTreeByEdge(1, tree_copy + 1)) {
        return true;
      }
    }
    return false;
  }
  return GenerateNextEdge(iter, tree_copy, GenByTreeByEdge, iter + 1, tree_copy);
}

bool GenByEdgeByTree(int iter, int tree_copy) {
  if (tree_copy == n) {
    if (iter == tree_size) {
      return AllGood();
    } else {
      if (GenByEdgeByTree(iter + 1, 0)) {
        return true;
      }
    }
    return false;
  }
  return GenerateNextEdge(iter, tree_copy, GenByEdgeByTree, iter, tree_copy + 1);
}

int main(int argc, char** argv) {
  random_device rd;
  mt19937 g(rd());

  begin_clock = clock();
  bool found_combination_without_solution = false;
  int iter_num = 0;

  NFreeTree::FreeTree(tree_size + 1);
  cerr << "total_amount=" << NFreeTree::total_amount << endl;

  while (!found_combination_without_solution) {
    cerr << "iter=" << iter_num << "; ";
    for (n = min_n; n <= max_n; ++n) {
      cerr << "n=" << n << ": ";
      if (found_combination_without_solution) {
        break;
      }

      pii_edges.clear();

      for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
          if (i != j) {
            pii_edges.push_back(make_pair(i, j));
          }
        }
      }

      bool is_regular = false;
      while (!is_regular) {
        shuffle(pii_edges.begin(), pii_edges.end(), g);
        for (int i = 0; i < n; ++i) {
          deg_in[i] = 0;
          deg_out[i] = 0;
        }
        for (int i = 0; i < (int) pii_edges.size(); ++i) {
          take_pair[i] = false;
          int v1, v2;
          v1 = pii_edges[i].first;
          v2 = pii_edges[i].second;
          if ((deg_out[v1] < tree_size) && (deg_in[v2] < tree_size)) {
            take_pair[i] = true;
            ++deg_out[v1];
            ++deg_in[v2];
          }
        }
        is_regular = true;
        for (int i = 0; i < n; ++i) {
          is_regular = is_regular && (deg_in[i] == tree_size) && (deg_out[i] == tree_size);
          if (!is_regular) {
            break;
          }
        }
      }

      for (int i = 0; i < n; ++i) {
        gr[0][i].clear();
        gr[1][i].clear();
      }
      for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
          edge_deg[i][j] = 0;
        }
      }

      for (int i = 0; i < (int) pii_edges.size(); ++i) {
        int v1, v2;
        v1 = pii_edges[i].first;
        v2 = pii_edges[i].second;
        if (take_pair[i]) {
          edge_deg[v1][v2] = 1;
        }
      }

      for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
          if (edge_deg[i][j] != 0) {
            gr[0][i].push_back(j);
            gr[1][j].push_back(i);
          }
        }
      }

      cerr << "tree#";
      for (cur_tree = 0; cur_tree < NFreeTree::total_amount - 1; ++cur_tree) {
        cerr << " " << cur_tree << ":";
        for (int cur_type = 0; cur_type < 2; ++cur_type) {
          v_part[0] = cur_type;
          for (int i = 0; i < tree_size; ++i) {
            v_part[i + 1] = 1 - v_part[NFreeTree::tree_database[cur_tree][i + 2]];
          }

          for (int i = 0; i < n; ++i) {
            for (int j = 0; j <= tree_size; ++j) {
              is_n[i][j] = false;
            }
          }

          for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
              used_edge[i][j] = false;
              used_copy[i][j] = false;
              neibed[i][j] = false;
            }
          }

          for (int i = 0; i < n; ++i) {
            neib[i][0] = i;
            neibed[i][i] = true;
          }

          cnt = 0;
          bool result = GenByTreeByEdge(1, 0);
          //bool result = GenByEdgeByTree(1, 0);

          if (cnt) {
            cerr << "_";
          }

          if (!cnt) {
            cerr << "Found combination without solution!" << endl;
            found_combination_without_solution = true;
            cerr << "iter#=" << iter_num << "; tree#=" << cur_tree << "; type#=" << cur_type << endl;
            cerr << "tree: ";
            for (int i = 0; i <= tree_size; ++i) {
              cerr << NFreeTree::tree_database[cur_tree][i + 1] << " ";
            }
            cerr << endl;
            cerr << "parts: ";
            for (int i = 0; i <= tree_size; ++i) {
              cerr << v_part[i] << " ";
            }
            cerr << endl;
            for (int i = 0; i < n; ++i) {
              cerr << i << ": ";
              for (int j = 0; j < n; ++j) {
                if (edge_deg[i][j] > 0) {
                  cerr << j << " ";
                }
              }
              cerr << endl;
            }
            cerr << endl;
            cerr << endl;
          }
        }
      }
    }
    ++iter_num;
    end_clock = clock();
    double elapsed_secs = double(end_clock - begin_clock) / CLOCKS_PER_SEC;
    begin_clock = end_clock;
    cerr << "time: " << elapsed_secs << "s" << endl;
  }

  end_clock = clock();
  double elapsed_secs = double(end_clock - begin_clock) / CLOCKS_PER_SEC;
  cerr << "time: " << elapsed_secs << "s" << endl;
  return 0;
}

