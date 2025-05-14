#include "FolderComponent.h"
#include "FileHandler.h"

FolderComponent::FolderComponent(std::string componentName, std::shared_ptr<Component> prevComponent) : Component(componentName, prevComponent) {}

std::shared_ptr<Component> FolderComponent::getComponent() {
	std::string path = getPath();
	std::vector<std::shared_ptr<Component>> components = FileHandler::getComponents(shared_from_this());
	std::cout << "Current Path : " << path << std::endl;
	if(path != "/")	std::cout << "0. Go back" << std::endl;
	for (int i = 0; i < components.size(); i++) {
		std::cout << i + 1 << ". " << components[i]->componentName << std::endl;
	}
	std::cout << "-1. Exit" << std::endl;
	std::cout << "Enter the path to open : ";
	std::string input;
	std::cin >> input;
	int option = parsedOption(input);
	if (option == -2 || (option == 0 && path == "/")) {
		std::cout << "Wrong input. Try again!" << std::endl;
		return getComponent();
	}
	if (option == -1) {
		std::cout << "Exiting..." << std::endl;
		return nullptr;
	}
	std::shared_ptr<Component> reqComponent;
	if (option == 0) {
		reqComponent = prevComponent;
	} else {
		reqComponent = components[option - 1];
	}
	return reqComponent->getComponent();
}

int FolderComponent::parsedOption(std::string input) {
	int result = 0;
	if (input == "-1")	return -1;
	for (char ch : input) {
		if (!isdigit(ch)) {
			return -2;
		}
		result = (result * 10) + (int)ch - '0';
	}
	return result;
}