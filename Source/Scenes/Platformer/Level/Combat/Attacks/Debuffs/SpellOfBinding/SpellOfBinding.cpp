#include "SpellOfBinding.h"

#include "cocos/2d/CCActionInstant.h"
#include "cocos/2d/CCActionInterval.h"
#include "cocos/2d/CCSprite.h"

#include "Engine/Animations/SmartAnimationSequenceNode.h"
#include "Engine/Events/ObjectEvents.h"
#include "Engine/Hackables/HackableCode.h"
#include "Engine/Hackables/HackableObject.h"
#include "Engine/Hackables/Menus/HackablePreview.h"
#include "Engine/Localization/ConcatString.h"
#include "Engine/Localization/ConstantString.h"
#include "Engine/Optimization/LazyNode.h"
#include "Engine/Particles/SmartParticles.h"
#include "Engine/Localization/ConstantFloat.h"
#include "Engine/Sound/WorldSound.h"
#include "Engine/Utils/GameUtils.h"
#include "Engine/Utils/MathUtils.h"
#include "Entities/Platformer/PlatformerEntity.h"
#include "Events/CombatEvents.h"
#include "Events/PlatformerEvents.h"
#include "Scenes/Platformer/Hackables/HackFlags.h"
#include "Scenes/Platformer/Level/Combat/Attacks/Debuffs/SpellOfBinding/SpellOfBindingGenericPreview.h"
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

#define LOCAL_FUNC_ID_SPELL_OF_BINDING 1

const std::string SpellOfBinding::SpellOfBindingIdentifier = "spell-of-binding";

// Note: UI sets precision on these to 1 digit
const float SpellOfBinding::MinSpeed = -1.25f;
const float SpellOfBinding::DefaultSpeed = -1.25f;
const float SpellOfBinding::DefaultHackSpeed = -0.5f; // Keep in sync with the asm
const float SpellOfBinding::MaxSpeed = 2.0f;
const float SpellOfBinding::Duration = 6.0f;

// Static to prevent GCC optimization issues
volatile float SpellOfBinding::currentSpeed = 0.0f;

SpellOfBinding* SpellOfBinding::create(PlatformerEntity* caster, PlatformerEntity* target)
{
	SpellOfBinding* instance = new SpellOfBinding(caster, target);

	instance->autorelease();

	return instance;
}

SpellOfBinding::SpellOfBinding(PlatformerEntity* caster, PlatformerEntity* target)
	: super(caster, target, UIResources::Menus_Icons_Book, AbilityType::Arcane, BuffData(SpellOfBinding::Duration, SpellOfBinding::SpellOfBindingIdentifier))
{
	this->spellEffect = SmartParticles::create(ParticleResources::Platformer_Combat_Abilities_Curse);
	this->spellAura = Sprite::create(FXResources::Auras_ChantAura);

	this->spellAura->setColor(Color3B::MAGENTA);
	this->spellAura->setOpacity(0);

	this->addChild(this->spellEffect);
	this->addChild(this->spellAura);
}

SpellOfBinding::~SpellOfBinding()
{
}

void SpellOfBinding::onEnter()
{
	super::onEnter();

	this->spellEffect->setPositionY(this->owner->getEntityBottomPointRelative().y);
	this->spellEffect->start();

	this->spellAura->runAction(Sequence::create(
		FadeTo::create(0.25f, 255),
		DelayTime::create(0.5f),
		FadeTo::create(0.25f, 0),
		nullptr
	));

	CombatEvents::TriggerHackableCombatCue();
}

void SpellOfBinding::initializePositions()
{
	super::initializePositions();
}

