#pragma once

#include "Scenes/Platformer/Inventory/Items/Recipes/Weapons/Wands/WandRecipe.h"

class EmeraldWandPlans : public WandRecipe
{
public:
	static EmeraldWandPlans* create();

	Item* craft() override;
	Item* clone() override;
	std::string getItemName() override;
	LocalizedString* getString() override;
	std::string getIconResource() override;
	std::string getCraftedItemIconResource() override;
	std::string getSerializationKey() override;

	static const std::string SaveKeyEmeraldWandPlans;

protected:
	EmeraldWandPlans();
	virtual ~EmeraldWandPlans();
	
	std::vector<std::tuple<Item*, int>> getReagentsInternal() override;

private:
	typedef WandRecipe super;
};