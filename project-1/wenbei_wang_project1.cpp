#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <string>
#include <algorithm>

using namespace std;

class state{
    private:
        int fscore;
        int gscore;
        int currentRow;
        vector<int> row;
        int d;
    public:
        state(int drones, int row, int _fscore, int _gscore, vector<int> _row):fscore(_fscore), gscore(_gscore),currentRow(row),row(_row),d(drones){}
        int getRow () { return currentRow;}
        int getD() {return d;};
        int getGscore(){return gscore;}
        vector<int> getRowV() {return row;};
        bool operator< (const state &other) const {
            if (fscore == other.fscore) return currentRow < other.currentRow;
		    return fscore < other.fscore;
	    }


};
bool greaterThan (int x, int y){
    return x > y;
}

// for dsf the h function
int** getScore(vector<vector<int> > board, const int n){
    int** score = new int*[n];
    // initialize the empty board
    for (int i = 0; i < n; i++){
        score[i] = new int[n];
        for (int j = 0; j < n; j++){
            score[i][j] = 0;
        }
    }

    vector<int> rowMax(n,0);
    vector<int> rowSecondMax(n,0);
    vector<int> indexMax(n,-1);

    // for each row, find the largest and second largest packet
    // and record the number and the position
    for (int i = 0; i < n; i++){
        for (int j = 0; j < n; j++){
            if (board[i][j] >= rowMax[i]){
                rowSecondMax[i] = rowMax[i];
                rowMax[i] = board[i][j];
                indexMax[i] = j;
            }
            else if (board[i][j] > rowSecondMax[i]){
                rowSecondMax[i] = board[i][j];
            }
        }
    }

    for (int i = 0; i < n; i++){
        for (int j = 0; j < n; j++){
            // for score[i][j]
            // count from row i+1 to n-1 the max or the second max
            for (int z = i+1; z < n; z++){
                if (indexMax[z] != j){
                    score[i][j] += rowMax[z];
                }
                else{
                    score[i][j] += rowSecondMax[z];
                }
            }
        }
    }
    return score;
}

// the hscore for astar
std::vector<std::vector<int> > hscoreAstar(std::vector<std::vector<int> >& board, int d) {
    std::vector<std::vector<int> > result;
    std::vector<int> rowScores(board.size(),0);
    
    for (int i = 0;i < board.size();i++){
        int max = 0;
        for (int j = 0; j < board[i].size();j++){
            if (board[i][j] > max) max = board[i][j]; 
        }
        rowScores[i] = max;
    }


    for (int i = 0; i < board.size();i++){
        vector <int> eachRow;
        vector<int> temp(rowScores.begin()+i+1, rowScores.end());
        sort(temp.begin(),temp.end(),greaterThan);

        if (i < board.size() - d){
            for (int j = 0; j <= d; j++){
                // sum up the first n-i-1 items
                int accu = 0;
                for (int z = 0; z < j;z++){
                    accu += temp[z];
                }
                eachRow.push_back(accu);
            }
        }
        else{
            for (int j = 0; j <= d - (i - (board.size() - d - 1)); j++){
                // sum up the first n-i-1 items
                int accu = 0;
                for (int z = 0; z < j;z++){
                    accu += temp[z];
                }
                eachRow.push_back(accu);
            }
        }
        result.push_back(eachRow);
    }
    return result;
}

// given the position row col, check if conflict with rows
bool isLegal(int row, int col, vector<int> rowV, int n){
    if (row >= n || col >= n) return false;
    for (int i = 0; i < rowV.size();i++){
        if (rowV[i] != -1 && i == row) return false;
        if (rowV[i] == col) return false;
        if (rowV[i] != -1 && i + rowV[i] == row + col) return false;
        if (rowV[i] != -1 && (row - col + n - 1) == (i - rowV[i] + n - 1)) return false;
    }
    return true;
}


