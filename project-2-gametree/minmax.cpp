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
        isMax(_isMax), unvisited(_unvisited), tmpTeamA(_teamA), tmpTeamB(_teamB) {}

        bool isRoot;
        bool isMax;
        vector<state*> children;
        vector<contestant*> unvisited;
        team tmpTeamA;
        team tmpTeamB;
        double advantage = 0.0; 

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
double maxValue(state*s, double alpha, double beta);
double minValue(state*s, double alpha, double beta);

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

double maxValue (state*s){
    if (terminateTest(s)) {
        s->advantage = s->tmpTeamA.sumUp() - s->tmpTeamB.sumUp();
        return s->advantage;
    }
    double retval = -(numeric_limits<double>::max)();
    for (state* child : s->children){
        double tmp = minValue(child);
        retval = (retval < tmp) ? tmp : retval;
    }
    s->advantage = retval;
    return retval;
}

double minValue (state*s){
    if (terminateTest(s)) {
        s->advantage = s->tmpTeamA.sumUp() - s->tmpTeamB.sumUp();
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
    double adv = -(numeric_limits<double>::max)();
    for (state* s : root->children) {
        double tmpVal = minValue(s);
        if (tmpVal > adv){
            adv = tmpVal;
            tmp = s;
        }
    }
    root->tmpTeamA.sortTeam();
    tmp->tmpTeamA.sortTeam();
    for (int i = 0; i < root->tmpTeamA.count();i++){
        if (root->tmpTeamA.members[i] != tmp->tmpTeamA.members[i]) return tmp->tmpTeamA.members[i]->id;
    }
    return tmp->tmpTeamA.members[tmp->tmpTeamA.count()-1]->id;
}
state* expandChild(state* parent, int val){
    state* s = NULL;
    vector <contestant*> newUnvisited;
    for (int i = 0; i < parent->unvisited.size(); i++){
        if (i != val) newUnvisited.push_back(parent->unvisited[i]);
    }
    if (parent->isMax){
        team newTeamA(true);
        newTeamA.members = parent->tmpTeamA.members;
        newTeamA.members.push_back(parent->unvisited[val]);
        s = new state(false, false, newUnvisited, newTeamA, parent->tmpTeamB);
    }
    else{
        team newTeamB(false);
        newTeamB.members = parent->tmpTeamB.members;
        newTeamB.members.push_back(parent->unvisited[val]);
        s = new state(false, true, newUnvisited, parent->tmpTeamA, newTeamB);
    }
    parent->children.push_back(s);
    return s;
}
double maxValue(state*s, double alpha, double beta){
    if (terminateTest(s)) {
        s->advantage = s->tmpTeamA.sumUp() - s->tmpTeamB.sumUp();
        return s->advantage;
    }
    double retval = -(numeric_limits<double>::max)();
    for (int i = 0; i < s->unvisited.size(); i++){
        // expand the next child
        state* child = expandChild(s,i);
        retval = max(retval, minValue(child,alpha,beta));
        if (retval >= beta) {
            s->advantage = retval;
            return retval;
        }
        alpha = max(alpha, retval);
    }
    s->advantage = retval;
    return retval;
}
double minValue(state*s, double alpha, double beta){
    if (terminateTest(s)) {
        s->advantage = s->tmpTeamA.sumUp() - s->tmpTeamB.sumUp();
        return s->advantage;
        //return s->tmpTeamA.sumUp() - s->tmpTeamB.sumUp();
    }
    double retval = (numeric_limits<double>::max)();
    for (int i = 0; i < s->unvisited.size(); i++){
        // explore the next child
        state* child = expandChild(s,i);
        retval = min(retval, maxValue(child, alpha, beta));
        if (retval <= alpha) {
            s->advantage = retval;
            return retval;
        }
        beta = min(beta, retval);
    }
    s->advantage = retval;
    return retval;
}
int ab(team teamA, team teamB, vector<contestant*> people){
    state* root = new state(true, true, people, teamA, teamB);
    double ad = maxValue(root, -(numeric_limits<double>::max)(), (numeric_limits<double>::max)());
    state* next;
    for (state* s : root->children) {
        if (s->advantage == ad){
            next = s;
            break;
        }
    }
    root->tmpTeamA.sortTeam();
    next->tmpTeamA.sortTeam();

    for (int i = 0; i < root->tmpTeamA.count();i++){
        if (root->tmpTeamA.members[i] != next->tmpTeamA.members[i]) return next->tmpTeamA.members[i]->id;
    }
    return next->tmpTeamA.members[next->tmpTeamA.count()-1]->id;
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
        cout << minmax(teamA, teamB, people) << endl;
    }
    else if (algorithm == "ab"){
        cout << ab(teamA, teamB, people) << endl;
    }
    return 0;
}