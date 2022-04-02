
// General Problem Solver, GitHub, 2019

//#include <opencv2/opencv.hpp>
#include <vector>
#include <stack>
#include <string>
#include <algorithm>
#include <iostream>

using namespace std;

struct Operator {
	std::string action;
	std::vector<std::string> preconds;
	std::vector<std::string> add;
	std::vector<std::string> remove; };

class prefixed_state {
public:
	bool operator () (std::string s) {
		if (s.find("Exec ") == 0) { return true; }
		else { return false; }	} };

std::vector<std::string> apply_operator (Operator op,
	std::vector<std::string> states, std::vector<Operator> ops,
	std::string goal, std::vector<std::string>& goal_stack);

vector<string> achieve (vector<string> states, vector<Operator> operators,
	string goal, vector<string>& goal_stack) {
	vector<string> result;
	if (find(states.begin(), states.end(), goal) != states.end()) {
		return states; }
	if (find(goal_stack.begin(), goal_stack.end(), goal) != goal_stack.end()) {
		return{}; }
	for (auto const& op : operators) {
		if (find(op.add.begin(), op.add.end(), goal) == op.add.end()) {
			continue; }
		result = apply_operator(op, states, operators, goal, goal_stack);
		if (!result.empty()) {
			return result; } }
	return{}; }

vector<string> achieve_all (vector<string> states, vector<Operator>& ops,
	vector<string> goals, vector<string>& goal_stack) {
	for (auto const& goal : goals) {
		states = achieve(states, ops, goal, goal_stack);
		if (states.empty()) { return{}; } }
	for (auto const& goal : goals) {
		if (find(states.begin(), states.end(), goal) == states.end()) {
			return{}; } }
	return states; }

vector<string> apply_operator (Operator op,	vector<string> states,
	vector<Operator> ops, string goal, vector<string>& goal_stack) {
	goal_stack.push_back(goal);
	vector<string> result = achieve_all(states, ops, op.preconds, goal_stack);
	if (result.empty()) { return{}; }
	vector<string> add_list = op.add, del_list = op.remove, new_result = {};
	for (auto const& state : result) {
		if (find(del_list.begin(), del_list.end(), state) != del_list.end()) {
			continue; }
		else { new_result.push_back(state); } }
		for (auto const& state : add_list) {
			new_result.push_back(state); }
		return new_result; }

vector<string> gps1 (vector<string> init_states, vector<string> goal_states,
	vector<Operator>& operators) {
	string prefix = "Exec ";
	for (Operator& op : operators) {
		op.add.push_back(prefix + op.action); }
	vector<string> goal_stack = vector<string>();
	vector<string> final_states = achieve_all(init_states,
		operators, goal_states, goal_stack);
	vector<string> prefixed_final(final_states.size());
	if (final_states.empty()) { return{}; }
	else { auto it = copy_if(final_states.begin(), final_states.end(),
			prefixed_final.begin(), prefixed_state());
		prefixed_final.resize(distance(prefixed_final.begin(), it)); }
	return prefixed_final; }

vector<string> init1 = { "at door", "on floor", "has ball", "hungry", "chair at door" };
vector<string> goal1 = { "not hungry" };
vector<Operator> operators1 = {
	{ "climb on chair", { "chair at middle room", "at middle room", "on floor" },
	  { "at bananas", "on chair" }, { "at middle room", "on floor" } },
	{ "push chair from door to middle room", { "chair at door", "at door" },
	  { "chair at middle room", "at middle room" }, { "chair at door", "at door" } },
	{ "walk from door to middle room", { "at door", "on floor" },
	  { "at middle room" }, { "at door" } },
	{ "grasp bananas", { "at bananas", "empty handed" },
	  { "has bananas" }, { "empty handed" } },
	{ "drop ball", { "has ball" },
	  { "empty handed" }, { "has ball" } },
	{ "eat bananas", { "has bananas" },
	  { "empty handed", "not hungry" }, { "has bananas", "hungry" } } };

void main1(){
	vector<string> result1 = gps1(init1, goal1, operators1);
	printf("Steps: %d\n", result1.size());
	for (int i = 0; i < result1.size(); i++) cout << result1[i].substr(4) << "\n";
}

// ***************************************************************************

struct Execution { int op; vector<int>els; };
struct Problem { vector<int> start; vector<int> ops; vector<int> goal; };
struct Node { vector<int> state; vector<Execution> exs; int num; };

int w1 = 7, w2 = 5;
Problem problem1 = { { 0, 0 }, { 5, 4, 3, 2, 1, 0 }, { 4, 0 } };

vector<int> calcfunc (int f,vector<int>s) {
	switch (f) {
	case 0: return{ 0, s[1] };
	case 1: return{ s[0], 0 };
	case 2: return{ w1, s[1] };
	case 3: return{ s[0], w2 };
	case 4: return{ s[0] - min(s[0], w2 - s[1]), s[1] + min(s[0], w2 - s[1]) };
	case 5: return{ s[0] + min(s[1], w1 - s[0]), s[1] - min(s[1], w1 - s[0]) };
	} }

Node gps2 (Problem problem) {
	Node node, newnode;
	vector<vector<int>> states; vector<Node> nodes;
	vector<Execution> hve; Execution he;
	newnode.state = problem.start; newnode.exs = {}; newnode.num = 0;
	nodes.push_back(newnode);
	for (;;) {
		if (nodes.size() == 0) return{ {}, {}, 0 };
		node = nodes[nodes.size()-1]; nodes.pop_back();
		if (equal(node.state.begin(),node.state.end(),problem.goal.begin())){
			return node; }
		for (int opi=0; opi < problem.ops.size(); opi++) {
			int operation = problem.ops[opi];
			he.op = operation; he.els = node.state;
			hve = node.exs; hve.push_back(he);
			newnode.state = calcfunc(operation, node.state);
			newnode.exs=hve; newnode.num=node.num+1;
			int mv = 0;
			for (int i = 0; i < states.size(); i++)
				if (equal(newnode.state.begin(), newnode.state.end(), states[i].begin()))
				{ mv = 1; break; }
			if (!mv) {
				nodes.push_back(newnode);
				states.push_back(newnode.state); } } } }

void main2 () {
	Node result2 = gps2(problem1);
	printf("Steps: %d\n", result2.exs.size());
	for (int i = 0; i < result2.exs.size(); i++) {
		printf(" %d: ", result2.exs[i].op);
		for (int j = 0; j < result2.exs[i].els.size(); j++) {
			printf("%d,", result2.exs[i].els[j]);
		} printf("\n");
	} printf("\n"); }

void main () {
	main1();
	main2(); }
