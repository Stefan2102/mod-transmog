#include "Transmog.h"

bool TransmogRules_IsAllowed(uint32 entry);
bool TransmogRules_IsValidOffhandArmor(uint32 subClass, uint32 invType);
bool TransmogRules_IsTieredArmorSubclass(uint32 subClass);
bool TransmogRules_TierAvailable(Player const* player, uint32 tier);

namespace
{
    bool IsInvTypeMismatchAllowed(ItemTemplate const* source, ItemTemplate const* target)
    {
        uint32 sourceType = source->InventoryType;
        uint32 targetType = target->InventoryType;
        uint32 sourceClass = source->Class;
        uint32 targetClass = target->Class;
        uint32 sourceSub = source->SubClass;

        if (targetClass == ITEM_CLASS_WEAPON)
        {
            if (TransmogRules_IsRangedWeapon(sourceClass, sourceSub))
                return true;

            if (sTransmog->AllowMixedWeaponTypes == 2)
                return true;

            if (targetType == INVTYPE_WEAPONMAINHAND || targetType == INVTYPE_WEAPONOFFHAND)
            {
                if (sourceType == INVTYPE_WEAPONMAINHAND || sourceType == INVTYPE_WEAPONOFFHAND)
                    return sTransmog->AllowMixedWeaponHandedness;
                if (sourceType == INVTYPE_WEAPON)
                    return true;
            }
            else if (targetType == INVTYPE_WEAPON)
            {
                return sourceType == INVTYPE_WEAPONMAINHAND ||
                    (sTransmog->AllowMixedWeaponHandedness && sourceType == INVTYPE_WEAPONOFFHAND);
            }
        }
        else if (targetClass == ITEM_CLASS_ARMOR)
        {
            if (targetType == INVTYPE_CHEST || targetType == INVTYPE_ROBE)
                return sourceType == INVTYPE_CHEST || sourceType == INVTYPE_ROBE;
        }

        return false;
    }

    bool IsSubclassMismatchAllowed(Player const* player, ItemTemplate const* source, ItemTemplate const* target)
    {
        if (TransmogRules_IsAllowed(source->ItemId))
            return true;

        uint32 sourceType = source->InventoryType;
        uint32 targetType = target->InventoryType;
        uint32 sourceClass = source->Class;
        uint32 targetClass = target->Class;
        uint32 sourceSub = source->SubClass;
        uint32 targetSub = target->SubClass;

        if (targetClass == ITEM_CLASS_WEAPON)
        {
            if (TransmogRules_IsRangedWeapon(sourceClass, sourceSub))
                return true;

            if (sTransmog->AllowMixedWeaponTypes == 1)
            {
                switch (targetSub)
                {
                    case ITEM_SUBCLASS_WEAPON_AXE:
                    case ITEM_SUBCLASS_WEAPON_SWORD:
                    case ITEM_SUBCLASS_WEAPON_MACE:
                        return sourceSub == ITEM_SUBCLASS_WEAPON_AXE ||
                            sourceSub == ITEM_SUBCLASS_WEAPON_SWORD ||
                            sourceSub == ITEM_SUBCLASS_WEAPON_MACE;
                    case ITEM_SUBCLASS_WEAPON_AXE2:
                    case ITEM_SUBCLASS_WEAPON_SWORD2:
                    case ITEM_SUBCLASS_WEAPON_MACE2:
                    case ITEM_SUBCLASS_WEAPON_STAFF:
                    case ITEM_SUBCLASS_WEAPON_POLEARM:
                        return sourceSub == ITEM_SUBCLASS_WEAPON_AXE2 ||
                            sourceSub == ITEM_SUBCLASS_WEAPON_SWORD2 ||
                            sourceSub == ITEM_SUBCLASS_WEAPON_MACE2 ||
                            sourceSub == ITEM_SUBCLASS_WEAPON_STAFF ||
                            sourceSub == ITEM_SUBCLASS_WEAPON_POLEARM;
                }
            }
            else if (sTransmog->AllowMixedWeaponTypes == 2)
            {
                return true;
            }

            if (sourceSub == ITEM_SUBCLASS_WEAPON_MISC)
                return sourceType == targetType;
        }
        else if (targetClass == ITEM_CLASS_ARMOR)
        {
            if (sTransmog->AllowMixedArmorTypes)
                return true;

            if (sTransmog->AllowMixedOffhandArmorTypes &&
                TransmogRules_IsValidOffhandArmor(targetSub, targetType) &&
                TransmogRules_IsValidOffhandArmor(sourceSub, sourceType))
                return true;

            if (sTransmog->AllowLowerTiers && TransmogRules_IsTieredArmorSubclass(targetSub) &&
                TransmogRules_TierAvailable(player, sourceSub))
                return true;

            if (sourceSub == ITEM_SUBCLASS_ARMOR_MISC)
                return sourceType == targetType;
        }

        return false;
    }
}

bool TransmogRules_CanTransmogrifyItemWithItem(Player const* player, ItemTemplate const* target, ItemTemplate const* source)
{
    if (!target || !source)
        return false;

    if (source->ItemId == target->ItemId)
        return false;

    if (source->DisplayInfoID == target->DisplayInfoID)
        return false;

    if (source->Class != target->Class)
        return false;

    if (TransmogRules_CanNeverTransmog(source) || TransmogRules_CanNeverTransmog(target))
        return false;

    if (!TransmogRules_SuitableForTransmogrification(player, target) ||
        !TransmogRules_SuitableForTransmogrification(player, source))
        return false;

    if (TransmogRules_IsRangedWeapon(source->Class, source->SubClass) !=
        TransmogRules_IsRangedWeapon(target->Class, target->SubClass))
        return false;

    if (source->SubClass != target->SubClass &&
        !IsSubclassMismatchAllowed(player, source, target))
        return false;

    if (source->InventoryType != target->InventoryType &&
        !IsInvTypeMismatchAllowed(source, target))
        return false;

    return true;
}
