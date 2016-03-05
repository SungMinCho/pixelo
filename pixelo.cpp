#include <stdio.h>

#define DEBUG(s)

enum flag { Yes = 0, No, Maybe};

flag board[100][100];
int rn, cn;
int rowhint[100][100], colhint[100][100];
int configN, configs[100][100]; // store first positions of each consecutive blocks. configs

inline int min(int a, int b){
  if(a < b) return a;
  return b;
}

void print() {
  int i, j;
  for(i = 1; i <= rn; i++) {
    for(j = 1; j <= cn; j++) {
      printf("%c", toChar(board[i][j]));
    }
    printf("\n");
  }
  DEBUG(printf("\n");)
}

void init() {
  int i, j;
  for(i = 1; i <= rn; i++) {
    for(j = 1; j <= cn; j++) {
      board[i][j] = Maybe;
    }
  }
}

void saveConfig(int n) {
  configN++;
  for(int i = 1; i <= n; i++)
    configs[configN][i] = configs[0][i];
}

int findFirstInRow(int row, int from, flag f) {
  for(int i = from; i <= cn; i++)
    if(board[row][i] == f) return i;
  return -1;
}

void findConfigsInRowAux(int row, int col, int hint) {
  if(hint > rowhint[row][0]) {
    saveConfig(rowhint[row][0]);
    return;
  }
  if(col > cn) {
    return;
  }
  int len = rowhint[row][hint];
  int firstNo = findFirstInRow(row, col, No);
  if(firstNo == -1) firstNo = cn+1;
  int maximumFirstPos = firstNo - len;
  if(maximumFirstPos < col) findConfigsInRowAux(row, firstNo+1, hint);
  int firstYes = findFirstInRow(row, col, Yes);
  if(firstYes != -1) maximumFirstPos = min(maximumFirstPos, firstYes);
  DEBUG(printf("row %d col %d maxfirst %d len %d firstno %d\n", row, col, maximumFirstPos, len, firstNo);)
  for(int i = col; i <= maximumFirstPos; i++) {
    configs[0][hint] = i;
    findConfigsInRowAux(row, i + len + 1, hint + 1);
  }
  if(firstYes == -1)
    findConfigsInRowAux(row, firstNo + 1, hint);
}

void findConfigsInRow(int row) {
  configN = 0;
  findConfigsInRowAux(row, 1, 1); 
}

// returns yes if changed something
bool tryFitRow(int row) {
  findConfigsInRow(row);
  if(configN == 0) return false; // this shouldn't happen...
  int h, c, represent;
  bool changed = false, unique;
  for(h = 1; h <= rowhint[row][0]; h++) {
    represent = configs[1][h];
    unique = true;
    for(c = 2; c <= configN; c++) {
      if(configs[c][h] != represent) {
        unique = false;
        break;
      }
    }

    if(unique) {
      int i, len = rowhint[row][h];
      for(i = 0; i < len; i++) {
        if(board[row][represent + i] == Maybe) { 
          board[row][represent + i] = Yes;
          DEBUG(printf("row put consecutive O\n");) DEBUG(print();)
          changed = true;
        }
      }
      
      if(board[row][represent + len] == Maybe) {
        board[row][represent + len] = No;
        DEBUG(printf("row put X after consecutive\n");) DEBUG(print();)
        changed = true;
      }
    }
  }
  return changed;
}

bool solve_row(int at) {
  bool alldone = true;
  int i;
  for(i = 1; i <= cn; i++) {
    if(board[at][i] == Maybe) {
      alldone = false;
      break;
    }
  }
  if(alldone) return false;

  if(rowhint[at][0] == 1) {
    // handle zero
    if(rowhint[at][1] == 0) {
      for(i = 1; i <= cn; i++)
        board[at][i] = No;
      return true;
    }
  }

  // all hints realized
  int yescnt = 0, yescntideal = 0;
  for(i = 1; i <= rowhint[at][0]; i++) yescntideal += rowhint[at][i];
  for(i = 1; i <= cn; i++)
    if(board[at][i] == Yes) yescnt++;
  if(yescnt == yescntideal) {
    bool changed = false;
    for(i = 1; i <= cn; i++) {
      if(board[at][i] == Maybe) {
        board[at][i] = No;
        DEBUG(printf("row fill x\n");) DEBUG(print();)
        changed = true;
      }
    }
    return changed;
  }

  return tryFitRow(at);
}

