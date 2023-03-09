#include "UnholyBlade.h"

#include "cocos/2d/CCActionInstant.h"
#include "cocos/2d/CCActionInterval.h"
#include "cocos/2d/CCSprite.h"

#include "Engine/Animations/SmartAnimationSequenceNode.h"
#include "Engine/Events/ObjectEvents.h"
#include "Engine/Hackables/HackableCode.h"
#include "Engine/Hackables/HackableObject.h"
#include "Engine/Hackables/Menus/HackablePreview.h"
#include "Engine/Optimization/LazyNode.h"
#include "Engine/Particles/SmartParticles.h"
#include "Engine/Localization/ConstantString.h"
#include "Engine/Sound/WorldSound.h"
#include "Engine/Utils/GameUtils.h"
#include "Engine/Utils/MathUtils.h"
#include "Entities/Platformer/PlatformerEntity.h"
#include "Events/CombatEvents.h"
#include "Events/PlatformerEvents.h"
#include "Scenes/Platformer/Hackables/HackFlags.h"
#include "Scenes/Platformer/Level/Combat/Attacks/Debuffs/UnholyBlade/UnholyBladeGenericPreview.h"
#include "Scenes/Platformer/Level/Combat/CombatMap.h"
#include "Scenes/Platformer/Level/Combat/TimelineEvent.h"
#include "Scenes/Platformer/Level/Combat/TimelineEventGroup.h"

#include "Resources/FXResources.h"
#include "Resources/ObjectResources.h"
#include "Resources/ParticleResources.h"
#include "Resources/SoundResources.h"
#include "Resources/UIResources.h"

#include "Strings/Strings.h"

using namespace cocos2d;

#define LOCAL_FUNC_ID_CURSED_BLADE 1

const std::string UnholyBlade::UnholyBladeIdentifier = "cursed-blade";
const std::string UnholyBlade::HackIdentifierUnholyBlade = "cursed-blade";

const int UnholyBlade::MaxMultiplier = 6;
const int UnholyBlade::DamageDelt = 1;
const float UnholyBlade::Duration = 24.0f;

UnholyBlade* UnholyBlade::create(PlatformerEntity* caster, PlatformerEntity* target)
{
	UnholyBlade* instance = new UnholyBlade(caster, target);

	instance->autorelease();

	return instance;
}

UnholyBlade::UnholyBlade(PlatformerEntity* caster, PlatformerEntity* target)
	: super(caster, target, UIResources::Menus_Icons_PurpleScarabShell, AbilityType::Nature, BuffData(UnholyBlade::Duration, UnholyBlade::UnholyBladeIdentifier))
{
	this->spellEffect = SmartParticles::create(ParticleResources::Platformer_Combat_Abilities_Speed);

	this->addChild(this->spellEffect);
}

UnholyBlade::~UnholyBlade()
{
}

void UnholyBlade::onEnter()
{
	super::onEnter();

	this->spellEffect->setPositionY(this->owner->getEntityBottomPointRelative().y);
	this->spellEffect->start();

	CombatEvents::TriggerHackableCombatCue();
}

void UnholyBlade::initializePositions()
{
	super::initializePositions();
}

