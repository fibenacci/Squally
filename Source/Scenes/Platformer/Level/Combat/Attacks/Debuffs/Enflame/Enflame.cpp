#include "Enflame.h"

#include "cocos/2d/CCActionInstant.h"
#include "cocos/2d/CCActionInterval.h"
#include "cocos/2d/CCSprite.h"

#include "Engine/Animations/SmartAnimationSequenceNode.h"
#include "Engine/Events/ObjectEvents.h"
#include "Engine/Hackables/HackableCode.h"
#include "Engine/Hackables/HackableObject.h"
#include "Engine/Hackables/Menus/HackablePreview.h"
#include "Engine/Optimization/LazyNode.h"
#include "Engine/Sound/WorldSound.h"
#include "Engine/Utils/GameUtils.h"
#include "Engine/Utils/MathUtils.h"
#include "Entities/Platformer/PlatformerEntity.h"
#include "Events/CombatEvents.h"
#include "Events/PlatformerEvents.h"
#include "Scenes/Platformer/Hackables/HackFlags.h"
#include "Scenes/Platformer/Level/Combat/Attacks/Debuffs/Enflame/EnflameGenericPreview.h"
#include "Scenes/Platformer/Level/Combat/CombatMap.h"
#include "Scenes/Platformer/Level/Combat/TimelineEvent.h"
#include "Scenes/Platformer/Level/Combat/TimelineEventGroup.h"

#include "Resources/FXResources.h"
#include "Resources/ItemResources.h"
#include "Resources/SoundResources.h"
#include "Resources/UIResources.h"

#include "Strings/Strings.h"

using namespace cocos2d;

#define LOCAL_FUNC_ID_ENFLAME 1

const std::string Enflame::EnflameIdentifier = "enflame";
const int Enflame::DamageAmount = 5;
const int Enflame::DamageAmountMax = 8;
const float Enflame::TimeBetweenTicks = 0.5f;
const float Enflame::StartDelay = 0.25f;

Enflame* Enflame::create(PlatformerEntity* caster, PlatformerEntity* target)
{
	Enflame* instance = new Enflame(caster, target);

	instance->autorelease();

	return instance;
}

Enflame::Enflame(PlatformerEntity* caster, PlatformerEntity* target)
	: super(caster, target, UIResources::Menus_Icons_Fire, AbilityType::Shadow, BuffData())
{
	this->healEffect = SmartAnimationSequenceNode::create(FXResources::Heal_Heal_0000);
	this->healAmount = Enflame::DamageAmount;
	this->impactSound = WorldSound::create(SoundResources::Platformer_Spells_Heal2);
	this->healSound = WorldSound::create(SoundResources::Platformer_Spells_Ding1);

	this->addChild(this->healEffect);
	this->addChild(this->impactSound);
	this->addChild(this->healSound);
}

Enflame::~Enflame()
{
}

void Enflame::onEnter()
{
	super::onEnter();

	this->runEnflame();

	CombatEvents::TriggerHackableCombatCue();
}

void Enflame::initializePositions()
{
	super::initializePositions();

	this->setPosition(Vec2(0.0f, 118.0f - this->owner->getEntityCenterPoint().y));
}

