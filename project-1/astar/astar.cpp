#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <queue>
#include <string>

using namespace std;
using namespace std::chrono;

class state{
    private:
        int fscore;
        int gscore;
        int hscore;
        int currentRow;
        int currentCol;
        vector<int> row;
        int d;
    public:
        state(int drones, int row, int col, int _fscore, int _gscore, int _hscore, vector<int> _row):d(drones),currentRow(row), currentCol(col), fscore(_fscore), gscore(_gscore), hscore(_hscore), row(_row){}
        int getRow () { return currentRow;}
        int getD() {return d;};
        int getGscore(){return gscore;}
        vector<int> getRowV() {return row;};
        bool operator< (const state &other) const {
		    return fscore < other.fscore;
	    }


};
bool greaterThan (int x, int y){
    return x > y;
}
std::vector<std::vector<int>> createHscores(std::vector<std::vector<int> >& animPos) {
	std::vector<std::vector<int>> result;
    std::vector<int> rowScores;
    for (auto& iter: animPos) {
        rowScores.emplace_back(*std::max_element(iter.begin(), iter.end()));
    }
    int num = 0;
    std::vector<int> tempScores;
	std::vector<int> hScores;
	
    for (int i = 0; i < rowScores.size(); ++i) {
        tempScores.clear();
		copy(rowScores.begin() + i, rowScores.end(), back_inserter(tempScores));
        sort(tempScores.begin(), tempScores.end(), greaterThan);

		int accumulate = 0;
		hScores.clear();
		for (auto& iter: tempScores){
			accumulate += iter;
			hScores.emplace_back(accumulate);
		}

		result.emplace_back(hScores);
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

// clarify the meaning of currentRow
// currentRow = 3: is row 3 placed or to be placed
// d = 2: is 2 drones left after placing or before
int astarSearch(int n, int d, int p, vector<vector<int> >& board){
    priority_queue<state> q;
    int maxd = d;
    vector<int> _row(n,-1);
    // a node state
    state head(d, 0, -1, 0 , 0, 0, _row);
    q.push(head);
    int max = 0;
    //vector<vector<int> > hscores = {{0,1,2},{0,1},{0}};

    vector<vector<int> > hscores = createHscores(board);
    for (int i = 0; i < hscores.size(); i++){
        for (int j = 0; j < hscores[i].size(); j++){
            //cout << hscores[i][j]<<" ";
        }
        //cout << endl;
    }
    //cout << endl;
    while (!q.empty()){
        state head = q.top();
        cout << "at row " << head.getRow() << " with "<< head.getD() << " drones" << endl;
        q.pop();
        bool found = false;
        // if the number of drones remaining is 0
        // return the number
        vector<int> rowV = head.getRowV();

        int row = head.getRow();
        if (head.getD() == 0) {
            
            return head.getGscore();
            
           //return head.getPack();
        }
        // if decide to place and still smaller
        // then continue
        //if (head.getGscore() + hscores[row][head.getD()-1] < max) continue;
        int newgscore = head.getGscore();
        int newHscore = 0;

        // fill the rowth row
        for (int i = 0; i < n;i++) {
            rowV = head.getRowV();
            newHscore = 0;
            newgscore = head.getGscore();
            if (isLegal(row, i, rowV, n)){
                // decide to place
                if (row+1 < n) newHscore = hscores[row+1][d-1];
                else newHscore = 0;

                newgscore += board[row][i];
                if (newgscore > max) max = newgscore;
                rowV[row] = i;
                // << "row: "<< row << " col: "<< i <<" with score " << newHscore + newgscore<< endl;
                state newstate(head.getD()-1, row+1, i, newHscore + newgscore, newgscore, newHscore, rowV);
                q.push(newstate);
                found = true;
            }
        }

        // if not found
        // either no fit place
        // or we don't place in this line
        if (row+1 < n && n - row >= head.getD()){
            rowV[row] = -1;
            newHscore = hscores[row][d];
            newgscore = head.getGscore();
            if (newHscore + newgscore > max){
                state newstate(head.getD(), row+1, -1, newgscore + newHscore, newgscore, newHscore, rowV);
                q.push(newstate);
            }
        }
    }
    return max;
    
}

int main(){
    // n is weight and height of the area
    // d is the number of drones
    // p is the number of packets
    for (int i = 1; i < 2; i++){
        auto start = high_resolution_clock::now();
        int n, d, p;
        string algorithm;
        ifstream myfile;
        string filename = "input" + to_string(i) + ".txt";
        myfile.open("input1.txt");
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
            //cout << dfSearch(n, d, p, board, row, col, dia, revdia)<<endl;
            auto stop = high_resolution_clock::now();
            auto duration = duration_cast<microseconds>(stop - start);
            cout << "time used: " << duration.count()/1000000.0 << "s" << endl;
        }
        else if (algorithm == "astar"){
            //cout << "---------------"<<endl;
            cout << astarSearch(n, d, p, board) << endl;
            auto stop = high_resolution_clock::now();
            auto duration = duration_cast<microseconds>(stop - start);
            cout << "time used: " << duration.count()/1000000.0 << "s" << endl;
        }
    }  
    
}