#include "HexusPoolCVGeneric.h"

#include "cocos/base/CCValue.h"

#include "Engine/Inventory/ItemChance.h"
#include "Objects/Platformer/ItemPools/Tiered/Tier4/AlchemyPoolTier4.h"
#include "Objects/Platformer/ItemPools/Tiered/Tier4/CardPoolTier4.h"
#include "Objects/Platformer/ItemPools/Tiered/Tier4/SmithingPoolTier4.h"
#include "Objects/Platformer/ItemPools/Tiered/Tier4/PotionPoolTier4.h"
#include "Scenes/Platformer/Inventory/Items/PlatformerItems.h"

using namespace cocos2d;

HexusPoolCVGeneric* HexusPoolCVGeneric::create()
{
	HexusPoolCVGeneric* instance = new HexusPoolCVGeneric();

	instance->autorelease();

	return instance;
}

HexusPoolCVGeneric::HexusPoolCVGeneric() : super(ValueMap(), "hexus-pool-cv-generic", SampleMethod::Random, 1, 2,
	{ CardPoolTier4::create(SampleMethod::Guarantee, 1, 1), AlchemyPoolTier4::create(SampleMethod::Random, 1, 1), SmithingPoolTier4::create(SampleMethod::Random, 1, 2) })
{
}

HexusPoolCVGeneric::~HexusPoolCVGeneric()
{
}