int dfSearch(const int n, int d, const int p, vector<vector<int> > board, vector<int> row, vector<bool>& col, vector<bool>& dia, vector<bool>& revdia){
    bool backtrace = false,found = false;
    int pac = d, max = 0, currentRow = 0, currentCol = 0, currentPack = 0;

    if (!d) return 0;
    int** score = getScore(board, n);
    while (true) {
        found = false,backtrace = false;

        while (currentCol < n){
            if ( !col[currentCol] && !dia[currentCol + currentRow] && !revdia[currentRow - currentCol + n - 1]){
                if (currentCol < n && currentPack + board[currentRow][currentCol] + score[currentRow][currentCol] > max){
                    currentPack += board[currentRow][currentCol];
                    // set the column, diagonal and reverse diagonal to be occupied
                    col[currentCol] = 1, dia [currentCol + currentRow] = 1, revdia[currentRow - currentCol + n - 1] = 1;
                    row[currentRow] = currentCol;
                    d --;
                    found = true;
                    break;
                }
            }
            currentCol++;
        }

        if (found) {
            if (d == 0){
                if (currentPack > max) max = currentPack;
                if (max == p) return p;

                int tempCol = row[currentRow];
                col[tempCol] = 0;
                dia[tempCol + currentRow] = 0;
                revdia[currentRow - tempCol + n - 1] = 0;
                currentPack -= board[currentRow][tempCol];
                d++;
                row[currentRow] = -1;                    
                if (currentRow == n-1) backtrace = true;
                else if (currentCol + 1 < n) currentCol ++;
                else {
                    currentRow++;
                    currentCol = 0;
                }
            } 
            else {
                currentRow ++;
                currentCol = 0;
            }


        }
        else {
            // if no place is found
            // if the number of rows left is smaller than the number of drones
            // this means we can no longer place all the drones, so no need to cont, backtrace
            if (n - currentRow - 1 <  d) backtrace = true;
            // or it could be don't have to place in this row
            else {
                row[currentRow] = -1;
                currentRow++;
                currentCol = 0;
            }
        }

        // need to backtrack
        if (backtrace) {
            
            row[currentRow] = -1;
            // if still has more columns, start with the same row but next col
            currentCol++;
            // if it reaches the last columns, go back to the last row
            if (currentCol >= n){
                // need a while loop to decide which row to go
                currentRow --;
                if (currentRow < 0) return max;
                // or we can test whether the currentCol is equal to d+1
                d++;
                int contback = true;
                while (row[currentRow] == -1 || row[currentRow] == n-1){
                    if (row[currentRow] == n-1) {
                        d++;
                        if (d > pac){
                            d = pac;
                            break;
                        }
                        row[currentRow] = -1;
                        col[n-1] = 0;
                        dia[n-1 + currentRow] = 0;
                        revdia[currentRow] = 0;
                        currentPack -= board[currentRow][n-1];
                    }
                    currentRow --;
                    if (currentRow < 0 ) return max;
                }  
                if (contback){
                    int tempCol = row[currentRow];
                    currentCol = row[currentRow]+1;
                    col[tempCol] = 0;
                    dia[tempCol + currentRow] = 0;
                    revdia[currentRow - tempCol + n - 1] = 0;
                    row[currentRow] = -1;
                    currentPack -= board[currentRow][tempCol];
                }
            }
        }
    }
    return max;
}


// currentRow = 3: regardless of how to place row 3
// d = 2: there will be 2 drones starting next line
int astarSearch(int n, int d, int p, vector<vector<int> >& board){
    priority_queue<state> q;
    vector<int> _row(n,-1);
    state head(d, 0, 0 , 0, _row);
    q.push(head);
    int max = 0;
    int lastMax = 0;

    vector<vector<int> > hscores = hscoreAstar(board,d);
  
    while (!q.empty()){
        state head = q.top();
        q.pop();
        bool found = false;
        // if the number of drones remaining is 0
        // return the number
        vector<int> rowV = head.getRowV();
        int row = head.getRow();
        if (head.getD() == 0) {
            return head.getGscore();
        }
        // if decide to place and still smaller, then continue
        int newgscore = head.getGscore();
        int newHscore = 0;

        // fill the rowth row
        for (int i = 0; i < n;i++) {
            rowV = head.getRowV();
            newHscore = 0;
            newgscore = head.getGscore();
            if (isLegal(row, i, rowV, n)){
                if (row+1 < n && head.getD()-1 > 0) {
                    newHscore = hscores[row][head.getD()-1];
                }
                else newHscore = 0;
                newgscore += board[row][i];
                if (newgscore > max) {
                    max = newgscore;
                    max = lastMax;
                }
                rowV[row] = i;
                if (n - row >= head.getD()){ 
                    state newstate(head.getD()-1, row+1, newHscore + newgscore, newgscore, rowV);
                    q.push(newstate);
                    found = true;
                }
            }
        }
        // if not found, either no fit place or we don't place in this line
        if (row+1 < n && n - row - 1 >= head.getD()){
            rowV[row] = -1;
            newHscore = hscores[row][head.getD()];
            newgscore = head.getGscore();
            if (newHscore + newgscore > max){
                state newstate(head.getD(), row+1, newgscore + newHscore, newgscore, rowV);
                q.push(newstate);
                found = true;
            }
        }
        if (!found) {
            max = lastMax;
            lastMax = 0;
        }
    }
    return max;
    
}
void writeOutput(int ans){
    ofstream myfile;
    myfile.open("output.txt");
    myfile << ans << "\n";
    myfile.close();
}

int main(){
    int n, d, p;
    string algorithm;
    ifstream myfile;
    myfile.open("input.txt");
    myfile >> n >> d >> p;
    myfile >> algorithm;
    vector<vector<int> > board;
    
    for (int i = 0; i < n; i++){
        vector<int> temp(n,0);
        board.push_back(temp);
    }
    for (int i = 0; i < p;i++){
        int a, b;
        char c;
        myfile >> a >> c >> b;
        board[a][b] += 1;
    }
    vector <int> row;
    vector <bool> col;
    vector <bool> dia;
    vector <bool> revdia;

    for (int i = 0; i < n; i++){
        row.push_back(-1);
        col.push_back(0);
    }
    for (int i = 0; i < 2*n - 1; i++){
        dia.push_back(0);
        revdia.push_back(0);
    }
    if (algorithm == "dfs"){
        writeOutput(dfSearch(n, d, p, board, row, col, dia, revdia));
    }
    else if (algorithm == "astar"){
        writeOutput(astarSearch(n, d, p, board));
    }
    
}