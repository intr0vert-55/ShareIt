#include"FileComponent.h"

FileComponent::FileComponent(std::string componentName, std::shared_ptr<Component> prevComponent) : Component(componentName, prevComponent) {}

std::shared_ptr<Component> FileComponent::getComponent() {
	return shared_from_this();
}