#include "Component.h"


Component::Component(std::string componentName, std::shared_ptr<Component> prevComponent) 
	: componentName(componentName), prevComponent(prevComponent) {}


std::string Component::getPath() {	// Not working as expected when we share this as file
	if (path == "") {
		path = "/";		// default or root path
		std::shared_ptr<Component> parentComponent = prevComponent;
		if (parentComponent != nullptr) {
			std::string parentPath = parentComponent->getPath();
			path = ((parentPath == "/") ? "" : parentPath) + "/" + componentName;
		}
	}
	return path;
}