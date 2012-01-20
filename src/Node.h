#ifndef NODE_H
#define NODE_H

#include <string>
#include <list>
#include <iostream>

class Node {
public:
	Node();
	Node(std::string ruleName);
	~Node();
	inline void setRule(bool value) { rule = value; };
	inline void setFinished() { hasFinished = true; };
	inline void setComputing() { computing = true; };
	inline void setCommand(std::string com) { command = com; };
	inline void addDependency() { dependencies++; };
	inline void addNeeds(Node* node) { needs.push_back(node); };
	inline void addResolves(Node* dep) { solves.push_back(dep); };
	inline void addTerminal(std::string terminal) { terminals.push_back(terminal); };
	inline const std::list<Node*>& getResolves() { return solves; };
	inline int getDependencies() { return dependencies; };
	inline std::list<Node*>& getNeeds() { return needs; };
	inline std::list<std::string> getTerminals() { return terminals; };
	inline std::string getNodeName() { return nodeName; };
	inline std::string getCommand() { return command; };
	inline void remDependency() { if(dependencies > 0) dependencies--; };
	inline void remNeeds(Node* node) { needs.remove(node); };
	inline bool isFinished() { return hasFinished; };
	inline bool isRule() { return rule; };
	inline bool isReady() { return dependencies == 0; };
	inline bool isComputing() { return computing; };
	void toString();

private:
	bool hasFinished;
	bool rule;
	bool computing;
	std::string nodeName;
	int dependencies;
	std::list<Node*> solves;
	std::list<Node*> needs; //Used to create used graph only, will be destroyed after parsing
	std::list<std::string> terminals;
	std::string command;
};

#endif //NODE_H
