#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
using namespace std;
using namespace std::chrono;
struct coord{
    int x;
    int y;
};

void astarSearch(int n, int d, int p, bool** board){

}

int dfSearch(const int n, int d, const int p, int** board, vector<int> row, vector<bool>& col, vector<bool>& dia, vector<bool>& revdia){
    // change from recursion to iterative

    bool backtrace = false;
    int max = 0;
    int currentRow = 0;
    int currentCol = 0;
    int currentPack = 0;
    bool found = false;
    if (!d) return 0;
    bool cont = true;
    // loop over each column
    while (cont) {
        cout << "row is : " << currentRow << " col is : "<< currentCol << " number of drones is " << d << " max value is " << max << endl;
        found = false;
        backtrace = false;
        for (currentCol; currentCol < n; currentCol++){
            if ( !col[currentCol] && !dia[currentCol + currentRow] && !revdia[currentRow - currentCol + n - 1]){
                // cout << "placed in: "<< currentCol << endl;
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

        if (found) {
            if (d == 0){
                if (currentPack > max) max = currentPack;
                if (max == p) return p;
                backtrace = true;
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
            if (found){
                // cancel all the conflicts that have been labelled
                int tempCol = row[currentRow];
                col[tempCol] = 0;
                dia[tempCol + currentRow] = 0;
                revdia[currentRow - tempCol + n - 1] = 0;
                currentPack -= board[currentRow][tempCol];
                d++;
            }
            
            row[currentRow] = -1;
            // if still has more columns, start with the same row but next col
            currentCol++;
            // if it reaches the last columns, go back to the last row
            if (currentCol >= n){
                // need a while loop to decide which row to go
                currentRow --;
                // not necessary; don't have to fill the first line
                if (currentRow < 0) return max;
                d++;
                while (row[currentRow] == -1 || row[currentRow] == n-1){
                    if (row[currentRow] == n-1) {
                    
                        row[currentRow] = -1;
                        col[n-1] = 0;
                        dia[n-1 + currentRow] = 0;
                        revdia[currentRow] = 0;
                        currentPack -= board[currentRow][n-1];
                        d++;
                    }
                    currentRow --;
                    // not necessary; don't have to fill the first line
                    if (currentRow < 0 ) return max;
                }  
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


    return max;
}

int main(){
    // n is weight and height of the area
    // d is the number of drones
    // p is the number of packets
    auto start = high_resolution_clock::now();
    int n, d, p;
    string algorithm;
    ifstream myfile;
    myfile.open("input1.txt");
    myfile >> n >> d >> p;
    myfile >> algorithm;
    int** board = new int*[n];
    for (int i = 0; i < n; i++){
        board[i] = new int[n];
        for (int j = 0; j < n; j++){
            board[i][j] = 0;
        }
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
        cout << dfSearch(n, d, p, board, row, col, dia, revdia)<<endl;
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        cout << "time used: " << duration.count()/1000000.0 << "s" << endl;
    }
    
}