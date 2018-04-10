/*=====================================================================================*/
/* program: freetree.h                                                                 */
/* purpose: generating all free trees                                                  */
/* input  : n -- number of nodes                                                       */
/*          m -- max degree                                                            */
/*          lb, ub -- lower and upper bound on diameter                                */
/* output : listing of free trees in relex order                                       */
/* date   : September 1995, updated September 2000                                     */
/* programmers: Gang Li & Frank Ruskey                                                 */
/* algorithm: From the paper: G. Li and F. Ruskey, "The                                */
/*  Advantages of Forward Thinking in Generating Rooted and                            */
/*  Free Trees",  10th Annual ACM-SIAM Symposium on Discrete                           */
/*  Algorithms (SODA), (1999) S939-940.  See the web page at                           */
/*  http://webhome.cs.uvic.ca/~ruskey/Publications/RootedFreeTree/RootedFreeTree.html  */
/* more info: see                                                                      */
/*  http://theory.cs.uvic.ca/inf/tree/FreeTrees.html                                   */
/*=====================================================================================*/

namespace NFreeTree {

const int MAX_SIZE = 30; /* max size of the tree           */

int par[MAX_SIZE];       /* parent position of i           */
int L[MAX_SIZE];         /* level of node i                */
int k;                   /* max number of children         */
int chi[MAX_SIZE];       /* number of children of a node   */
int next_p[MAX_SIZE];    /* next good pos to add nodes     */
int r_chi[MAX_SIZE];     /* the rightmost child of node i  */
int ub;                  /* upper bound                    */
int num;                 /* number of trees                */

int N;  /* Corresponds to -n */
int K;  /*                -k */
int M;  /*                -m */
int U;  /*                -u */

short tree_database[30000000][MAX_SIZE];
static size_t total_amount = 0;

void UpdateL() {
  L[1] = 0;
  for (int i = 2; i <= N; ++i) {
    L[i] = L[par[i]] + 1;
  }
}

bool Good(int p, int h, int t) {
  if (p == 2 && K <= 2 && t == 0) {
    return true;
  }
  if (t == 1) {
    if ((2 * h >= K + 1) && (2 * h <= U + 1)) {
      if ((p - 1) * 2 >= N) {
        return true;
      } else if (p - h - 1 == 1) {
        if (par[p] > 2) {
          return true;
        }
      } else if ((p - h - 1 >= 2) && ((par[h + 2] > 2) || (par[h + 3] > 2))) {
        return true;
      }
    }
  } else if ((N - p >= h) && (2 * h >= K)) {
    if ((U == N - 1) && (N % 2 == 0)) {
      return (2 * h <= U + 1);
    } else {
      return(2 * h <= U);
    }
  }
  return false;
}

void GenTree(int p, int s, int cL, int h, int l, int n, int f, int g) {
  int hh, flag, entry, temp;
  if (p > n) {
    if (f == 0) {
      if (Good(p - 1, h, 0)) {
        GenTree(p, 2, p - 2, h, n, N, 1, 0);
      }
      if (Good(p - 1, h, 1)) {
        GenTree(p, 2, p - 3, h, n, N, 1, 1);
      }
    }
    else {
      UpdateL();
      tree_database[total_amount][0] = N;
      for (size_t i = 1; i <= static_cast<size_t>(N); ++i) {
        tree_database[total_amount][i] = par[i] - 1;
      }
      ++total_amount;
    }
  } else {
    if (cL == 0) {
      if (p < ub + 2) {
        par[p] = p - 1;
      } else {
        GenTree(p, p - 1, 1, h, l, n, f, g);
        return;
      }
    } else if (par[p - cL] < s) {
      par[p] = par[p - cL];
    } else {
      par[p] = cL + par[p - cL];
      if (g == 1) {
        if (((l - 1) * 2 < n) && (p - cL <= l) && (
          ((p - cL + 1 < l) &&  (par[p - cL + 1] == 2)
          && (p - cL + 2 <= l) && (par[p - cL + 2] == 2))   /* case 1 */
          || ((p - cL + 1 == l) && (par[p - cL + 1] == 2))  /* case 2 */
          || (p - cL + 1 > l)))                             /* case 3 */
        {
          s = par[p];
          cL = p - s;
          par[p] = par[par[p]];
        } else {
          if (par[p - cL] == 2) {
            par[p] = 1;
          }
        }
      }
    }
    if (s != 0 || p <= ub + 1) {
      chi[par[p]] = chi[par[p]] + 1;
      temp = r_chi[par[p]];
      r_chi[par[p]] = p;
      if (chi[par[p]] <= ((par[p] == 1) ? k : k - 1)) {
        if (chi[par[p]] < (par[p] == 1 ? k : k - 1)) {
          next_p[p] = par[p];
        } else {
          next_p[p] = next_p[par[p]];
        }
        GenTree(p + 1, s, cL, h, l, n, f, g);
      }
      chi[par[p]] = chi[par[p]] - 1;
      r_chi[par[p]] = temp;
    }
    if (s == 0 && 2 * (p - 2) < K) {
      return;
    }

    next_p[p] = next_p[par[p]];
    entry = next_p[p];
    flag = 0;
    hh = 1;
    while ((((f == 0) && (entry >= 2)) || ((f == 1) && (entry >= 1))) && (flag == 0)) {
      if (s == 0) {
        h = p - 2;
      }
      if (p <= l + h - g) {
        hh = 0;
      }
      if ((f == 0) || (hh == 1)) {
        //s = par[p];
        //par[p] = par[s];
        par[p] = entry;

        chi[entry] = chi[entry] + 1;
        temp = r_chi[par[p]];
        r_chi[par[p]] = p;
        if (chi[entry] >= (entry == 1 ? k : k - 1)) {
          next_p[p] = next_p[entry];
        }
        if (f == 0) {
          GenTree(p + 1, temp, p - temp, h, 0, N - h + 1, f, g);
        }
        else if (hh == 1) {
          GenTree(p + 1, temp, p - temp, h, l, n, f, g);
        }
        chi[entry] = chi[entry] - 1;
        r_chi[par[p]] = temp;
        entry = next_p[entry];
        next_p[p] = entry;
      } else {
        flag = 1;
      }
    }
    if (f == 0) {
      if (Good(p - 1, h, 0)) {
        GenTree(p, 2, p - 2, h, p - 1, N, 1, 0);
      }
      if (Good(p - 1, h, 1)) {
        GenTree(p, 2, p - 3, h, p - 1, N, 1, 1);
      }
    }
  }
} /* GenTree */

void ProcessInput(int n) {
  N = n;     // N is the number of nodes in the tree
  M = N - 1; // TODO: what is this?
  K = 2;     // TODO: what is this?
  U = N - 1; // U is an upper bound on the diameter of the tree
}

int FreeTree(int n) {
  num = 0;
  /* first set all the parameters */
  ProcessInput(n);
  if (K > U) {
    //printf("ERROR: lower bound > upper bound !!\n");
    return -1;
  }
  if (U > 1) {
    if (U < N) {
      ub = (U + 1) / 2;
    } else {
      ub = N / 2;
      U = N - 1;
    }
  } else {
    //printf("ERROR: upper bound is too small, should be >= 2\n");
    return -1;
  }

  if (M > 0) {
    k = M;
  } else {
    k = -1;
  }

  if (K <= 1) {
    //printf("ERROR: k is too small, should be >= 2\n");
    return -1;
  } else {
    for (int i = 1; i <= N; ++i) {
      chi[i] = 0;
    }

    par[1] = 0;
    par[2] = 1;
    next_p[1] = 0;
    next_p[2] = 1;
    chi[1] = 1;
    GenTree(3, 0, 0, ub, 0, N - ub + 1, 0, 0);
  }
  return 0;
}

} // end of namespace NFreeTree