int findFirstInCol(int col, int from, flag f) {
  for(int i = from; i <= rn; i++)
    if(board[i][col] == f) return i;
  return -1;
}

void findConfigsInColAux(int col, int row, int hint) {
  if(hint > colhint[col][0]) {
    saveConfig(colhint[col][0]);
    return;
  }
  if(row > rn) {
    return;
  }
  int len = colhint[col][hint];
  int firstNo = findFirstInCol(col, row, No);
  if(firstNo == -1) firstNo = rn+1;
  int maximumFirstPos = firstNo - len;
  if(maximumFirstPos < row) findConfigsInColAux(col, firstNo+1, hint);
  int firstYes = findFirstInCol(col, row, Yes);
  if(firstYes != -1) maximumFirstPos = min(maximumFirstPos, firstYes);
  DEBUG(printf("col %d row %d maxfirst %d len %d firstno %d\n", col, row, maximumFirstPos, len, firstNo);)
  for(int i = row; i <= maximumFirstPos; i++) {
    configs[0][hint] = i;
    findConfigsInColAux(col, i + len + 1, hint + 1);
  }
  if(firstYes == -1)
    findConfigsInColAux(col, firstNo + 1, hint);
}

void findConfigsInCol(int col) {
  configN = 0;
  findConfigsInColAux(col, 1, 1);
}

// returns yes if changed something
bool tryFitCol(int col) {
  findConfigsInCol(col);
  if(configN == 0) return false; // this shouldn't happen...
  int h, c, represent;
  bool changed = false, unique;
  for(h = 1; h <= colhint[col][0]; h++) {
    represent = configs[1][h];
    unique = true;
    for(c = 2; c <= configN; c++) {
      if(configs[c][h] != represent) {
        unique = false;
        break;
      }
    }

    if(unique) {
      int i, len = colhint[col][h];
      for(i = 0; i < len; i++) {
        if(board[represent + i][col] == Maybe) { 
          board[represent + i][col] = Yes;
          DEBUG(printf("col fill consecutive O\n");) DEBUG(print();)
          changed = true;
        }
      }
      
      if(board[represent + len][col] == Maybe) {
        board[represent + len][col] = No;
        DEBUG(printf("col put X after consecutive\n");) DEBUG(print();) 
        changed = true;
      }
    }
  }
  return changed;
}

bool solve_col(int at) {
  bool alldone = true;
  int i;
  for(i = 1; i <= rn; i++) {
    if(board[i][at] == Maybe) {
      alldone = false;
      break;
    }
  }
  if(alldone) return false;

  if(colhint[at][0] == 1) {
    // handle zero
    if(colhint[at][1] == 0) {
      for(i = 1; i <= rn; i++)
        board[i][at] = No;
      return true;
    }
  }

  // all hints realized
  int yescnt = 0, yescntideal = 0;
  for(i = 1; i <= colhint[at][0]; i++) yescntideal += colhint[at][i];
  for(i = 1; i <= rn; i++)
    if(board[i][at] == Yes) yescnt++;
  if(yescnt == yescntideal) {
    bool changed = false;
    for(i = 1; i <= rn; i++) {
      if(board[i][at] == Maybe) {
        board[i][at] = No; 
        DEBUG(printf("col fill X\n");) DEBUG(print();) 
        changed = true;
      }
    }
    return changed;
  }

  return tryFitCol(at);
}

void solve() {
  bool changed;
  int i;
  do{
    changed = false;
    for(i = 1; i <= rn; i++)
      changed |= solve_row(i);

    for(i = 1; i <= cn; i++)
      changed |= solve_col(i);
  }while(changed);
}

char toChar(flag f) {
  if(f == Maybe) return '?';
  if(f == Yes) return 'O';
  return 'X';
}

int main() {
  scanf("%d %d", &rn, &cn);
  init();
  int i, j, c;
  for(i = 1; i <= rn; i++) {
    scanf("%d", &c);
    rowhint[i][0] = c;
    for(j = 1; j <= c; j++) {
      scanf("%d", &rowhint[i][j]);
    }
  }
  for(i = 1; i <= cn; i++) {
    scanf("%d", &c);
    colhint[i][0] = c;
    for(j = 1; j <= c; j++) {
      scanf("%d", &colhint[i][j]);
    }
  }
  solve();
  print();
  return 0;
}
