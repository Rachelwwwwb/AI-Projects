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

int dfSearch(const int n, int d, const int p, int** board, vector<bool>& col, vector<bool>& row, vector<bool>& dia, vector<bool>& revdia){
    // if all the drones are on the board
    if (!d) return 0;
    int max = 0;
    for (int i = 0; i < n; i++){
        if (!row[i]){
            for (int j = 0; j < n; j++){
                if (!row[j] && !dia[i+j] && !revdia[i-j+n-1]){
                    //cout << "checking... i: " << i << " j: "<< j << " for the drone: "<< d << endl;
                    row[j] = 1;
                    col[j] = 1;
                    dia[i+j] = 1;
                    revdia[i-j+n-1] = 1;
                    int val = board[i][j] + dfSearch(n, d-1, p, board, col, row, dia, revdia);
                    //cout << "val is: "<< val << " and max is: "<< max << endl;
                    if (val > max) max = val;
                    // if already pick up all the packets, return strictly
                    row[j] = 0;
                    col[j] = 0;
                    dia[i+j] = 0;
                    revdia[i-j+n-1] = 0;
                }
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
    myfile.open("input4.txt");
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
    vector<bool> col;
    vector<bool> row;
    vector<bool> dia;
    vector<bool> revdia;
    for (int i = 0; i < n; i++){
        col.push_back(0);
        row.push_back(0);
    }
    for (int i = 0; i < 2*n - 1; i++){
        dia.push_back(0);
        revdia.push_back(0);
    }
    if (algorithm == "dfs"){
        //cout << "---------------"<<endl;
        cout << dfSearch(n, d, p, board, col, row, dia, revdia)<<endl;
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        cout << "time used: " << duration.count()/1000000.0 << "s" << endl;
    }
    
}