void SpellOfBinding::registerHackables()
{
	super::registerHackables();

	if (this->owner == nullptr)
	{
		return;
	}

	HackableCode::CodeInfoMap codeInfoMap =
	{
		{
			LOCAL_FUNC_ID_SPELL_OF_BINDING,
			HackableCode::HackableCodeInfo(
				SpellOfBinding::SpellOfBindingIdentifier,
				Strings::Menus_Hacking_Abilities_Debuffs_SpellOfBinding_SpellOfBinding::create(),
				HackableBase::HackBarColor::Purple,
				UIResources::Menus_Icons_Book,
				LazyNode<HackablePreview>::create([=](){ return SpellOfBindingGenericPreview::create(); }),
				{
					{
						HackableCode::Register::zbx, Strings::Menus_Hacking_Abilities_Debuffs_SpellOfBinding_RegisterEbx::create(), HackableDataType::Int32, true
					},
				},
				int(HackFlags::None),
				this->getRemainingDuration(),
				0.0f,
				{
					HackableCode::ReadOnlyScript(
						Strings::Menus_Hacking_CodeEditor_OriginalCode::create(),
						// x86
						ConcatString::create({
							COMMENT(Strings::Menus_Hacking_Abilities_Generic_FPU_CommentFldz::create()),
							ConstantString::create("fldz\n"),
							COMMENT(Strings::Menus_Hacking_Abilities_Debuffs_SpellOfBinding_CommentPushSpeed::create()),
							ConstantString::create("fld dword ptr [ebx]\n"),
							COMMENT(Strings::Menus_Hacking_Abilities_Debuffs_SpellOfBinding_CommentCompare::create()),
							ConstantString::create("fcompp\n\n"),
							COMMENT(Strings::Menus_Hacking_Abilities_Generic_FPU_CommentConvert::create()),
							ConstantString::create("fstsw ax\n"),
							ConstantString::create("sahf\n\n"),
							COMMENT(Strings::Menus_Hacking_Abilities_Generic_Conditional_CommentJ::create()),
							COMMENT(Strings::Menus_Hacking_Abilities_Generic_Conditional_CommentB::create()),
							COMMENT(Strings::Menus_Hacking_Abilities_Generic_Conditional_CommentE::create()),
							ConstantString::create("jbe skipSpellOfBindingCode\n"),
							COMMENT(Strings::Menus_Hacking_Abilities_Debuffs_SpellOfBinding_CommentNegateSpeed::create()),
							ConstantString::create("fld dword ptr [ebx]\n"),
							ConstantString::create("fchs\n"),
							ConstantString::create("fstp dword ptr [ebx]\n"),
							ConstantString::create("skipSpellOfBindingCode:\n\n"),
							COMMENT(Strings::Menus_Hacking_Abilities_Debuffs_SpellOfBinding_CommentHint1::create()),
							COMMENT(Strings::Menus_Hacking_Abilities_Debuffs_SpellOfBinding_CommentHint2::create()),
							ConstantString::create("\n\n"),
							COMMENT(Strings::Menus_Hacking_Abilities_Generic_Stack_CommentStackBalance::create()),
							COMMENT(Strings::Menus_Hacking_Abilities_Generic_Stack_CommentStackBalanceFPUPush::create()),
							COMMENT(Strings::Menus_Hacking_Abilities_Generic_Stack_CommentStackBalanceFPUPop::create())
						})
						, // x64
						ConcatString::create({
							COMMENT(Strings::Menus_Hacking_Abilities_Generic_FPU_CommentFldz::create()),
							ConstantString::create("fldz\n"),
							COMMENT(Strings::Menus_Hacking_Abilities_Debuffs_SpellOfBinding_CommentPushSpeed::create()),
							ConstantString::create("fld dword ptr [rbx]\n"),
							COMMENT(Strings::Menus_Hacking_Abilities_Debuffs_SpellOfBinding_CommentCompare::create()),
							ConstantString::create("fcompp\n\n"),
							COMMENT(Strings::Menus_Hacking_Abilities_Generic_FPU_CommentConvert::create()),
							ConstantString::create("fstsw ax\n"),
							ConstantString::create("sahf\n"),	
							COMMENT(Strings::Menus_Hacking_Abilities_Generic_Conditional_CommentJ::create()),
							COMMENT(Strings::Menus_Hacking_Abilities_Generic_Conditional_CommentB::create()),
							COMMENT(Strings::Menus_Hacking_Abilities_Generic_Conditional_CommentE::create()),
							ConstantString::create("jbe skipSpellOfBindingCode\n"),
							COMMENT(Strings::Menus_Hacking_Abilities_Debuffs_SpellOfBinding_CommentNegateSpeed::create()),
							ConstantString::create("fld dword ptr [rbx]\n"),
							ConstantString::create("fchs\n"),
							ConstantString::create("fstp dword ptr [rbx]\n"),
							ConstantString::create("skipSpellOfBindingCode:\n\n"),
							COMMENT(Strings::Menus_Hacking_Abilities_Debuffs_SpellOfBinding_CommentHint1::create()),
							COMMENT(Strings::Menus_Hacking_Abilities_Debuffs_SpellOfBinding_CommentHint2::create()),
							ConstantString::create("\n\n"),
							COMMENT(Strings::Menus_Hacking_Abilities_Generic_Stack_CommentStackBalance::create()),
							COMMENT(Strings::Menus_Hacking_Abilities_Generic_Stack_CommentStackBalanceFPUPush::create()),
							COMMENT(Strings::Menus_Hacking_Abilities_Generic_Stack_CommentStackBalanceFPUPop::create())
						})
					)
				},
				true
			)
		},
	};

	this->hackables = CREATE_HACKABLES(SpellOfBinding::applySpellOfBinding, codeInfoMap);

	for (HackableCode* next : this->hackables)
	{
		this->owner->registerCode(next);
	}
}

void SpellOfBinding::onModifyTimelineSpeed(CombatEvents::ModifiableTimelineSpeedArgs* speed)
{
	super::onModifyTimelineSpeed(speed);
	
	this->currentSpeed = *(speed->speed);

	this->applySpellOfBinding();

	*(speed->speed) = this->currentSpeed;
}

NO_OPTIMIZE void SpellOfBinding::applySpellOfBinding()
{
	static volatile float* currentSpeedPtr;

	currentSpeedPtr = &this->currentSpeed;
	
	ASM_PUSH_EFLAGS();
	ASM(push ZAX);
	ASM(push ZBX);

	ASM_MOV_REG_PTR(ZBX, currentSpeedPtr);

	HACKABLE_CODE_BEGIN(LOCAL_FUNC_ID_SPELL_OF_BINDING);
	ASM(fldz);
	ASM(fld dword ptr [ZBX]);
	ASM(fcompp);
	ASM(fstsw ax);	// Store in AX
	ASM(sahf);		// Convert to eflags
	ASM(jbe skipSpellOfBindingCode);
	ASM(fld dword ptr [ZBX]);
	ASM(fchs) // Negate
	ASM(fstp dword ptr [ZBX]);
	ASM(skipSpellOfBindingCode:);
	ASM_NOP12();
	HACKABLE_CODE_END();

	ASM(pop ZBX);
	ASM(pop ZAX);
	ASM_POP_EFLAGS();
	HACKABLES_STOP_SEARCH();

	this->currentSpeed = MathUtils::clamp(this->currentSpeed, SpellOfBinding::MinSpeed, SpellOfBinding::MaxSpeed);
}
END_NO_OPTIMIZE
