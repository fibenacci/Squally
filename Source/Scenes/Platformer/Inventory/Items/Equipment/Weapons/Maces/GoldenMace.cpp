#include "GoldenMace.h"

#include "Engine/Inventory/CurrencyInventory.h"
#include "Scenes/Platformer/Inventory/Currencies/IOU.h"
#include "Scenes/Platformer/Inventory/Items/Recipes/Weapons/Maces/GoldenMacePlans.h"

#include "Resources/ItemResources.h"

#include "Strings/Strings.h"

using namespace cocos2d;

const std::string GoldenMace::SaveKey = "golden-mace";

GoldenMace* GoldenMace::create()
{
	GoldenMace* instance = new GoldenMace();

	instance->autorelease();

	return instance;
}

GoldenMace::GoldenMace() : super(CurrencyInventory::create({{ IOU::getIOUIdentifier(), 2 }}), MACE_DAMAGE_MIN(4), MACE_DAMAGE_MAX(4), ItemStats(
	// Health
	1,
	// Mana
	1,
	// Attack
	0,
	// Magic Attack
	0,
	// Armor
	0,
	// Speed
	0.05f
))
{
}

GoldenMace::~GoldenMace()
{
}

Item* GoldenMace::clone()
{
	return GoldenMace::create();
}

LocalizedString* GoldenMace::getString()
{
	return Strings::Items_Equipment_Weapons_Maces_GoldenMace::create();
}

const std::string& GoldenMace::getIconResource()
{
	return ItemResources::Equipment_Weapons_Maces_GoldenMace;
}

const std::string& GoldenMace::getIdentifier()
{
	return GoldenMace::SaveKey;
}

Vec2 GoldenMace::getDisplayOffset()
{
	return Vec2(0.0f, -24.0f);
}

Recipe* GoldenMace::getRecipe()
{
	return GoldenMacePlans::create();
}
