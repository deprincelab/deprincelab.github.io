#include<stdio.h>
#include<stdlib.h>
//#include<Python/Python.h>
void Solve();
int**rows,**cols,****blocks;
int***cand;
int**ncand;
long int total;
void CandidateList();
void SetElement(int i,int j,int val);
bool IsSolved();
void PrintPuzzle();
void CheckRows();
void CheckCols();
void CheckBlocks();
extern "C"{
    void solve_puzzle(int * input);
}
void solve_puzzle(int  * input) {
    int digit;
    rows = (int**)malloc(9*sizeof(int*));
    cols = (int**)malloc(9*sizeof(int*));
    for (int i = 0; i < 9; i++) {
        rows[i] = (int*)malloc(9*sizeof(int));
        cols[i] = (int*)malloc(9*sizeof(int));
    }
    blocks = (int****)malloc(3*sizeof(int***));
    for (int i = 0; i < 3; i++) {
        blocks[i] = (int***)malloc(3*sizeof(int**));
        for (int j = 0; j < 3; j++) {
            blocks[i][j] = (int**)malloc(3*sizeof(int*));
            for (int k = 0; k < 3; k++) {
                blocks[i][j][k] = (int*)malloc(3*sizeof(int));
            }
        }
    }
    cand = (int***)malloc(9*sizeof(int**));
    ncand = (int**)malloc(9*sizeof(int*));
    for (int i = 0; i < 9; i++) {
        ncand[i] = (int*)malloc(9*sizeof(int));
        cand[i] = (int**)malloc(9*sizeof(int*));
        for (int j = 0; j < 9; j++) {
            ncand[i][j] = 0;
            cand[i][j] = (int*)malloc(9*sizeof(int));
        }
    }
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            SetElement(i,j,input[i*9+j]);
        }
    }
    Solve();
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            input[i*9+j] = rows[i][j];
        }
    }
}
void CandidateList(){
    // loop over blocks
    for (int bi = 0; bi < 3; bi++) {
        int istart = bi * 3;
        for (int bj = 0; bj < 3; bj++) {
            // loop over i and j
            int jstart = bj * 3;
            for (int i = istart; i < istart+3; i++) {
                for (int j = jstart; j < jstart+3; j++) {
                    // zero candidate list
                    ncand[i][j] = 0;
                    if ( rows[i][j] != 0 ) {
                        // nothing to do
                    }else {
                        // check all numbers:
                        for (int n = 1; n < 10; n++) {
                            bool found = false;
                            // check this block:
                            for (int k = istart; k < istart + 3; k++) {
                                for (int l = jstart; l < jstart + 3; l++) {
                                    if ( i == k  && j == l ) continue;
                                    if ( rows[k][l] == n ) {
                                        found = true;
                                        break;
                                    }
                                }
                                if ( found ) break;
                            }
                            if ( found ) continue;
                            // check this row:
                            for (int l = 0; l < 9; l++) {
                                if ( l == j ) continue;
                                if ( rows[i][l] == n ) {
                                    found = true;
                                    break;
                                }
                            }
                            if ( found ) continue;
                            // check this column:
                            for (int k = 0; k < 9; k++) {
                                if ( k == i ) continue;
                                if ( rows[k][j] == n ) {
                                    found = true;
                                    break;
                                }
                            }
                            if ( found ) continue;
                            // add number to the candidate list
                            cand[i][j][ncand[i][j]++] = n;
                        }
                        // if only one candidate, fill.
                        if ( ncand[i][j] == 1 ) {
                            SetElement(i,j,cand[i][j][0]);
                            ncand[i][j] = 0;
                            return;
                        }
                    }
                }
            }
        }
    }

    // number claiming: if candidate appears in only 1 row or column of box, it claims that number for that
    // box ... remove from other lists
    int col[3],row[3];
    int tmp[9];
    int ntmp = 0;
    for (int bi = 0; bi < 3; bi++) {
        for (int bj = 0; bj < 3; bj++) {
            // for each of these numbers:
            for (int n = 1; n < 10; n++) {
                int nrow = 0;
                int ncol = 0;
                for (int i = 3*bi; i < 3*bi+3; i++) {
                    bool found = false;
                    for (int j = 3*bj; j < 3*bj+3; j++) {
                        for (int m = 0; m < ncand[i][j]; m++) {
                            if ( cand[i][j][m] == n ) {
                                col[ncol++] = i;
                                found = true;
                                break;
                            }
                        }
                        if (found) break;
                    }
                    found = false;
                    for (int j = 3*bj; j < 3*bj+3; j++) {
                        for (int m = 0; m < ncand[j][i]; m++) {
                            if ( cand[j][i][m] == n ) {
                                row[nrow++] = i;
                                found = true;
                                break;
                            }
                        }
                        if (found) break;
                    }
                }
                if ( ncol == 1 ) {
                    for (int j = 0; j < 9; j++) {
                        if ( j >= 3*bj && j < 3*bj+3) continue;
                        ntmp = 0;
                        for (int m = 0; m < ncand[col[0]][j]; m++) {
                            if ( cand[col[0]][j][m] == n ) continue;
                            tmp[ntmp++] = cand[col[0]][j][m];
                        }
                        if (ntmp < ncand[col[0]][j]) {
                            for (int m = 0; m < ntmp; m++) {
                                cand[col[0]][j][m] = tmp[m];
                                ncand[col[0]][j] = ntmp;
                            }
                        }
                        if ( ntmp == 1 ) {
                            SetElement(col[0],j,tmp[0]);
                            ncand[col[0]][j] = 0;
                            return;
                        }
                    }
                }
                if ( nrow == 1 ) {
                    for (int j = 0; j < 9; j++) {
                        if ( j >= 3*bj && j < 3*bj+3) continue;
                        ntmp = 0;
                        for (int m = 0; m < ncand[j][row[0]]; m++) {
                            if ( cand[j][row[0]][m] == n ) continue;
                            tmp[ntmp++] = cand[j][row[0]][m];
                        }
                        if (ntmp < ncand[j][row[0]]) {
                            for (int m = 0; m < ntmp; m++) {
                                cand[j][row[0]][m] = tmp[m];
                                ncand[j][row[0]] = ntmp;
                            }
                        }
                        if ( ntmp == 1 ) {
                            SetElement(j,row[0],tmp[0]);
                            ncand[j][row[0]] = 0;
                            return;
                        }
                    }
                }
            }
        }
    }

    // now that list is made, find pairs in same row/column/block with same list
    // columns:
    for (int i = 0; i < 9; i++) {
        int bi = ( i - i % 3 ) / 3;
        for (int j = 0; j < 9; j++) {
            int bj = ( j - j % 3 ) / 3;
            for (int k = 0; k < 9; k++) {
                if ( i == k ) continue;
                int bk = ( k - k % 3 ) / 3;
                // two candidates in each?
                if ( ncand[i][j] != ncand[k][j] || ncand[i][j] != 2 ) continue;
                bool elim = false;
                // same two candidates?
                if ( cand[i][j][0] == cand[k][j][0] && cand[i][j][1] == cand[k][j][1] ) elim = true;
                else if ( cand[i][j][0] == cand[k][j][1] && cand[i][j][1] == cand[k][j][0] ) elim = true;
                if ( !elim ) continue;
                // eliminate these candidates from other columns/blocks
                // columns l,j:
                int c1 = cand[i][j][0];
                int c2 = cand[i][j][1];
                for (int l = 0; l < 9; l++) {
                    if ( l == i || l == k ) continue;
                    ntmp = 0;
                    for (int m = 0; m < ncand[l][j]; m++) {
                        if ( cand[l][j][m] == c1 || cand[l][j][m] == c2 ) continue;
                        tmp[ntmp++] = cand[l][j][m];
                    }
                    if (ntmp < ncand[l][j]) {
                        for (int m = 0; m < ntmp; m++) {
                            cand[l][j][m] = tmp[m];
                            ncand[l][j] = ntmp;
                        }
                    }
                    if ( ntmp == 1 ) {
                        SetElement(l,j,tmp[0]);
                        ncand[l][j] = 0;
                        return;
                    }
                }
            }
        }
    }
    // rows:
    for (int i = 0; i < 9; i++) {
        int bi = ( i - i % 3 ) / 3;
        for (int j = 0; j < 9; j++) {
            int bj = ( j - j % 3 ) / 3;
            for (int k = 0; k < 9; k++) {
                if ( k == j ) continue;
                int bk = ( k - k % 3 ) / 3;
                // two candidates in each?
                if ( ncand[i][j] != ncand[i][k] || ncand[i][j] != 2 ) continue;
                bool elim = false;
                // same two candidates?
                if ( cand[i][j][0] == cand[i][k][0] && cand[i][j][1] == cand[i][k][1] ) elim = true;
                else if ( cand[i][j][0] == cand[i][k][1] && cand[i][j][1] == cand[i][k][0] ) elim = true;
                if ( !elim ) continue;
                // eliminate these candidates from other columns/blocks
                // columns:
                int c1 = cand[i][j][0];
                int c2 = cand[i][j][1];
                for (int l = 0; l < 9; l++) {
                    if ( l == j || l == k ) continue;
                    ntmp = 0;
                    for (int m = 0; m < ncand[i][l]; m++) {
                        if ( cand[i][l][m] == c1 || cand[i][l][m] == c2 ) continue;
                        tmp[ntmp++] = cand[i][l][m];
                    }
                    if (ntmp < ncand[i][l]) {
                        for (int m = 0; m < ntmp; m++) {
                            cand[i][l][m] = tmp[m];
                            ncand[i][l] = ntmp;
                        }
                    }
                    if ( ntmp == 1 ) {
                        SetElement(i,l,tmp[0]);
                        ncand[i][l] = 0;
                        return;
                    }
                }
            }
        }
    }
    // blocks:
    for (int bi = 0; bi < 3; bi++) {
        for (int bj = 0; bj < 3; bj++) {

            for (int ii = 0; ii < 3; ii++) {
                int i = 3*bi + ii;
                for (int jj = 0; jj < 3; jj++) {
                    int j = 3*bj + jj;
                    for (int kk = 0; kk < 3; kk++) {
                        int k = 3*bi + kk;
                        for (int ll = 0; ll < 3; ll++) {
                            int l = 3*bj + ll;
                            // skip ij >= kl
                            if ( i == k && j == l ) continue;
                            //if ( ii*3+jj > kk*3+ll ) continue;
                            // two candidates in each?
                            if ( ncand[i][j] != ncand[k][l] || ncand[i][j] != 2 ) continue;
                            bool elim = false;
                            // same two candidates?
                            if ( cand[i][j][0] == cand[k][l][0] && cand[i][j][1] == cand[k][l][1] ) elim = true;
                            else if ( cand[i][j][0] == cand[k][l][1] && cand[i][j][1] == cand[k][l][0] ) elim = true;
                            if ( !elim ) continue;
                            // eliminate c1 and c2 from all other candidate lists
                            int c1 = cand[i][j][0];
                            int c2 = cand[i][j][1];
                            // check all i,m
                            for (int m = 3*bj; m < 3*bj+3; m++) {
                                // skip i,j and k,l
                                if ( j == m ) continue;
                                if ( i == k && m == l ) continue;
                                ntmp = 0;
                                for (int n = 0; n < ncand[i][m]; n++) {
                                    if ( cand[i][m][n] == c1 || cand[i][m][n] == c2 ) continue;
                                    tmp[ntmp++] = cand[i][m][n];
                                }
                                if (ntmp < ncand[i][m]) {
                                    for (int n = 0; n < ntmp; n++) {
                                        cand[i][m][n] = tmp[n];
                                        ncand[i][m] = ntmp;
                                    }
                                }
                                if ( ntmp == 1 ) {
                                    SetElement(i,m,tmp[0]);
                                    ncand[i][m] = 0;
                                    return;
                                }
                            }
                            // check all m,j
                            for (int m = 3*bi; m < 3*bi+3; m++) {
                                // skip i,j and k,l
                                if ( i == m ) continue;
                                if ( m == k && j == l ) continue;
                                ntmp = 0;
                                for (int n = 0; n < ncand[m][j]; n++) {
                                    if ( cand[m][j][n] == c1 || cand[m][j][n] == c2 ) continue;
                                    tmp[ntmp++] = cand[m][j][n];
                                }
                                if (ntmp < ncand[m][j]) {
                                    for (int n = 0; n < ntmp; n++) {
                                        cand[m][j][n] = tmp[n];
                                        ncand[m][j] = ntmp;
                                    }
                                }
                                if ( ntmp == 1 ) {
                                    SetElement(m,j,tmp[0]);
                                    ncand[m][j] = 0;
                                    return;
                                }
                            }
                            // check all k,m
                            for (int m = 3*bj; m < 3*bj+3; m++) {
                                // skip i,j and k,l
                                if ( l == m ) continue;
                                if ( i == k && j == m ) continue;
                                ntmp = 0;
                                for (int n = 0; n < ncand[k][m]; n++) {
                                    if ( cand[k][m][n] == c1 || cand[k][m][n] == c2 ) continue;
                                    tmp[ntmp++] = cand[k][m][n];
                                }
                                if (ntmp < ncand[k][m]) {
                                    for (int n = 0; n < ntmp; n++) {
                                        cand[k][m][n] = tmp[n];
                                        ncand[k][m] = ntmp;
                                    }
                                }
                                if ( ntmp == 1 ) {
                                    SetElement(k,m,tmp[0]);
                                    ncand[k][m] = 0;
                                    return;
                                }
                            }
                            // check all m,l
                            for (int m = 3*bi; m < 3*bi+3; m++) {
                                // skip i,j and k,l
                                if ( k == m ) continue;
                                if ( m == i && l == j ) continue;
                                ntmp = 0;
                                for (int n = 0; n < ncand[m][l]; n++) {
                                    if ( cand[m][l][n] == c1 || cand[m][l][n] == c2 ) continue;
                                    tmp[ntmp++] = cand[m][l][n];
                                }
                                if (ntmp < ncand[m][l]) {
                                    for (int n = 0; n < ntmp; n++) {
                                        cand[m][l][n] = tmp[n];
                                        ncand[m][l] = ntmp;
                                    }
                                }
                                if ( ntmp == 1 ) {
                                    SetElement(m,l,tmp[0]);
                                    ncand[m][l] = 0;
                                    return;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

// again!!! (just in case any candidates were removed above)

    // number claiming: if candidate appears in only 1 row or column of box, it claims that number for that
    // box ... remove from other lists
    for (int bi = 0; bi < 3; bi++) {
        for (int bj = 0; bj < 3; bj++) {
            // for each of these numbers:
            for (int n = 1; n < 10; n++) {
                int nrow = 0;
                int ncol = 0;
                for (int i = 3*bi; i < 3*bi+3; i++) {
                    bool found = false;
                    for (int j = 3*bj; j < 3*bj+3; j++) {
                        for (int m = 0; m < ncand[i][j]; m++) {
                            if ( cand[i][j][m] == n ) {
                                col[ncol++] = i;
                                found = true;
                                break;
                            }
                        }
                        if (found) break;
                    }
                    found = false;
                    for (int j = 3*bj; j < 3*bj+3; j++) {
                        for (int m = 0; m < ncand[j][i]; m++) {
                            if ( cand[j][i][m] == n ) {
                                row[nrow++] = i;
                                found = true;
                                break;
                            }
                        }
                        if (found) break;
                    }
                }
                if ( ncol == 1 ) {
                    for (int j = 0; j < 9; j++) {
                        if ( j >= 3*bj && j < 3*bj+3) continue;
                        ntmp = 0;
                        for (int m = 0; m < ncand[col[0]][j]; m++) {
                            if ( cand[col[0]][j][m] == n ) continue;
                            tmp[ntmp++] = cand[col[0]][j][m];
                        }
                        if (ntmp < ncand[col[0]][j]) {
                            for (int m = 0; m < ntmp; m++) {
                                cand[col[0]][j][m] = tmp[m];
                                ncand[col[0]][j] = ntmp;
                            }
                        }
                        if ( ntmp == 1 ) {
/*for (int m = 0; m < ncand[col[0]][j]; m++) {
printf("cand %5i\n",cand[col[0]][j][m]);
}
printf(" block %5i %5i\n",bi,bj);
printf(" row %5i\n",col[0]);
printf(" column %5i\n",j);
printf(" value %5i\n",tmp[0]);
PrintPuzzle();
//exit(0);*/
                            SetElement(col[0],j,tmp[0]);
                            ncand[col[0]][j] = 0;
//PrintPuzzle();
                            return;
//return;
                        }
                    }
                }
                if ( nrow == 1 ) {
                    for (int j = 0; j < 9; j++) {
                        if ( j >= 3*bj && j < 3*bj+3) continue;
                        ntmp = 0;
                        for (int m = 0; m < ncand[j][row[0]]; m++) {
                            if ( cand[j][row[0]][m] == n ) continue;
                            tmp[ntmp++] = cand[j][row[0]][m];
                        }
                        if (ntmp < ncand[j][row[0]]) {
                            for (int m = 0; m < ntmp; m++) {
                                cand[j][row[0]][m] = tmp[m];
                                ncand[j][row[0]] = ntmp;
                            }
                        }
                        if ( ntmp == 1 ) {
                            SetElement(j,row[0],tmp[0]);
                            ncand[j][row[0]] = 0;
                            return;
                        }
                    }
                }
            }
        }
    }

    // now that list is made, find pairs in same row/column/block with same list
    // columns:
    for (int i = 0; i < 9; i++) {
        int bi = ( i - i % 3 ) / 3;
        for (int j = 0; j < 9; j++) {
            int bj = ( j - j % 3 ) / 3;
            for (int k = 0; k < 9; k++) {
                if ( i == k ) continue;
                int bk = ( k - k % 3 ) / 3;
                // two candidates in each?
                if ( ncand[i][j] != ncand[k][j] || ncand[i][j] != 2 ) continue;
                bool elim = false;
                // same two candidates?
                if ( cand[i][j][0] == cand[k][j][0] && cand[i][j][1] == cand[k][j][1] ) elim = true;
                else if ( cand[i][j][0] == cand[k][j][1] && cand[i][j][1] == cand[k][j][0] ) elim = true;
                if ( !elim ) continue;
                // eliminate these candidates from other columns/blocks
                // columns l,j:
                int c1 = cand[i][j][0];
                int c2 = cand[i][j][1];
                for (int l = 0; l < 9; l++) {
                    if ( l == i || l == k ) continue;
                    ntmp = 0;
                    for (int m = 0; m < ncand[l][j]; m++) {
                        if ( cand[l][j][m] == c1 || cand[l][j][m] == c2 ) continue;
                        tmp[ntmp++] = cand[l][j][m];
                    }
                    if (ntmp < ncand[l][j]) {
                        for (int m = 0; m < ntmp; m++) {
                            cand[l][j][m] = tmp[m];
                            ncand[l][j] = ntmp;
                        }
                    }
                    if ( ntmp == 1 ) {
                        SetElement(l,j,tmp[0]);
                        ncand[l][j] = 0;
                        return;
                    }
                }
            }
        }
    }
    // rows:
    for (int i = 0; i < 9; i++) {
        int bi = ( i - i % 3 ) / 3;
        for (int j = 0; j < 9; j++) {
            int bj = ( j - j % 3 ) / 3;
            for (int k = 0; k < 9; k++) {
                if ( k == j ) continue;
                int bk = ( k - k % 3 ) / 3;
                // two candidates in each?
                if ( ncand[i][j] != ncand[i][k] || ncand[i][j] != 2 ) continue;
                bool elim = false;
                // same two candidates?
                if ( cand[i][j][0] == cand[i][k][0] && cand[i][j][1] == cand[i][k][1] ) elim = true;
                else if ( cand[i][j][0] == cand[i][k][1] && cand[i][j][1] == cand[i][k][0] ) elim = true;
                if ( !elim ) continue;
                // eliminate these candidates from other columns/blocks
                // columns:
                int c1 = cand[i][j][0];
                int c2 = cand[i][j][1];
                for (int l = 0; l < 9; l++) {
                    if ( l == j || l == k ) continue;
                    ntmp = 0;
                    for (int m = 0; m < ncand[i][l]; m++) {
                        if ( cand[i][l][m] == c1 || cand[i][l][m] == c2 ) continue;
                        tmp[ntmp++] = cand[i][l][m];
                    }
                    if (ntmp < ncand[i][l]) {
                        for (int m = 0; m < ntmp; m++) {
                            cand[i][l][m] = tmp[m];
                            ncand[i][l] = ntmp;
                        }
                    }
                    if ( ntmp == 1 ) {
                        SetElement(i,l,tmp[0]);
                        ncand[i][l] = 0;
                        return;
                    }
                }
            }
        }
    }
    // blocks:
    for (int bi = 0; bi < 3; bi++) {
        for (int bj = 0; bj < 3; bj++) {

            for (int ii = 0; ii < 3; ii++) {
                int i = 3*bi + ii;
                for (int jj = 0; jj < 3; jj++) {
                    int j = 3*bj + jj;
                    for (int kk = 0; kk < 3; kk++) {
                        int k = 3*bi + kk;
                        for (int ll = 0; ll < 3; ll++) {
                            int l = 3*bj + ll;
                            // skip ij >= kl
                            if ( i == k && j == l ) continue;
                            //if ( ii*3+jj > kk*3+ll ) continue;
                            // two candidates in each?
                            if ( ncand[i][j] != ncand[k][l] || ncand[i][j] != 2 ) continue;
                            bool elim = false;
                            // same two candidates?
                            if ( cand[i][j][0] == cand[k][l][0] && cand[i][j][1] == cand[k][l][1] ) elim = true;
                            else if ( cand[i][j][0] == cand[k][l][1] && cand[i][j][1] == cand[k][l][0] ) elim = true;
                            if ( !elim ) continue;
                            // eliminate c1 and c2 from all other candidate lists
                            int c1 = cand[i][j][0];
                            int c2 = cand[i][j][1];
                            // check all i,m
                            for (int m = 3*bj; m < 3*bj+3; m++) {
                                // skip i,j and k,l
                                if ( j == m ) continue;
                                if ( i == k && m == l ) continue;
                                ntmp = 0;
                                for (int n = 0; n < ncand[i][m]; n++) {
                                    if ( cand[i][m][n] == c1 || cand[i][m][n] == c2 ) continue;
                                    tmp[ntmp++] = cand[i][m][n];
                                }
                                if (ntmp < ncand[i][m]) {
                                    for (int n = 0; n < ntmp; n++) {
                                        cand[i][m][n] = tmp[n];
                                        ncand[i][m] = ntmp;
                                    }
                                }
                                if ( ntmp == 1 ) {
                                    SetElement(i,m,tmp[0]);
                                    ncand[i][m] = 0;
                                    return;
                                }
                            }
                            // check all m,j
                            for (int m = 3*bi; m < 3*bi+3; m++) {
                                // skip i,j and k,l
                                if ( i == m ) continue;
                                if ( m == k && j == l ) continue;
                                ntmp = 0;
                                for (int n = 0; n < ncand[m][j]; n++) {
                                    if ( cand[m][j][n] == c1 || cand[m][j][n] == c2 ) continue;
                                    tmp[ntmp++] = cand[m][j][n];
                                }
                                if (ntmp < ncand[m][j]) {
                                    for (int n = 0; n < ntmp; n++) {
                                        cand[m][j][n] = tmp[n];
                                        ncand[m][j] = ntmp;
                                    }
                                }
                                if ( ntmp == 1 ) {
                                    SetElement(m,j,tmp[0]);
                                    ncand[m][j] = 0;
                                    return;
                                }
                            }
                            // check all k,m
                            for (int m = 3*bj; m < 3*bj+3; m++) {
                                // skip i,j and k,l
                                if ( l == m ) continue;
                                if ( i == k && j == m ) continue;
                                ntmp = 0;
                                for (int n = 0; n < ncand[k][m]; n++) {
                                    if ( cand[k][m][n] == c1 || cand[k][m][n] == c2 ) continue;
                                    tmp[ntmp++] = cand[k][m][n];
                                }
                                if (ntmp < ncand[k][m]) {
                                    for (int n = 0; n < ntmp; n++) {
                                        cand[k][m][n] = tmp[n];
                                        ncand[k][m] = ntmp;
                                    }
                                }
                                if ( ntmp == 1 ) {
                                    SetElement(k,m,tmp[0]);
                                    ncand[k][m] = 0;
                                    return;
                                }
                            }
                            // check all m,l
                            for (int m = 3*bi; m < 3*bi+3; m++) {
                                // skip i,j and k,l
                                if ( k == m ) continue;
                                if ( m == i && l == j ) continue;
                                ntmp = 0;
                                for (int n = 0; n < ncand[m][l]; n++) {
                                    if ( cand[m][l][n] == c1 || cand[m][l][n] == c2 ) continue;
                                    tmp[ntmp++] = cand[m][l][n];
                                }
                                if (ntmp < ncand[m][l]) {
                                    for (int n = 0; n < ntmp; n++) {
                                        cand[m][l][n] = tmp[n];
                                        ncand[m][l] = ntmp;
                                    }
                                }
                                if ( ntmp == 1 ) {
                                    SetElement(m,l,tmp[0]);
                                    ncand[m][l] = 0;
                                    return;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    // single-square candidates.  a candidate number appears only once in a row/column/block
    // check columns:
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) tmp[j] = 0;
        for (int j = 0; j < 9; j++) {
            for (int k = 0; k < ncand[i][j]; k++) {
                tmp[cand[i][j][k]-1]++;
            }
        }
        for (int k = 0; k < 9; k++) {
            if (tmp[k] != 1) continue;
            for (int j = 0; j < 9; j++) {
                for (int l = 0; l < ncand[i][j]; l++) {
                    if ( k + 1 == cand[i][j][l] ) {
                       SetElement(i,j,k+1);
                       ncand[i][j] = 0;
                       return;
                    }
                }
            }
        }
    }
    // check rows:
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) tmp[j] = 0;
        for (int j = 0; j < 9; j++) {
            for (int k = 0; k < ncand[j][i]; k++) {
                tmp[cand[j][i][k]-1]++;
            }
        }
        for (int k = 0; k < 9; k++) {
            if (tmp[k] != 1) continue;
            for (int j = 0; j < 9; j++) {
                for (int l = 0; l < ncand[j][i]; l++) {
                    if ( k + 1 == cand[j][i][l] ) {
                       SetElement(j,i,k+1);
                       ncand[j][i] = 0;
                       return;
                    }
                }
            }
        }
    }
    // blocks
    for (int bi = 0; bi < 3; bi++) {
        for (int bj = 0; bj < 3; bj++) {
            for (int i = 0; i < 9; i++) tmp[i] = 0;
            for (int i = 3*bi; i < 3*bi+3; i++) {
                for (int j = 3*bj; j < 3*bj+3; j++) {
                    for (int k = 0; k < ncand[i][j]; k++) {
                        tmp[cand[i][j][k]-1]++;
                    }
                }
            }
            for (int k = 0; k < 9; k++) {
                if (tmp[k] != 1) continue;
                for (int i = 3*bi; i < 3*bi+3; i++) {
                    for (int j = 3*bj; j < 3*bj+3; j++) {
                        for (int l = 0; l < ncand[i][j]; l++) {
                            if ( k + 1 == cand[i][j][l] ) {
                                SetElement(i,j,k+1);
                                ncand[i][j] = 0;
                                return;
                            }
                        }
                    }
                }
            }
        }
    }

    // TODO: triples

    // check one last time ... are any only 1 candidate?
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if ( ncand[i][j] == 1 ) {
               SetElement(i,j,cand[i][j][0]);
            }
        }
    }
}
void SetElement(int i,int j,int val){
    rows[i][j] = val;
    cols[j][i] = val;
    int imod = i % 3;
    int blocki = ( i - imod ) / 3;
    int jmod = j % 3;
    int blockj = ( j - jmod ) / 3;
    blocks[blocki][blockj][imod][jmod] = val;
}
void PrintPuzzle(){
    printf("\n");
    for (int i = 0; i < 34; i++) printf("-");
    printf("\n");
    for (int i = 0; i < 9; i++){
        printf("|");
        for (int j = 0; j < 9; j++){
            if (rows[i][j] != 0) printf("%3i",rows[i][j]);
            else                 printf("  _",rows[i][j]);
            if ( j % 3 == 2 ) printf(" |");
        }
        printf("\n");
        if ( i % 3 == 2 ) {
            for (int j = 0; j < 34; j++) printf("-");
            printf("\n");
        }
    }
    printf("\n");
}
void Solve(){
    bool solved = false;
    int iter = 0;
    do{
        //CheckRows();
        //CheckCols();
        //CheckBlocks();
        CandidateList();
        solved = IsSolved();
        iter++;
    }while(!solved && iter < 100);

    if (solved) {
         //printf("solved!\n");
         //PrintPuzzle();
    }
    else {
        //printf("not solved...\n");
        //PrintPuzzle();
//exit(0);
    }
}
bool IsSolved(){
    //for (int j = 0; j < 3; j++) {
    //    if ( rows[0][j] == 0 ) return false;
    //}

    // check for duplicates
    // rows:
    int countr[9];
    int countc[9];
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) countr[j] = countc[j] = 0;
        for (int j = 0; j < 9; j++) {
            if (rows[i][j] > 0)
                countc[rows[i][j]-1]++;
            if (rows[j][i] > 0)
                countr[rows[j][i]-1]++;
        }
        int count = 0;
        for (int j = 0; j < 9; j++) {
            //if ( countc[j] > 1) {printf("very bad things.\n",i,j);PrintPuzzle(); exit(0);}//return false;
            //if ( countr[j] > 1) {printf("very bad things.\n",i,j);PrintPuzzle(); exit(0);}//return false;
        }
    }
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if ( rows[i][j] == 0 ) return false;
        }
    }

    return true;
}
// for each number,go through each each row.  if the number is
// missing from only a single row, add it.
void CheckRows(){
    // loop over rows:
    for (int i = 0; i < 9; i++) {

        // check each number:
        for (int n = 1; n < 10; n++) {

            // check which columns are empty
            bool isempty[9];
            for (int j = 0; j < 9; j++) {
                if ( rows[i][j] == 0 ) isempty[j] = true;
                else                   isempty[j] = false;
            }
            int npos = 0;
            for (int j = 0; j < 9; j++) npos += isempty[j];

            bool found = false;
            for (int j = 0; j < 9; j++) {
                if ( rows[i][j] == n ) {
                    found = true;
                    break;
                }
            }
            // if n is in this row, move on
            if ( found ) continue;

            // if there is only one possible place to put n, do so
            if ( npos == 1 ) {
                for (int j = 0; j < 9; j++) {
                    if ( isempty[j] ) {
                        SetElement(i,j,n);
                        isempty[j] = false;
                        npos--;
                        //printf("checkrow(1) %5i %5i %5i\n",i,j,n);
                    }
                }
                // move on
                continue;
            }

            // try to eliminate possible positions by checking columns
            for (int j = 0; j < 9; j++) {
                if ( !isempty[j] ) continue;
                for (int k = 0; k < 9; k++) {
                    if ( rows[k][j] == n ) {
                        isempty[j] = false;
                        npos--;
                        break;
                    }
                }
            }
            if ( npos == 1 ) {
                for (int j = 0; j < 9; j++) {
                    if ( isempty[j] ) {
                        SetElement(i,j,n);
                        isempty[j] = false;
                        //printf("checkrow(2) %5i %5i %5i\n",i,j,n);
                    }
                }
                // move on
                continue;
            }

            // try to eliminate possible positions by checking block
            int imod = i % 3;
            int blocki = ( i - imod ) / 3;
            for (int j = 0; j < 9; j++) {
                if ( !isempty[j] ) continue;
                int jmod = j % 3;
                int blockj = ( j - jmod ) / 3;
                for (int ii = 0; ii < 3; ii++) {
                    for (int jj = 0; jj < 3; jj++) {
                        if ( blocks[blocki][blockj][ii][jj] == n ) {
                            isempty[j] = false;
                            npos--;
                            break;
                        }
                    }
                }
            }
            if ( npos == 1 ) {
                for (int j = 0; j < 9; j++) {
                    if ( isempty[j] ) {
                        SetElement(i,j,n);
                        isempty[j] = false;
                        //printf("checkrow(3) %5i %5i %5i\n",i,j,n);
                    }
                }
                // move on
                continue;
            }
        }
    }
}
void CheckCols(){
    // loop over columns:
    for (int j = 0; j < 9; j++) {

        // check each number:
        for (int n = 1; n < 10; n++) {

            // check which rows are empty
            bool isempty[9];
            for (int i = 0; i < 9; i++) {
                if ( rows[i][j] == 0 ) isempty[i] = true;
                else                   isempty[i] = false;
            }
            int npos = 0;
            for (int i = 0; i < 9; i++) npos += isempty[i];

            bool found = false;
            for (int i = 0; i < 9; i++) {
                if ( rows[i][j] == n ) {
                    found = true;
                    break;
                }
            }
            // if n is in this column, move on
            if ( found ) continue;

            // if there is only one possible place to put n, do so
            if ( npos == 1 ) {
                for (int i = 0; i < 9; i++) {
                    if ( isempty[i] ) {
                        SetElement(i,j,n);
                        isempty[i] = false;
                        npos--;
                        //printf("checkcol(1) %5i %5i %5i\n",i,j,n);
                    }
                }
                // move on
                continue;
            }

            // try to eliminate possible positions by checking rows
            for (int i = 0; i < 9; i++) {
                if ( !isempty[i] ) continue;
                for (int k = 0; k < 9; k++) {
                    if ( rows[i][k] == n ) {
                        isempty[i] = false;
                        npos--;
                        break;
                    }
                }
            }
            if ( npos == 1 ) {
                for (int i = 0; i < 9; i++) {
                    if ( isempty[i] ) {
                        SetElement(i,j,n);
                        isempty[i] = false;
                        //printf("checkcol(2) %5i %5i %5i\n",i,j,n);
                    }
                }
                // move on
                continue;
            }

            // try to eliminate possible positions by checking block
            int jmod = j % 3;
            int blockj = ( j - jmod ) / 3;
            for (int i = 0; i < 9; i++) {
                if ( !isempty[i] ) continue;
                int imod = i % 3;
                int blocki = ( i - imod ) / 3;
                for (int ii = 0; ii < 3; ii++) {
                    for (int jj = 0; jj < 3; jj++) {
                        if ( blocks[blocki][blockj][ii][jj] == n ) {
                            isempty[i] = false;
                            npos--;
                            break;
                        }
                    }
                }
            }
            if ( npos == 1 ) {
                for (int i = 0; i < 9; i++) {
                    if ( isempty[i] ) {
                        SetElement(i,j,n);
                        isempty[i] = false;
                        //printf("checkcol(3) %5i %5i %5i\n",i,j,n);
                    }
                }
                // move on
                continue;
            }

        }
    }
}
void CheckBlocks(){
  // for each block:
  for (int blocki = 0; blocki < 3; blocki++) {
      for (int blockj = 0; blockj < 3; blockj++) {
          // for each possible number:
          for (int n = 1; n < 10; n++) {
              // check if this number is missing
              bool found = false;
              for (int i = 0; i < 3; i++) {
                  for (int j = 0; j < 3; j++) {
                      if ( blocks[blocki][blockj][i][j] == n ) {
                          found = true;
                          break;
                      }
                  }
                  if ( found ) break;
              }
              // if the number is missing, check the rows and cols to 
              // see if there is one unique position to put it in
              if ( found ) continue;

              int starti = blocki * 3;
              int startj = blockj * 3;

              // check rows:
              int nposi = 0;
              int posi[3];
              for (int i = starti; i < starti + 3; i++) {
                  bool found = false;
                  for (int j = 0; j < 9; j++) {
                      if ( rows[i][j] == n ) {
                          found = true;
                          break;
                      }
                  }
                  // if n isn't in this row, add to list of possibilities
                  if ( !found ) {
                      posi[nposi++] = i;
                  }
              }
              // check cols:
              int nposj = 0;
              int posj[3];
              for (int j = startj; j < startj + 3; j++) {
                  bool found = false;
                  for (int i = 0; i < 9; i++) {
                      if ( rows[i][j] == n ) {
                          found = true;
                          break;
                      }
                  }
                  // if n isn't in this row, add to list of possibilities
                  if ( !found ) {
                      posj[nposj++] = j;
                  }
              }
              // if only one possibility, put it there.
              if ( nposi == 1 && nposj == 1 ) {
                  //printf("checkblock  %5i %5i %5i\n",posi[0],posj[0],n);fflush(stdout);
                  SetElement(posi[0],posj[0],n);
              }
              // TODO: if one possible row, multiple columns
              // TODO: if one possible colum, multiple rows
          }

      }
  }
}
