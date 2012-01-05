#ifndef NODE_H
#define NODE_H

#include <string>
#include <list>
#include <set>

class Node {
	private:
		bool hasFinished;
		std::list<Node*> dependencies;
		std::list<Node*> solves;
		std::list<std::string> terminals;
		std::string command;

	public:
		Node();
		~Node();	
		inline void updateDependencies(Node* node) { dependencies.remove(node); };
		inline bool isReady() { return dependencies.empty(); };
		
};

#endif //NODE_H
