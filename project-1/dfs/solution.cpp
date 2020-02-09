//#include <cstdlib>
//#include "trapState.h"
#include <fstream>
#include <string>
#include <iostream>
#include <algorithm>
#include <vector>
#include <numeric>
#include <queue>
#include <iterator>

#define optimizer __attribute__((optimize("O2")))
// For the trapState class
class trapState {
public:
    trapState(int cam, int r, int c, int g, int f, std::vector<int> o);
    //bool operator< (const trapState &other);
    std::vector<int>& getOccupied(){
        return occupied;
    }
    int getGScore(){
        return gScore;
    }
    int getFScore(){
        return fScore;
    }
    int getCamNum(){
        return camNum;
    }
    int getRow(){
        return row;
    }
	bool operator< (const trapState &other) const {
		return fScore < other.fScore;
	}

protected:

private:
    int camNum;
    int row;
    int col;
    int gScore;
    int fScore;
    std::vector<int> occupied;
};

trapState::trapState(int cam, int r, int c, int g, int f, std::vector<int> o):
    camNum(cam), row(r), col(c), gScore(g), fScore(f), occupied(o){
    // -1 is the start. The board is empty.
    if (row != -1) {
        occupied[row] = col;
    }
}

/*
class ComparisonClass {
public:
    bool operator() (trapState &s1, trapState &s2) {
        return s1.getFScore() < s2.getFScore();
    }
};
*/

int dfsSearch(int dimension, int camNum, std::vector<std::vector<int> >& animPos);
int astarSearch(int dimension, int camNum, std::vector<std::vector<int> >& animPos);
bool canVisit(int row, int col, int dimension, std::vector<int>& occupied);
std::vector<std::vector<int>> createHscores(std::vector<std::vector<int> >& animPos);
void writeOutput(std::string fileName, int x);

// This function is taken referenced from ITP435 for parsing input

inline std::vector<std::string> Split(const std::string& str)
{
    const char delim = ',';
    std::vector<std::string> retVal;

    size_t start = 0;
    size_t delimLoc = str.find_first_of(delim, start);
    while (delimLoc != std::string::npos)
    {
        retVal.emplace_back(str.substr(start, delimLoc - start));

        start = delimLoc + 1;
        delimLoc = str.find_first_of(delim, start);
    }

    retVal.emplace_back(str.substr(start));
    return retVal;
}


int main(int argc, const char* argv[]) {
	/*
    // Main function is to read the input from the given file name
    if (argc < 2) {
        std::cout << "No file name given!" << std::endl;
        return -1;
    }

    std::string fileName = argv[1];
	*/
	std::string fileName = "input4.txt";
	
	
    std::ifstream in(fileName);
    std::string line;
    int dimension;
    int camNum;
    int animNum;
    std::string searchType;
    std::vector<std::string> tempNums;

    if (in.is_open())
    {
        // Read in dimension of the zoo
        std::getline(in, line);
        dimension = std::stoi(line);
        // Read in number of traps
        std::getline(in, line);
        camNum = std::stoi(line);
        // Read in number of animals
        std::getline(in, line);
        animNum = std::stoi(line);
        // Read in search type
        std::getline(in, searchType);
        // Read in animal positions
        // Initialize animal number to 0
        std::vector<int> numLine;
        for (int i = 0; i < 15; ++i) {
            numLine.emplace_back(0);
        }
        std::vector<std::vector<int> > animPos;
        for (int i = 0; i < 15; ++i) {
            animPos.emplace_back(numLine);
        }

        for (int i = 0; i < animNum; ++i) {
            std::getline(in, line);
            tempNums = Split(line);
            animPos[std::stoi(tempNums[0])][std::stoi(tempNums[1])] += 1;
        }

        int result = 0;
        if (searchType == "dfs") {
            result = dfsSearch(dimension, camNum, animPos);
        }
        else if (searchType == "astar") {
            result = astarSearch(dimension, camNum, animPos);
        }
        else {
            std::cout << "Invalid search type!" << std::endl;
        }

        // Write output
        std::cout << result << std::endl;
        writeOutput("output.txt", result);
    }
    else {
        std::cout << "File " << fileName << " failed to get open!" << std::endl;
    }

    //system("PAUSE");

    return 0;
}