void Enflame::registerHackables()
{
	super::registerHackables();

	if (this->owner == nullptr)
	{
		return;
	}

	HackableCode::CodeInfoMap codeInfoMap =
	{
		{
			LOCAL_FUNC_ID_ENFLAME,
			HackableCode::HackableCodeInfo(
				Enflame::EnflameIdentifier,
				Strings::Menus_Hacking_Abilities_Debuffs_Enflame_Enflame::create(),
				HackableBase::HackBarColor::Red,
				UIResources::Menus_Icons_Fire,
				LazyNode<HackablePreview>::create([=](){ return EnflameGenericPreview::create(); }),
				{
					{
						HackableCode::Register::zdi, Strings::Menus_Hacking_Abilities_Debuffs_Enflame_RegisterEdi::create(),
					},
					{
						HackableCode::Register::zsi, Strings::Menus_Hacking_Abilities_Debuffs_Enflame_RegisterEsi::create(),
					}
				},
				int(HackFlags::None),
				Enflame::StartDelay + Enflame::TimeBetweenTicks * float(Enflame::DamageAmount),
				0.0f,
				{
					HackableCode::ReadOnlyScript(
						Strings::Menus_Hacking_CodeEditor_OriginalCode::create(),
						// x86
						COMMENT(Strings::Menus_Hacking_Abilities_Debuffs_Enflame_CommentRng::create()) +
						"cmp esi, 0\n" +
						COMMENT(Strings::Menus_Hacking_Abilities_Debuffs_Enflame_CommentJnz::create()) +
						"jnz radiation\n\n" +
						COMMENT(Strings::Menus_Hacking_Abilities_Debuffs_Enflame_CommentJmp::create()) +
						"jmp radiationSkip\n\n" +
						COMMENT(Strings::Menus_Hacking_Abilities_Debuffs_Enflame_CommentApplyDamage::create()) +
						"mov edi, 5:\n" + // Enflame::DamageAmount
						"radiation:\n" +
						"radiationSkip:\n\n"
						, // x64
						COMMENT(Strings::Menus_Hacking_Abilities_Debuffs_Enflame_CommentRng::create()) +
						"cmp rsi, 0\n" +
						COMMENT(Strings::Menus_Hacking_Abilities_Debuffs_Enflame_CommentJnz::create()) +
						"jnz radiation\n\n" +
						"jmp radiationSkip\n\n" +
						COMMENT(Strings::Menus_Hacking_Abilities_Debuffs_Enflame_CommentApplyDamage::create()) +
						"mov rdi, 5:\n" + // Enflame::DamageAmount
						"radiation:\n" +
						COMMENT(Strings::Menus_Hacking_Abilities_Debuffs_Enflame_CommentJmp::create()) +
						"radiationSkip:\n\n"
					),
				},
				true
			)
		},
	};

	auto restoreFunc = &Enflame::runEnflameTick;
	this->hackables = HackableCode::create((void*&)restoreFunc, codeInfoMap);

	for (HackableCode* next : this->hackables)
	{
		this->owner->registerCode(next);
	}
}

void Enflame::runEnflame()
{
	this->impactSound->play();

	std::vector<TimelineEvent*> timelineEvents = std::vector<TimelineEvent*>();

	for (int healIndex = 0; healIndex < this->healAmount; healIndex++)
	{
		Sprite* icon = Sprite::create(UIResources::Menus_Icons_BloodGoblet);

		icon->setScale(0.5f);

		timelineEvents.push_back(TimelineEvent::create(
				this->owner,
				icon,
				Enflame::TimeBetweenTicks * float(healIndex) + Enflame::StartDelay, [=]()
			{
				if (!this->healEffect->isPlayingAnimation())
				{
					this->healEffect->playAnimation(FXResources::Heal_Heal_0000, 0.05f);
				}
				
				this->runEnflameTick();
			})
		);
	}

	CombatEvents::TriggerRegisterTimelineEventGroup(CombatEvents::RegisterTimelineEventGroupArgs(
		TimelineEventGroup::create(timelineEvents, this, this->owner, [=]()
		{
			this->removeBuff();
		})
	));
}

NO_OPTIMIZE void Enflame::runEnflameTick()
{
	static volatile int drainAmount = 0;
	static volatile int rng = 0;

	drainAmount = 0;
	rng = RandomHelper::random_int(0, 1);

	ASM_PUSH_EFLAGS()
	ASM(push ZDI);
	ASM(push ZSI);

	ASM(mov ZDI, 0);
	ASM_MOV_REG_VAR(esi, rng);

	HACKABLE_CODE_BEGIN(LOCAL_FUNC_ID_ENFLAME);
	ASM(cmp ZSI, 0);
	ASM(jnz radiation);
	ASM(jmp radiationSkip);
	ASM(radiation:);
	ASM(mov ZDI, 5); // Enflame::DamageAmount
	ASM(radiationSkip:);
	HACKABLE_CODE_END();

	ASM_MOV_VAR_REG(drainAmount, edi);

	ASM(pop ZSI);
	ASM(pop ZDI);
	ASM_POP_EFLAGS()

	drainAmount = MathUtils::clamp(drainAmount, -Enflame::DamageAmountMax, Enflame::DamageAmountMax);

	this->healSound->play();
	CombatEvents::TriggerDamage(CombatEvents::DamageOrHealingArgs(this->caster, this->owner, drainAmount, this->abilityType));

	HACKABLES_STOP_SEARCH();
}
END_NO_OPTIMIZE