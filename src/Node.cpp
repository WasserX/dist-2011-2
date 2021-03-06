#include "Node.h"

Node::Node() {
	nodeName = "";
	rule = false;
	hasFinished = false;
	dependencies = 0;
	computing = false;
}

Node::Node(std::string ruleName) { 
	nodeName = ruleName;
	rule = false;
	hasFinished = false;
	dependencies = 0;
	computing = false;
}

Node::~Node() {
}

void Node::toString(){
	using namespace std;

	cout << "Node: " << nodeName << endl;
	cout << "Is a Rule? " << rule << endl;
	cout << "Has Finished? " << hasFinished << endl;
	cout << "Command: " << command << endl;

	cout << "Dependencies: " << dependencies << endl;

	cout << "Solves: ";
	for(list<Node*>::iterator it = solves.begin(); it != solves.end(); it++)
		cout << (*it)->getNodeName() << " ";
	cout << endl;

	cout << "Terminals: ";
	for(list<string>::iterator it = terminals.begin(); it != terminals.end(); it++)
		cout << *it << " ";

	cout << endl << endl;
}
