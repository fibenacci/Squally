#pragma once

#include "Engine/SmartNode.h"

namespace cocos2d
{
	class DrawNode;
}

class LocalizedLabel;
class LocalizedString;
 
class TextPanel : public SmartNode
{
public:
	static TextPanel* create(LocalizedString* localizedString);

	void show(float duration = 0.0f);
	void hide(float duration = 0.0f);

protected:
	TextPanel(LocalizedString* localizedString);
	virtual ~TextPanel();

private:
	typedef SmartNode super;

	void initializePositions() override;
	void initializeListeners() override;

	cocos2d::DrawNode* panel = nullptr;
	LocalizedLabel* text = nullptr;

	static const cocos2d::Color4F PanelColor;
	static const cocos2d::Color4F PanelEdgeColor;
	static const cocos2d::Color4B PanelTextColor;
	static const float PanelBorderSize;
};
