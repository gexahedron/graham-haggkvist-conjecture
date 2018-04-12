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

clock_t begin_clock, end_clock;
vector<pair<int, int>> pii_edges;
bool take_pair[maxn * maxn];
int deg_in[maxn];
int deg_out[maxn];

int edge_deg[maxn][maxn];
bool used_edge[maxn][maxn];
int whose_edge[maxn][maxn];

bool used_copy[maxn][maxn];
int copy_count[maxn];

int cnt;

vector<int> gr[2][maxn]; // 0 - from, 1 - to

int n;
const int tree_size = 3; // number of edges
const int min_n = tree_size + 2;//2 * tree_size + 1; // min_n >= 2 * tree_size + 1
const int max_n = min_n;
const int max_iter = 20;

int neib[maxn][tree_size + 1];
bool neibed[maxn][maxn];
//bool is_n[maxn][tree_size + 1];
//int v_part[maxn][tree_size + 1];
// TODO: fix all this tree_size + 1 to tree_size; don't mess with indices

int tree_indices[maxn];
//int tree_types[maxn];

int factor_deg[maxn][maxn];

bool GenEdge(int iter, int v) {
  if (iter == tree_size + 1) {
    if (v == n - 1) {
      /*bool regular_count = true;
      for (int v1 = 0; v1 < n; ++v1) {
        set<int> trees;
        for (int v2 = 0; v2 < n; ++v2) {
          if (used_edge[v1][v2]) {
            trees.insert(whose_edge[v1][v2]);
          }
          if (used_edge[v2][v1]) {
            trees.insert(whose_edge[v2][v1]);
          }
        }
        if (trees.size() != tree_size + 1) {
          regular_count = false;
          break;
        }
      }
      if (!regular_count) {
        return false;
      }*/
      ++cnt;
      return true; // use false for counting, true for detecting 0
    }
    else {
      if (GenEdge(1, v + 1)) {
        return true;
      }
    }
    return false;
  }

  const int v_in_tree = NFreeTree::tree_database[tree_indices[v]][iter - 1 + 2];
  const int cur_v = neib[v][v_in_tree];
  if (factor_deg[cur_v][iter] >= 2) {
    //cerr << "WUT" << endl;
    return false;
  }
  //const int dir = v_part[v][v_in_tree];

  for (int dir = 0; dir < 2; ++dir) {
    for (int i = 0; i < tree_size; ++i) {
      int u = gr[dir][cur_v][i];

      /*if (is_n[u][iter]) {
        continue;
      }*/
      if (factor_deg[u][iter] == 2) {
        continue;
      }

      if (factor_deg[u][iter] > 2) {
        cerr << "WTF" << endl;
      }

      int v1 = cur_v;
      int v2 = u;
      if (dir == 1) {
        swap(v1, v2);
      }
      if (used_edge[v1][v2]) {
        continue;
      }

      /*// TODO: add description what is_bad here
      bool is_bad = false;
      for (int j = 0; j < iter - 1; ++j) {
        is_bad = (u == neib[v][j]);
        if (is_bad) {
          break;
        }
      }
      if (is_bad) {
        continue;
      }*/

      if (neibed[v][u]) {
        continue;
      }

      ++factor_deg[v1][iter];
      ++factor_deg[v2][iter];
      //is_n[u][iter] = true;
      neib[v][iter] = u;
      neibed[v][u] = true;
      used_edge[v1][v2] = true;

      /*whose_edge[v1][v2] = v;
      const bool prev_used_copy_v1 = used_copy[v1][v];
      const bool prev_used_copy_v2 = used_copy[v2][v];
      if (!used_copy[v1][v]) {
          used_copy[v1][v] = true;
          ++copy_count[v1];
      }
      if (!used_copy[v2][v]) {
          used_copy[v2][v] = true;
          ++copy_count[v2];
      }*/

      //if (copy_count[v1] <= tree_size + 1 && copy_count[v2] <= tree_size + 1) {
        if (GenEdge(iter + 1, v)) {
          return true;
        }
      //}

      /*if (!prev_used_copy_v1) {
          used_copy[v1][v] = false;
          --copy_count[v1];
      }
      if (!prev_used_copy_v2) {
          used_copy[v2][v] = false;
          --copy_count[v2];
      }*/

      neibed[v][u] = false;
      used_edge[v1][v2] = false;
      //is_n[u][iter] = false;
      --factor_deg[v1][iter];
      --factor_deg[v2][iter];
    }
  }
  return false;
}

