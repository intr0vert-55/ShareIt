#pragma once
#include "Component.h"
class FileComponent : public Component {
	public :
		FileComponent(std::string componentName, std::shared_ptr<Component> prevComponent);
		std::shared_ptr<Component> getComponent() override;
};