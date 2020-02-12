#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <queue>
using namespace std;
using namespace std::chrono;
struct coord{
    int x;
    int y;
};

class state{
    private:
        int fscore;
        int gscore;
        int hscore;
        int packNum;
        int currentRow;
        int currentCol;
        int d;
    public:
        state(int d, int row, int col, int fscore, int gscore, int hscore);
};
bool greaterThan(int x, int y) 
{ 
    return x > y; 
} 


vector<vector<int>> createHscores(int** board, int n) {
	vector<vector<int>> result;
    vector<vector<int>> animPos;
    for (int i = 0; i < n; i++){
        vector<int>temp(n,0);
        for (int j = 0; j < n; j++){
            temp[j] = board[i][j];
        }
        result.push_back(temp);
    }
    vector<int> rowScores;
    for (auto& iter: animPos) {
        rowScores.emplace_back(*std::max_element(iter.begin(), iter.end()));
    }
    int num = 0;
    vector<int> tempScores;
	vector<int> hScores;
	
    for (int i = 0; i < rowScores.size(); ++i) {
        tempScores.clear();
		copy(rowScores.begin() + i, rowScores.end(), back_inserter(tempScores));
        /*
        for (int i = 0; i < tempScores.size();i++) std::cout << tempScores[i] << " ";
        std::cout << std::endl << std::endl;
		*/
        sort(tempScores.begin(), tempScores.end(), greaterThan);
        /*
        for (int i = 0; i < tempScores.size();i++) std::cout << tempScores[i] << " ";
        std::cout << std::endl << std::endl;
        */
		int accumulate = 0;
		hScores.clear();
		for (auto& iter: tempScores){
			accumulate += iter;
			hScores.emplace_back(accumulate);
		}
        //for (int i = 0; i < hScores.size();i++) std::cout << hScores[i] << " ";
        //std::cout << std::endl << std::endl;
		result.emplace_back(hScores);
        //std::cout << "-----------"<<std::endl;
    }

    return result;
}



void astarSearch(int n, int d, int p, bool** board){
}
bool canVisit(int row, int col, int dimension, std::vector<int>& occupied) {
    for (int i = 0; i < row; ++i) {
        // First make sure the row has the trap placed
        if (occupied[i] != dimension) {
            // Then check column conflicts and diagonal conflicts
            if (occupied[i] == col || abs(row - i) == abs(col - occupied[i])) {
                return false;
            }
        }
    }

    return true;
}


int dfSearch(const int n, int d, const int p, int** board, vector<int> row, vector<bool>& col, vector<bool>& dia, vector<bool>& revdia){
    bool backtrace = false;
    int max = 0;
    int currentRow = 0;
    int currentCol = 0;
    int currentPack = 0;
    bool found = false;

    if (!d) return 0;
    vector<vector<int>> score = createHscores(board, n);
    while (!(currentRow == 0 && currentCol == n+1)) {
        //cout <<"row is : " << currentRow << " col is : "<< currentCol << " number of drones is " << d << " max value is " << max << endl;
        found = false;
        backtrace = false;
        // loop over each column

        if (currentCol < n && currentPack + board[currentRow][currentCol] + score[currentRow][currentCol] < max) {
                //cout << "backtrace" << endl;
                backtrace = true;
        }
        else{
            for (currentCol; currentCol < n; currentCol++){

                if ( !col[currentCol] && !dia[currentCol + currentRow] && !revdia[currentRow - currentCol + n - 1]){
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
                    
                    if (currentCol == n-1) found = false;
                    //if (currentRow == n-1) backtrace = true;
                    
                    //else if (currentCol + 1 < n) currentCol ++;
                    else {
                        currentCol ++;
                        //found = false;
                    }
                } 
                else {
                    currentRow ++;
                    currentCol = 0;
                }


            }
            if (!found) {

                if (n - currentRow - 1 <  d || currentRow == n-1) backtrace = true;
                else {
                    row[currentRow] = -1;
                    currentRow++;
                    currentCol = 0;
                }
            }
        }

        // need to backtrack
        if (backtrace) {
            if (currentRow == 0){
                backtrace = false;
                currentCol += 1;
            }

            while (backtrace) {

                currentRow -= 1;

                currentCol = row[currentRow];
                if (currentCol != -1 || currentRow == 0) {
                    if (currentCol != -1) {
                        currentPack -= board[currentRow][currentCol];
                        d += 1;
                        col[currentCol] = 0;
                        dia[currentCol + currentRow] = 0;
                        revdia[currentRow - currentCol + n - 1] = 0;
                        row[currentRow] = -1;
                    }
                    currentCol += 1;
                    backtrace = false;
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