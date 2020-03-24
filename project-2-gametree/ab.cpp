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
    int count(){
        return members.size();
    }
    double sumUp(){
        double retval = 0.0;
        int arr[10] = {0};
        bool bonus = true;
        for(int i = 0; i < members.size(); i++){
            int digit = to_string(members[i]->id)[to_string(members[i]->id).size()-1] - '0';
            arr[digit] += 1;
            if (arr[digit] > 1) bonus = false;
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
    void print(){
        for (contestant* i : members) cout << i->id << " ";
        cout << endl;
    }
};

class state{
    public:
        state(bool _isRoot, bool _isMax, vector<contestant*> _unvisited, team _teamA, team _teamB):isRoot(_isRoot), 
        isMax(_isMax), unvisited(_unvisited), tmpTeamA(_teamA), tmpTeamB(_teamB),advantage(0.0) {}

        bool isRoot;
        bool isMax;
        vector<state*> children;
        vector<contestant*> unvisited;
        team tmpTeamA;
        team tmpTeamB;
        double advantage; 

};
state* smallestChild(state*s){
    double smallest = (numeric_limits<double>::max)();
    state* retval = NULL;
    for (auto& i : s->children){
        if (i->advantage < smallest){
            smallest = i->advantage;
            retval = i;
        }
    }
    return retval;
}
state* largestChild(state*s){
    double largest = (numeric_limits<double>::min)();
    state* retval = NULL;
    for (auto& i : s->children){
        if (i->advantage > largest){
            largest = i->advantage;
            retval = i;
        }
    }
    return retval;
}
double maxValue (state*s);
double minValue (state*s);

bool terminateTest(state*s){
    return ( (s->tmpTeamA.count() == 5) && (s->tmpTeamB.count() == 5) );
}
state* buildGraph(team teamA, team teamB, vector<contestant*> people){
    // form the tree
    state*root = new state(true, true,people,teamA, teamB);
    stack<state*> stk;
    stk.push(root);
    while (!stk.empty()){
        state* tmp = stk.top();
        stk.pop();

        if (terminateTest(tmp)) continue;
        for (int i = 0; i < tmp->unvisited.size();i++){
            vector<contestant*> newPeople;
            team teamNext(tmp->isMax);
            // if the state is max, add one in the teamA
            if (tmp->isMax) teamNext.members = tmp->tmpTeamA.members;
            else teamNext.members = tmp->tmpTeamB.members;

            teamNext.insert(tmp->unvisited[i]);
            for (auto& it : tmp->unvisited){
                if (it != tmp->unvisited[i]) newPeople.push_back(it);
            }
            state* nextState;
            if (tmp->isMax) nextState = new state(false,false, newPeople, teamNext, tmp->tmpTeamB);
            else nextState = new state(false,true, newPeople, tmp->tmpTeamA, teamNext);
            
            stk.push(nextState);
            tmp->children.push_back(nextState);
        }

    }
    return root;
}

state* maxState = NULL;
double maxValue (state*s){
    if (terminateTest(s)) {
        if (!maxState) maxState = s;
        s->advantage = s->tmpTeamA.sumUp() - s->tmpTeamB.sumUp();
        if (maxState->tmpTeamA.sumUp() - maxState->tmpTeamB.sumUp() < s->advantage) maxState = s;
        return s->advantage;
    }
    double retval = (numeric_limits<double>::min)();
    for (state* child : s->children){
        double tmp = minValue(child);
        retval = (retval < tmp) ? tmp : retval;
    }
    s->advantage = retval;
    return retval;
}

double minValue (state*s){
    if (terminateTest(s)) {
        if (!maxState) maxState = s;
        s->advantage = s->tmpTeamA.sumUp() - s->tmpTeamB.sumUp();
        if (maxState->tmpTeamA.sumUp() - maxState->tmpTeamB.sumUp() < s->advantage) maxState = s;
        return s->advantage;
    }
    double retval = (numeric_limits<double>::max)();
    for (state* child : s->children){
        double tmp = maxValue(child);
        retval = (tmp < retval) ? tmp : retval;
    }
    s->advantage = retval;
    return retval;
}

int minmax(team teamA, team teamB, vector<contestant*> people){
    state* root = buildGraph(teamA, teamB, people);
    state* tmp = NULL;
    double adv = (numeric_limits<double>::min)();
    for (state* s : root->children) {
        double tmpVal = minValue(s);
        if (!tmp) tmp = s;
        cout << tmpVal;
        if (tmpVal > adv){
            adv = tmpVal;
            tmp = s;
        }
    }
    root->tmpTeamA.sortTeam();
    tmp->tmpTeamA.sortTeam();
    cout << "the root team is " << endl;
    root->tmpTeamA.print();
    cout << "the tmp team is " << endl;
    tmp->tmpTeamA.print();

    cout << "max state" << endl;
    maxState->tmpTeamA.print();
    maxState->tmpTeamA.sortTeam();
    cout << maxState->tmpTeamA.sumUp();
    for (int i = 0; i < root->tmpTeamA.count();i++){
        if (root->tmpTeamA.members[i] != tmp->tmpTeamA.members[i]) return tmp->tmpTeamA.members[i]->id;
    }
    return tmp->tmpTeamA.members[tmp->tmpTeamA.count()-1]->id;
}
// possible optimization: pass only the score and current chosen contestant 
// rather than the whole team
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

    if (algorithm == "minimax"){
        cout << minmax(teamA, teamB, people);
    }
    else if (algorithm == "ab"){
    }
    return 0;
}