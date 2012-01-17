#ifndef NODE_H
#define NODE_H

#include <string>
#include <list>
#include <iostream>

class Node {
public:
	Node();
	inline Node(std::string ruleName) { nodeName = ruleName; rule = false; hasFinished = false; };
	~Node();
	inline void setRule(bool value) { rule = value; };
	inline void setFinished(bool value) { hasFinished = value; };
	inline void setCommand(std::string com) { command = com; };
	inline void addDependency() { dependencies++; };
	inline void addResolves(Node* dep) { solves.push_back(dep); };
	inline void addTerminal(std::string terminal) { terminals.push_back(terminal); };
	inline std::list<Node*> getResolves() { return solves; };
	inline int getDependencies() { return dependencies; };
	inline std::list<std::string> getTerminals() { return terminals; };
	inline std::string getNodeName() { return nodeName; };
	inline std::string getCommand() { return command; };
	inline void remDependency() { if(dependencies > 0) dependencies--; };
	inline bool isFinished() { return hasFinished; };
	inline bool isRule() { return rule; };
	inline bool isReady() { return dependencies == 0; };
	void toString();

private:
	bool hasFinished;
	bool rule;
	std::string nodeName;
	int dependencies;
	std::list<Node*> solves;
	std::list<std::string> terminals;
	std::string command;
};

#endif //NODE_H
