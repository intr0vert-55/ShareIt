#pragma once
#include <iostream>
using namespace std;
class Component {		// Interface -> Composite Pattern
	protected :
		string component;
		Component* prevComponent;
	public :
		virtual string getPath() = 0;
		virtual ~Component() = 0;
};