#pragma once
#include "Component.h"
class FileComponent : public Component {
	public :
		FileComponent();
		string getPath() override;
		~FileComponent() override;
};