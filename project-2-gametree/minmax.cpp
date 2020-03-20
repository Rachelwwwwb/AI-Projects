#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <stack>

using namespace std;
class contestant{
    public: 
        contestant(int _id, double _c, double _happya, double _happyb, int _constestState): 
        id(_id),capacity(_c), happy_a(_happya), happy_b(_happyb), constestState(_constestState){}
        
            int id;
            double capacity;
            double happy_a;
            double happy_b;
            int constestState;
};
bool smallerThan (contestant*a, contestant*b ) {
            return a->id < b->id;}
        
class team
{
public:
    team(bool max):isMax(max){}
    vector<contestant*> members;
    bool isMax;
    double sumUp(){
        double retval = 0.0;
        int arr[10] = {0};
        bool bonus = true;
        for(int i = 0; i < members.size(); i++){
            arr[members[i]->id & 1] += 1;
            if (arr[members[i]->id & 1] > 1) bonus = false;
            if (isMax) retval += members[i]->capacity * members[i]->happy_a;
            else retval += members[i]->capacity * members[i]->happy_b;
        }
        if (bonus) retval += 120;
        return retval;
    }
    bool canInsert(){
        return members.size() < 5;
    }
    void insert(contestant* a){
        if (canInsert()) members.push_back(a);
    }
    void sortTeam(){
        sort(members.begin(), members.end(),smallerThan);
    }
};

class state{
    public:
        state(bool _isRoot, bool _isMax, vector<contestant*> _unvisited, team _teamA, team _teamB):isRoot(_isRoot), 
        isMax(_isMax), unvisited(_unvisited), tmpTeamA(_teamA), tmpTeamB(_teamB){}

        bool isRoot;
        bool isMax;
        vector<contestant*> unvisited;
        team tmpTeamA;
        team tmpTeamB;
};


state* buildGraph(team teamA, team teamB, vector<contestant*> people){
    // form the tree
    int count = 1;
    state*root = new state(true, true,people,teamA, teamB);
    stack<state*> stk;
    stk.push(root);
    while (!stk.empty()){
        state* tmp = stk.top();
        stk.pop();
        for (int i = 0; i < tmp->unvisited.size();i++){
            vector<contestant*> newPeople;
            team teamNext(tmp->isMax);
            // if the state is max, add one in the teamA
            if (tmp->isMax) for (auto& it : tmp->tmpTeamA.members) teamNext.members.push_back(it);
            else for (auto& it : tmp->tmpTeamB.members) teamNext.members.push_back(it);
            teamNext.insert(tmp->unvisited[i]);
            for (auto& it : tmp->unvisited){
                if (it != tmp->unvisited[i]) newPeople.push_back(it);
            }
            state* nextState;
            if (tmp->isMax) nextState = new state(false,false, newPeople, teamNext, tmp->tmpTeamB);
            else nextState = new state(false,false, newPeople, tmp->tmpTeamA, teamNext);
            
            stk.push(nextState);
            count++;
        }

    }
    cout << count << endl;
    return root;
}

int main(){
    int num;
    string algorithm;
    ifstream myfile;
    string infilename = "input0.txt";
    string outfilename = "output.txt";
    vector<contestant*> people;
    team teamA(true);
    team teamB(false);
    myfile.open(infilename);
    myfile >> num;
    myfile >> algorithm;

    for (int i = 0; i < num; i++){
        char comma;
        int _id;
        double _c,  _happya,  _happyb;
        int _constestState;
        myfile >> _id >> comma >>  _c >> comma >> _happya >> comma >> _happyb >> comma >>  _constestState;
        contestant* tmp = new contestant(_id, _c, _happya, _happyb, _constestState);
        // push all unpicked into the vector people
        if (!_constestState)people.push_back(tmp);
        else if (_constestState == 1) teamA.insert(tmp);
        else teamB.insert(tmp);
    }
    sort(people.begin(), people.end(), smallerThan);

    cout << endl;
    if (algorithm == "minimax"){
        buildGraph(teamA, teamB, people);
    }
    else if (algorithm == "ab"){
    }
    return 0;
}