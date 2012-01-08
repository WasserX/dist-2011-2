#ifndef NODE_H
#define NODE_H

#include <string>
#include <list>
#include <iostream>

class Node {
public:
	Node();
	inline Node(std::string ruleName) { nodeName = ruleName; isRule = false; hasFinished = false; };
	~Node();
	inline void setRule(bool value) { isRule = value; };
	inline void setFinished(bool value) { hasFinished = value; };
	inline void setCommand(std::string com) { command = com; };
	inline void addDependency(Node* dep) { dependencies.push_back(dep); };
	inline void addResolves(Node* dep) { solves.push_back(dep); };
	inline void addTerminal(std::string terminal) { terminals.push_back(terminal); };
	inline std::list<Node*> getDependencies() { return dependencies; };
	inline std::list<std::string> getTerminals() { return terminals; };
	inline std::string getNodeName() { return nodeName; };
	inline std::string getCommand() { return command; };
	inline void updateDependencies(Node* node) { dependencies.remove(node); };
	inline bool isFinished() { return hasFinished; };
	inline bool isReady() { return dependencies.empty(); };
	void toString();

private:
	bool hasFinished;
	bool isRule;
	std::string nodeName;
	std::list<Node*> dependencies;
	std::list<Node*> solves;
	std::list<std::string> terminals;
	std::string command;
};

Node::Node() {
	nodeName = "";
	isRule = false;
	hasFinished = false;
}

Node::~Node() {
}

void Node::toString(){
	using namespace std;

	cout << "Node: " << nodeName << endl;
	cout << "Is a Rule? " << isRule << endl;
	cout << "Has Finished? " << hasFinished << endl;
	cout << "Command: " << command << endl;

	cout << "Dependencies: ";
	for(list<Node*>::iterator it = dependencies.begin(); it != dependencies.end(); it++)
		cout << (*it)->getNodeName() << " ";
	cout << endl;

	cout << "Solves: ";
	for(list<Node*>::iterator it = solves.begin(); it != solves.end(); it++)
		cout << (*it)->getNodeName() << " ";
	cout << endl;

	cout << "Terminals: ";
	for(list<string>::iterator it = terminals.begin(); it != terminals.end(); it++)
		cout << *it << " ";
	cout << endl << endl;
}

#endif //NODE_H
