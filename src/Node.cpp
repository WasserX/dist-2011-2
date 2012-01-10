#include "Node.h"

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
