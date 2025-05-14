#pragma once
#include <iostream>
#include <memory>
class Component : public std::enable_shared_from_this<Component> {		// Abstract class -> Composite Pattern | enable_shared_from_this is like marker interfaces that we use in JAVA
	public :
		std::string componentName;
		std::shared_ptr<Component> prevComponent;
		virtual std::shared_ptr<Component> getComponent() = 0;
		std::string getPath();
	protected :
		Component(std::string component, std::shared_ptr<Component> prevComponent);
	private :
		std::string path;
};