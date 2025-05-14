#pragma once
#include "Component.h"
#include <vector>

class FolderComponent : public Component {
	private :
		std::vector<std::shared_ptr<Component>> components;
		int parsedOption(std::string input);
	public :
		FolderComponent(std::string componentName, std::shared_ptr<Component> prevComponent);
		std::shared_ptr<Component> getComponent() override;
};