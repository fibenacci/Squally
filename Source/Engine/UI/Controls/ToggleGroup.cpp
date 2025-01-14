#include "ToggleGroup.h"

#include "cocos/base/ccMacros.h"

#include "Engine/UI/Controls/Checkbox.h"

using namespace cocos2d;

ToggleGroup* ToggleGroup::create(std::function<void(Checkbox*)> callback)
{
	ToggleGroup* instance = new ToggleGroup(callback);

	instance->autorelease();

	return instance;
}

ToggleGroup::ToggleGroup(std::function<void(Checkbox*)> callback)
{
	this->callback = callback;
}

ToggleGroup::~ToggleGroup()
{
}

void ToggleGroup::addToggle(Checkbox* toggle)
{
	toggle->setCallback(CC_CALLBACK_2(ToggleGroup::onToggleClick, this));

	this->toggles.push_back(toggle);

	this->addChild(toggle);
}

bool ToggleGroup::onToggleClick(Checkbox* toggle, bool willToggle)
{
	// Disable all other toggles
	for (auto next : this->toggles)
	{
		if (next != toggle)
		{
			next->setToggled(false);
		}
	}

	// One toggle must always be active -- always return true (remain toggled) if attempting to deactivate the only active toggle in this group
	if (toggle->getToggled() && !willToggle)
	{
		willToggle = true;
	}

	if (willToggle && this->callback != nullptr)
	{
		// Trigger the callback indicating the active toggle in this group has changed
		this->callback(toggle);
	}

	return willToggle;
}
