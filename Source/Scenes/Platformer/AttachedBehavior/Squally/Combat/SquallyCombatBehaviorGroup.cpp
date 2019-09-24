#include "SquallyCombatBehaviorGroup.h"

#include "Engine/Maps/GameObject.h"
#include "Scenes/Platformer/AttachedBehavior/Entities/Combat/EntityCombatBehaviorGroup.h"
#include "Scenes/Platformer/AttachedBehavior/Helpers/ScrappyManagerBehavior.h"
#include "Scenes/Platformer/AttachedBehavior/Squally/Abilities/SquallyHackingBehavior.h"
#include "Scenes/Platformer/AttachedBehavior/Squally/Abilities/SquallySensingBehavior.h"
#include "Scenes/Platformer/AttachedBehavior/Squally/Combat/SquallyAttackBehavior.h"
#include "Scenes/Platformer/AttachedBehavior/Squally/Movement/SquallyFloatBehavior.h"
#include "Scenes/Platformer/AttachedBehavior/Squally/Stats/SquallyStatsBehaviorGroup.h"
#include "Scenes/Platformer/AttachedBehavior/Squally/Visual/SquallyVisualBehaviorGroup.h"

using namespace cocos2d;

const std::string SquallyCombatBehaviorGroup::MapKeyAttachedBehavior = "squally-combat-group";

SquallyCombatBehaviorGroup* SquallyCombatBehaviorGroup::create(GameObject* owner)
{
	SquallyCombatBehaviorGroup* instance = new SquallyCombatBehaviorGroup(owner);

	instance->autorelease();

	return instance;
}

SquallyCombatBehaviorGroup::SquallyCombatBehaviorGroup(GameObject* owner) : super(owner, {
	EntityCombatBehaviorGroup::create(owner),
	ScrappyManagerBehavior::create(owner),
	SquallyHackingBehavior::create(owner),
	SquallySensingBehavior::create(owner),
	SquallyAttackBehavior::create(owner),
	SquallyFloatBehavior::create(owner),
	SquallyStatsBehaviorGroup::create(owner),
	SquallyVisualBehaviorGroup::create(owner),
	})
{
}

SquallyCombatBehaviorGroup::~SquallyCombatBehaviorGroup()
{
}

void SquallyCombatBehaviorGroup::onLoad()
{
}