int dfsSearch(int dimension, int camNum, std::vector<std::vector<int> >& animPos) {
    int currentRow = 0;
    int currentCol = 0;
    int maxCaught = 0;
    int currCaught = 0;
    int constCam = camNum;
    // Set up a clean board
    // We just store the column number of the trap placed
    std::vector<int> occupied;
    for (int i = 0; i < dimension; ++i) {
        occupied.emplace_back(dimension);
    }

    // Though it should be strictly positive, have a sanity check
    if (camNum <= 0) {
        return 0;
    }

    // A temporary variable showing whether found a viable position in current row
    bool found = false;
    // A temporary variable showing whether we need back track
    bool needBack = false;
	
	// The essence of estimate for alpha-beta is the same as astar
	std::vector<std::vector<int>> estimates = createHscores(animPos);

    while (!(currentRow == 0 && currentCol == dimension+1)) {
        std::cout << "row is : " << currentRow << " col is : "<< currentCol << " number of drones is " << camNum << " max value is " << maxCaught << std::endl;
        if (camNum == constCam) {
            std::cout << "hua dian!!!" << std::endl;
        }
        found = false;
        needBack = false;
		
		// Alpha-beta
		// If it's not possible to reach better, backtrack
		
        if (0){
        //if (currCaught + estimates[currentRow][camNum-1] <= maxCaught){
		//	needBack = true;
		}else{
				// Find a possible position in the row
			for (int i = currentCol; i < dimension; ++i) {
                // loop over the remaining col
                // record the col in the row array
				if (canVisit(currentRow, i, dimension, occupied)) {
					currentCol = i;
					camNum -= 1;
					occupied[currentRow] = currentCol;
					found = true;
					currCaught += animPos[currentRow][currentCol];
					break;
				}
			}

			// If we do find a valid position to place the camera in the column
			if (found) {
				if (camNum == 0) {
					// Update the maximum score
					if (maxCaught < currCaught) {
						maxCaught = currCaught;
					}

					// Revert the addition
					currCaught -= animPos[currentRow][currentCol];
					camNum += 1;

					// If it's the end column, it's equivalent to we did not find it
					if (currentCol == dimension - 1) {
						found = false;
					}
					else {
						// Else increment column number and continue
						currentCol += 1;
					}
				}
				else {
					currentCol = 0;
					currentRow += 1;
				}
			}
			
			if (!found){
				// If reach the end column or more camera than needed, we should backtrack
				if (currentRow == dimension-1 || dimension - currentRow <= camNum) {
					needBack = true;
				}
				else {
					// Or we don't need to place a camera
					occupied[currentRow] = dimension;
					currentCol = 0;
					currentRow += 1;
				}
			}
		}

        // Back track
        // First we need to check whether it has reached the first row
        if (needBack){
            if (currentRow == 0){
                currentCol += 1;
                needBack = false;
            }
        }

        // Continue backtrack until find a valid position
        // Or we have reached the first row and its end position
        while (needBack) {

            currentRow -= 1;

            currentCol = occupied[currentRow];
            if (currentCol != dimension || currentRow == 0) {
                if (currentCol != dimension) {
                    currCaught -= animPos[currentRow][currentCol];
                    camNum += 1;
                }
                currentCol += 1;
                needBack = false;
            }
        }

    }

    return maxCaught;
}


int astarSearch(int dimension, int camNum, std::vector<std::vector<int> >& animPos) {
    // Set up a clean board
    // We just store the column number of the trap placed
    std::vector<int> occupied;
    for (int i = 0; i < dimension; ++i) {
        occupied.emplace_back(dimension);
    }

    std::vector<std::vector<int>> hScores = createHscores(animPos);

	// Store max g to reduce some cases
	int maxG = 0;
	
    // Note that max heap doesn't work somehow...
    std::priority_queue<trapState> states;
    trapState t(camNum, -1, 0, 0, hScores[0][camNum - 1], occupied);
    states.push(t);

    while (!states.empty()) {
        // Take out the largest element
        trapState stateNow = states.top();
        states.pop();

        // If it's the goal state, return
        int camNum = stateNow.getCamNum();
        if (camNum == 0){
            /*
            // Testing
            std::cout << "dimension: " << dimension << std::endl;
            std::cout << stateNow.getRow() << std::endl;
            for (auto& iter: stateNow.getOccupied()){
                std::cout << iter << " ";
            }
            std::cout << std::endl;
            */
            return stateNow.getGScore();
        }

        // If we have reached the last row but camera number is not 0
        // There's something wrong with it
        // Sanity check
        int rowNum = stateNow.getRow();
		/*
        if (rowNum == dimension - 1){
            std::cout << "Reached last row but not finish! Remaining camera number: " << camNum << std::endl;
            return -1;
        }
		*/

        // Else, expand the states
        int gScore = stateNow.getGScore();
		int tempFScore = 0;

        for (int i = 0; i < dimension; ++i) {
            if (canVisit(rowNum+1, i, dimension, stateNow.getOccupied())) {
				if (camNum - 1 == 0) {
					tempFScore = gScore + animPos[rowNum+1][i];
				}
				else {
					/*
					// Sanity check
					if (rowNum + 1 == dimension - 1){
						std::cout << "Something went wrong overflow!" << std::endl;
					}
					*/
					tempFScore = gScore + animPos[rowNum+1][i] + hScores[rowNum+1+1][camNum-2];
				}
				// Only add it when it has the potential to reach higher
				if (tempFScore > maxG){
                    if (camNum == 1){
                        maxG = tempFScore;
                    }			
					trapState tempState(camNum-1, rowNum+1, i, gScore + animPos[rowNum+1][i], tempFScore, stateNow.getOccupied());
					states.push(tempState);
				}
            }
        }
        // If camera number exceeds the remaning column, discard
		// Or we can leave the row blank
        if (camNum <= dimension - 1 - (rowNum+1)){
			/*
			// Sanity check 2
			if (rowNum + 1 == dimension - 1){
				std::cout << "Something went wrong overflow!" << std::endl;
			}
			*/
			tempFScore = gScore + hScores[rowNum+1+1][camNum-1];
			if (tempFScore > maxG){
				trapState tempState(camNum, rowNum+1, dimension, gScore, tempFScore, stateNow.getOccupied());
				states.push(tempState);
			}
        }
    }

    return 0;
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


bool greaterThan(int x, int y) 
{ 
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


void writeOutput(std::string fileName, int x){
    std::ofstream ofs;
    ofs.open (fileName);
    if (ofs.is_open()) {
        ofs << x;
        ofs.close();
    }
    else {
        std::cout << "Error writing to output file " << fileName << std::endl;
    }
}