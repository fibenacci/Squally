﻿#include "JnoPage.h"

#include "cocos/2d/CCSprite.h"

#include "Engine/Events/HackableEvents.h"
#include "Engine/Hackables/Menus/CodeEditor/Lexicon/Pages/ControlFlow/ControlFlowIntroPage.h"
#include "Engine/Hackables/Menus/CodeEditor/Lexicon/Pages/ControlFlow/ControlFlowSelectPage.h"
#include "Engine/Localization/LocalizedLabel.h"

#include "Resources/UIResources.h"

#include "Strings/Strings.h"

using namespace cocos2d;

const std::string JnoPage::Identifier = "JNO_PAGE";

JnoPage* JnoPage::create()
{
	JnoPage* instance = new JnoPage();

	instance->autorelease();

	return instance;
}

JnoPage::JnoPage() : super(JnoPage::Identifier, PageType::Full)
{
	this->decorSprite = Sprite::create(UIResources::Menus_LexiconMenu_BookArt_Bread);
	this->chapterSprite = Sprite::create(UIResources::Menus_LexiconMenu_Chapter);
	this->titleLabel = LocalizedLabel::create(LocalizedLabel::FontStyle::Coding, LocalizedLabel::FontSize::H2, Strings::Menus_Hacking_Lexicon_Instructions_ControlFlow_Jno::create());
	this->introText = LocalizedLabel::create(
		LocalizedLabel::FontStyle::Main,
		LocalizedLabel::FontSize::P,
		Strings::Menus_Hacking_Lexicon_Pages_ControlFlow_Jno_Intro::create(),
		CSize(super::PageSize.width - 160.0f, super::PageSize.height)
	);

	this->titleLabel->setTextColor(super::TextColor);
	this->titleLabel->setAnchorPoint(Vec2(0.0f, 1.0f));
	this->introText->setTextColor(super::TextColor);
	this->introText->setAnchorPoint(Vec2(0.0f, 1.0f));

	this->addChild(this->decorSprite);
	this->addChild(this->chapterSprite);
	this->addChild(this->titleLabel);
	this->addChild(this->introText);
}

JnoPage::~JnoPage()
{
}

void JnoPage::initializePositions()
{
	super::initializePositions();

	this->decorSprite->setPosition(Vec2(32.0f, -160.0f));
	this->introText->setPosition(super::IntroLocation);
	this->chapterSprite->setPosition(super::ChapterMarkerLocation);
	this->titleLabel->setPosition(super::ChapterLocation);
}

void JnoPage::initializeListeners()
{
	super::initializeListeners();

	this->enableBack(ControlFlowIntroPage::Identifier, ControlFlowSelectPage::Identifier);
}
