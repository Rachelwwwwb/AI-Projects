#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <cmath>
using namespace std;

class square{
    public:
        square(){}
        char dir = 0;
        double utility = 0.0;
};

void value_iteration(vector<vector<square> >& grid, bool& stop){
    stop = true;
     for (int i = 0; i < grid.size(); i++){
        for (int j = 0; j < grid.size(); j++){
            if(grid[j][i].dir != '.'){
                double max = -INFINITY;
                int max_dir = -1;
                for (int num = 0; num < 4; num++){
                    // the probability of going to each direction
                    double up = 0.1;
                    double down = 0.1;
                    double left = 0.1;
                    double right = 0.1;
                    double stay = 0.0;
                    // the value of squares in each direction
                    double upValue = 0.0;
                    double downValue = 0.0;
                    double leftValue = 0.0;
                    double rightValue = 0.0;

                    // find the uility of going up
                    if (num == 0) up = 0.8;
                    // find the uility of going down
                    else if (num == 1) down = 0.8;
                    // find the uility of going left 
                    else if (num == 2) left = 0.8;
                    // find the uility of going right
                    else if (num == 3) right = 0.8;

                    if (j-1 < 0){
                        stay += left;
                        left = 0.0;
                    }
                    else {
                        leftValue = grid[j-1][i].utility;
                    }

                    if (j+1 >= grid.size()){
                        stay += right;
                        right = 0.0;
                    }
                    else {
                        rightValue = grid[j+1][i].utility;
                    }
                    if (i+1 >= grid.size()){
                        stay += down;
                        down = 0.0;
                    }
                    else {
                        downValue = grid[j][i+1].utility;
                    }
                    if (i-1 < 0) {
                        stay += up;
                        up = 0.0;
                    } 
                    else {
                        upValue = grid[j][i-1].utility;
                    }

                    // the reward for a given direction
                    double tmp = up * upValue + down * downValue + left * leftValue + right * rightValue + stay * grid[j][i].utility;
                    if (max < tmp) {
                        max = tmp;
                        max_dir = num;
                    }
                }

                // gamma is 0.9
                double oldU = grid[j][i].utility;
                if (grid[j][i].dir != 'o' && grid[j][i].dir != '.') {
                    grid[j][i].utility = -1 + 0.9 * max;
                    if (max_dir == 0) grid[j][i].dir = '^';
                    else if (max_dir == 1) grid[j][i].dir = 'v';
                    else if (max_dir == 2) grid[j][i].dir = '<';
                    else if (max_dir == 3) grid[j][i].dir = '>';

                }
                else if (grid[j][i].dir == 'o') grid[j][i].utility = -101 + 0.9 * max;
                // epsilon is 0.01
                if (abs( grid[j][i].utility - oldU) < 0.01) stop &= true;
                else stop = false;
            }
        }
    }

    
}
double checkPercent(const vector<vector<square> >& grid){
    ifstream myfile;
    myfile.open("output_sample.txt");
    int total = grid.size()*grid.size();
    int correct = 0;
    for (int i = 0; i < grid.size(); i++){
        string line; 
        myfile >> line;
        for (int j = 0; j < grid.size(); j++){
            if (grid[j][i].dir == line[j]) correct++;
        }
    }
    return 100*double(correct)/ double(total);
}
void writeOutput(const vector<vector<square> >& grid){

    ofstream myfile;
    myfile.open("output.txt");
    for (int i = 0; i < grid.size(); i++){
        for (int j = 0; j < grid.size(); j++){
            myfile <<  grid[j][i].dir;
        }
        myfile << "\n";
    }
    myfile.close();
}
// after getting the utility function
// write the direction for each square
void getDir(vector<vector<square> >& grid){
    bool stop = false;
    while(!stop) value_iteration(grid,stop);
    cout << checkPercent(grid) << "%" << endl;
    writeOutput(grid);
}

int main(){
    int d;  // for dimension
    int num; // for number of obstacles
    ifstream myfile;
    string infilename = "input.txt";
    string outfilename = "output.txt";
    myfile.open(infilename);
    myfile >> d;
    myfile >> num;
    vector<vector<square> > grid (d, vector<square>(d,square() ));


    for (int i = 0; i < num+1; i++){
        int x;
        int y;
        char comma;
        myfile >> y >> comma >> x;
        if (i == num) {
            grid[y][x].dir = '.';
            grid[y][x].utility = 100;
        }
        else {
            grid[y][x].dir = 'o';
        }
    }
    getDir(grid);
    return 0;
}