int main(int argc, char** argv) {
  random_device rd;
  mt19937 g(rd());//42);//rd());

  begin_clock = clock();
  bool found_combination_without_solution = false;
  int iter_num = 0;

  NFreeTree::FreeTree(tree_size + 1);
  cerr << "total_amount=" << NFreeTree::total_amount << endl;
  uniform_int_distribution<>dis(0, NFreeTree::total_amount - 1);
  uniform_int_distribution<>dis2(0, 1);

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

        for (int i = 0; i < n; ++i) {
          for (int j = 0; j < n; ++j) {
            edge_deg[i][j] = 0;
          }
        }

        for (int i = 0; i < (int) pii_edges.size(); ++i) {
          take_pair[i] = false;
          int v1, v2;
          v1 = pii_edges[i].first;
          v2 = pii_edges[i].second;
          if (/*edge_deg[v2][v1] == 0 &&*/ (deg_out[v1] < tree_size) && (deg_in[v2] < tree_size)) {
            take_pair[i] = true;
            edge_deg[v1][v2] = 1;
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

      /*for (int i = 0; i < n; ++i) {
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
      }*/

      for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
          if (edge_deg[i][j] != 0) {
            gr[0][i].push_back(j);
            gr[1][j].push_back(i);
          }
        }
      }

      cerr << "trees: ";
      for (int tree_idx = 0; tree_idx < n; ++tree_idx) {
        tree_indices[tree_idx] = dis(g);
        //const int cur_type = dis2(g);
        //tree_types[tree_idx] = cur_type;
        cerr << tree_indices[tree_idx] << " ";//"(" << cur_type << ") ";

        //v_part[tree_idx][0] = cur_type;
        for (int i = 0; i < tree_size; ++i) {
          //v_part[tree_idx][i + 1] = 1 - v_part[tree_idx][NFreeTree::tree_database[tree_indices[tree_idx]][i + 2]];
        }
      }
      cerr << endl;

      for (int i = 0; i < n; ++i) {
        for (int j = 0; j <= tree_size; ++j) {
          //is_n[i][j] = false;
          factor_deg[i][j] = 0;
        }
      }

      for (int i = 0; i < n; ++i) {
        copy_count[i] = 0;
        for (int j = 0; j < n; ++j) {
          used_edge[i][j] = false;
          whose_edge[i][j] = NONE;
          used_copy[i][j] = false;
          neibed[i][j] = false;
        }
      }

      for (int i = 0; i < n; ++i) {
        neib[i][0] = i;
      }

      cnt = 0;
      bool result = GenEdge(1, 0);

      if (!cnt) {
        cerr << "Found combination without solution!" << endl;
        found_combination_without_solution = true;
        cerr << "iter#=" << iter_num << endl;
        cerr << "trees: ";
        for (int i = 0; i < n; ++i) {
          cerr << tree_indices[i] << " ";//"(" << tree_types[i] << ") ";
        }
        cerr << endl;
        cerr << "trees:" << endl;
        for (int j = 0; j < n; ++j) {
          cerr << "#" << j << ": ";
          for (int i = 0; i <= tree_size; ++i) {
            cerr << NFreeTree::tree_database[tree_indices[j]][i + 1] << " ";
          }
          cerr << endl;
        }

        /*cerr << "parts:" << endl;
        for (int j = 0; j < n; ++j) {
          cerr << "#" << j << ": ";
          for (int i = 0; i <= tree_size; ++i) {
            cerr << v_part[j][i] << " ";
          }
          cerr << endl;
        }*/

        cerr << "graph:" << endl;
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

