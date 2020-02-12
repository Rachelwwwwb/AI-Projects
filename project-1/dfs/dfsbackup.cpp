#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <queue>
using namespace std;
using namespace std::chrono;


// for dsf the h function
int** getScore(vector<vector<int>> board, const int n){
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
    for (int i = 0; i < n; i++){
        for (int j = 0; j < n; j++){
            //cout<<score[i][j]<<" ";
        }
        //cout << endl;
    }
    return score;
}


void astarSearch(int n, int d, int p, bool** board){
}




int dfSearch(const int n, int d, const int p, vector<vector<int> > board, vector<int> row, vector<bool>& col, vector<bool>& dia, vector<bool>& revdia){
    int i = 0;
    int pac = d;
    bool backtrace = false;
    int max = 0;
    int currentRow = 0;
    int currentCol = 0;
    int currentPack = 0;
    bool found = false;

    if (!d) return 0;
    int** score = getScore(board, n);
    while (true) {
        i++;
        //cout << i<<": row is : " << currentRow << " col is : "<< currentCol << " number of drones is " << d << " max value is " << max << endl;
        found = false;
        backtrace = false;
        // loop over each column


        while(currentCol < n){
            if ( !col[currentCol] && !dia[currentCol + currentRow] && !revdia[currentRow - currentCol + n - 1]){
                if (currentCol < n && currentPack + board[currentRow][currentCol] + score[currentRow][currentCol] > max){
                    //cout << "placed in: "<< currentCol << endl;
                    //cout << endl;
                    currentPack += board[currentRow][currentCol];
                    col[currentCol] = 1;
                    dia [currentCol + currentRow] = 1;
                    revdia[currentRow - currentCol + n - 1] = 1;
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
            // it can either be unable to place all drones

            // if the number of rows left is smaller than the number of drones
            // this means we can no longer place all the drones
            // no need to cont, backtrace
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
                // not necessary; don't have to fill the first line
                if (currentRow < 0) return max;
                // or we can test whether the currentCol is equal to d+1
                d++;
                int contback = true;
                while (row[currentRow] == -1 || row[currentRow] == n-1){
                    if (row[currentRow] == n-1) {
                        d++;
                        if (d > pac){
                            d = pac;
                            //cout << "switch!" << endl;
                            break;
                        }
                        row[currentRow] = -1;
                        col[n-1] = 0;
                        dia[n-1 + currentRow] = 0;
                        revdia[currentRow] = 0;
                        currentPack -= board[currentRow][n-1];
                        // if (currentRow+1 < n && n - currentRow - 1 >= d){
                        //     currentRow++;
                        //     currentCol = 0;
                        //     contback = false;
                        //     break;
                        // }
                    }
                    currentRow --;
                    // not sure at all
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

int main(){
        for (int i = 0; i < 1; i++){
        auto start = high_resolution_clock::now();
        int n, d, p;
        string algorithm;
        ifstream myfile;
        string filename = "input" + to_string(i) + ".txt";
        myfile.open("input14.txt");
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
            cout << "---------------"<<endl;
            cout << filename << ": " << dfSearch(n, d, p, board, row, col, dia, revdia)<<endl;
            auto stop = high_resolution_clock::now();
            auto duration = duration_cast<microseconds>(stop - start);
            cout << "time used: " << duration.count()/1000000.0 << "s" << endl;
        }
        else if (algorithm == "astar"){
            //cout << "---------------"<<endl;
            //cout << filename << ": " << astarSearch(n, d, p, board) << endl;
            auto stop = high_resolution_clock::now();
            auto duration = duration_cast<microseconds>(stop - start);
            cout << "time used: " << duration.count()/1000000.0 << "s" << endl;
        }
        }
    
}