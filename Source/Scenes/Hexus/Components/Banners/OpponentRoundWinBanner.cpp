#include "OpponentRoundWinBanner.h"

#include "cocos/base/CCDirector.h"

#include "Scenes/Hexus/GameState.h"
#include "Scenes/Hexus/HexusConfig.h"

#include "Resources/HexusResources.h"

#include "Strings/Strings.h"

using namespace cocos2d;

OpponentRoundWinBanner* OpponentRoundWinBanner::create()
{
	OpponentRoundWinBanner* instance = new OpponentRoundWinBanner();

	instance->autorelease();

	return instance;
}

OpponentRoundWinBanner::OpponentRoundWinBanner()
{
	this->playerPassBanner1 = Sprite::create(HexusResources::EnemyBanner);
	this->playerPassBanner2 = Sprite::create(HexusResources::EnemyBanner);

	this->setBannerText(Strings::Hexus_Banners_OpponentRoundWin::create());

	this->addBannerChild(this->playerPassBanner1);
	this->addBannerChild(this->playerPassBanner2);
}

OpponentRoundWinBanner::~OpponentRoundWinBanner()
{
}

void OpponentRoundWinBanner::initializePositions()
{
	super::initializePositions();

	CSize visibleSize = Director::getInstance()->getVisibleSize();

	this->playerPassBanner1->setPosition(visibleSize.width / 2.0f + HexusConfig::centerColumnCenter - HexusConfig::bannerIconOffset, visibleSize.height / 2.0f + 320.0f);
	this->playerPassBanner2->setPosition(visibleSize.width / 2.0f + HexusConfig::centerColumnCenter + HexusConfig::bannerIconOffset, visibleSize.height / 2.0f + 320.0f);
}

void OpponentRoundWinBanner::onBeforeStateChange(GameState* gameState)
{
	ComponentBase::onBeforeStateChange(gameState);

	if (gameState->stateType == GameState::StateType::RoundEnd && gameState->isEnemyWinningRound())
	{
		this->flashBanner();
	}
}

void OpponentRoundWinBanner::onAnyStateChange(GameState* gameState)
{
	super::onAnyStateChange(gameState);
}