void UnholyBlade::registerHackables()
{
	super::registerHackables();

	if (this->owner == nullptr)
	{
		return;
	}

	HackableCode::CodeInfoMap codeInfoMap =
	{
		{
			LOCAL_FUNC_ID_CURSED_BLADE,
			HackableCode::HackableCodeInfo(
				UnholyBlade::HackIdentifierUnholyBlade,
				Strings::Menus_Hacking_Abilities_Debuffs_UnholyBlade_UnholyBlade::create(),
				HackableBase::HackBarColor::Purple,
				UIResources::Menus_Icons_PurpleScarabShell,
				LazyNode<HackablePreview>::create([=](){ return UnholyBladeGenericPreview::create(); }),
				{
					{
						HackableCode::Register::zdx, Strings::Menus_Hacking_Abilities_Debuffs_UnholyBlade_Register::create(),
					},
				},
				int(HackFlags::None),
				this->getRemainingDuration(),
				0.0f,
				{
					HackableCode::ReadOnlyScript(
						Strings::Menus_Hacking_CodeEditor_OriginalCode::create(),
						// x86
						COMMENT(Strings::Menus_Hacking_Abilities_Debuffs_UnholyBlade_CommentRepeat::create()) +
						std::string("fldz") +
						std::string("ficomp dword ptr [ebx]") +
						std::string("fstsw ax") +
						std::string("sahf") +
						std::string("jb skipCodeUnholyBlade") +
						std::string("fild dword ptr [ebx]") +
						std::string("fistp dword ptr [edx]") +
						std::string("fldz") +
						std::string("fistp dword ptr [ebx]") +
						std::string("skipCodeUnholyBlade:")
						, // x64
						COMMENT(Strings::Menus_Hacking_Abilities_Debuffs_UnholyBlade_CommentRepeat::create()) +
						std::string("fldz") +
						std::string("ficomp dword ptr [rbx]") +
						std::string("fstsw ax") +
						std::string("sahf") +
						std::string("jb skipCodeUnholyBlade") +
						std::string("fild dword ptr [rbx]") +
						std::string("fistp dword ptr [rdx]") +
						std::string("fldz") +
						std::string("fistp dword ptr [rbx]") +
						std::string("skipCodeUnholyBlade:")
					),
				},
				true
			)
		},
	};

	auto func = &UnholyBlade::applyUnholyBlade;
	this->hackables = HackableCode::create((void*&)func, codeInfoMap);

	for (HackableCode* next : this->hackables)
	{
		this->owner->registerCode(next);
	}
}

void UnholyBlade::onBeforeDamageDealt(CombatEvents::ModifiableDamageOrHealingArgs* damageOrHealing)
{
	super::onBeforeDamageDealt(damageOrHealing);

	Buff::HackStateStorage[Buff::StateKeyDamageDealt] = Value(UnholyBlade::DamageDelt);

	this->applyUnholyBlade();

	int min = -std::abs(Buff::HackStateStorage[Buff::StateKeyOriginalDamageOrHealing].asInt() * UnholyBlade::MaxMultiplier);
	int max = std::abs(Buff::HackStateStorage[Buff::StateKeyOriginalDamageOrHealing].asInt() * UnholyBlade::MaxMultiplier);

	*damageOrHealing->damageOrHealing = Buff::HackStateStorage[Buff::StateKeyDamageDealt].asInt();
	*damageOrHealing->damageOrHealingMin = min;
	*damageOrHealing->damageOrHealingMax = max;
}

NO_OPTIMIZE void UnholyBlade::applyUnholyBlade()
{
	static volatile int currentDamageDealtLocal = 0;
	static volatile int* currentDamageDealtLocalPtr = nullptr;
	static volatile int currentHealingLocal = 0;
	static volatile int* currentHealingLocalPtr = nullptr;

	currentDamageDealtLocal = GameUtils::getKeyOrDefault(Buff::HackStateStorage, Buff::StateKeyDamageDealt, Value(0)).asInt();
	currentDamageDealtLocalPtr = &currentDamageDealtLocal;
	currentHealingLocal = 0;
	currentHealingLocalPtr = &currentHealingLocal;
	
	ASM_PUSH_EFLAGS();
	ASM(push ZAX);
	ASM(push ZBX);
	ASM(push ZDX);

	ASM(MOV ZBX, 0)
	ASM_MOV_REG_VAR(ebx, currentDamageDealtLocal);

	ASM(MOV ZDX, 0)
	ASM_MOV_REG_VAR(edx, currentHealingLocal);

	HACKABLE_CODE_BEGIN(LOCAL_FUNC_ID_CURSED_BLADE);
	ASM(fldz);	// Load 0
	ASM(ficomp dword ptr [ZBX]); // Compare to damage
	ASM(fstsw ax);	// Store in AX
	ASM(sahf);		// Convert to eflags
	ASM(jb skipCodeUnholyBlade);
	ASM(fild dword ptr [ZBX]);
	ASM(fistp dword ptr [ZDX]);
	ASM(fldz);
	ASM(fistp dword ptr [ZBX]);
	ASM(skipCodeUnholyBlade:);
	ASM_NOP12();
	HACKABLE_CODE_END();

	ASM(pop ZDX);
	ASM(pop ZBX);
	ASM(pop ZAX);
	ASM_POP_EFLAGS();
	HACKABLES_STOP_SEARCH();

	Buff::HackStateStorage[Buff::StateKeyDamageDealt] = Value(currentDamageDealtLocal);

	HACKABLES_STOP_SEARCH();
}
END_NO_OPTIMIZE