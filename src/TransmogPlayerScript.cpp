#include "Transmog.h"
#include "Chat.h"

class TransmogPlayerScript : public PlayerScript
{
public:
    TransmogPlayerScript() : PlayerScript("TransmogPlayerScript", {
        PLAYERHOOK_ON_LOGIN,
        PLAYERHOOK_ON_LOGOUT,
        PLAYERHOOK_ON_DELETE,
        PLAYERHOOK_ON_EQUIP,
        PLAYERHOOK_ON_UNEQUIP_ITEM,
        PLAYERHOOK_ON_LEARN_SPELL,
        PLAYERHOOK_ON_AFTER_SET_VISIBLE_ITEM_SLOT,
        PLAYERHOOK_ON_AFTER_MOVE_ITEM_FROM_INVENTORY
    }) { }

    void OnPlayerLogin(Player* player) override
    {
        uint32 accountId = player->GetSession()->GetAccountId();
        sTransmog->LoadCollectionForAccount(accountId);

        {
            std::unique_lock<std::shared_mutex> lock(sTransmog->collectionMutex);
            ++sTransmog->collectionRefCounts[accountId];
        }

        sTransmog->LoadPlayerSlots(player->GetGUID());
        sTransmog->RefreshAllSlots(player);
    }

    void OnPlayerLogout(Player* player) override
    {
        sTransmog->UnloadPlayerSlots(player->GetGUID());
        sTransmog->ClearSelection(player->GetGUID());
        sTransmog->UnrefCollectionForAccount(player->GetSession()->GetAccountId());
    }

    void OnPlayerDelete(ObjectGuid guid, uint32) override
    {
        CharacterDatabase.Execute("DELETE FROM mod_transmog WHERE Owner = {}", guid.GetCounter());
    }

    void OnPlayerEquip(Player* player, Item* item, uint8, uint8, bool) override
    {
        if (!item)
            return;

        ItemTemplate const* itemTemplate = item->GetTemplate();
        if (itemTemplate->Class != ITEM_CLASS_ARMOR && itemTemplate->Class != ITEM_CLASS_WEAPON)
            return;

        uint32 accountId = player->GetSession()->GetAccountId();
        uint32 itemId = itemTemplate->ItemId;

        sTransmog->LoadCollectionForAccount(accountId);

        if (sTransmog->AddCollectedAppearance(accountId, itemId))
        {
            CharacterDatabase.Execute("INSERT INTO mod_transmog_appearances (account_id, item_template_id) VALUES ({}, {})", accountId, itemId);
            ChatHandler(player->GetSession()).PSendSysMessage("{} {}", Transmog::GetItemLink(itemId, player->GetSession()), Tstr(player->GetSession(), LANG_TRANSMOG_APPEARANCE_ADDED));
        }
    }

    void OnPlayerUnequip(Player* player, Item*) override
    {
        if (!sTransmog->Enable)
            return;

        ObjectGuid guid = player->GetGUID();
        for (uint8 slot = EQUIPMENT_SLOT_START; slot < EQUIPMENT_SLOT_END; ++slot)
        {
            if (!player->GetItemByPos(INVENTORY_SLOT_BAG_0, slot) && sTransmog->GetSlotAppearance(guid, slot) != 0)
                sTransmog->RefreshSlot(player, slot);
        }
    }

    void OnPlayerLearnSpell(Player* player, uint32 spellId) override
    {
        if (!sTransmog->Enable)
            return;

        if (TransmogRules_IsArmorProficiencySpell(spellId))
            sTransmog->RefreshAllSlots(player);
    }

    void OnPlayerAfterSetVisibleItemSlot(Player* player, uint8 slot, Item* item) override
    {
        if (!sTransmog->Enable)
            return;

        sTransmog->ApplySlot(player, slot, item);
    }

    void OnPlayerAfterMoveItemFromInventory(Player* player, Item*, uint8 bag, uint8 slot, bool) override
    {
        if (!sTransmog->Enable)
            return;

        if (bag == INVENTORY_SLOT_BAG_0 && slot < EQUIPMENT_SLOT_END)
            sTransmog->RefreshSlot(player, slot);
    }
};

void AddSC_TransmogPlayerScript()
{
    new TransmogPlayerScript();
}
