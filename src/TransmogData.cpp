#include "Transmog.h"

void Transmog::LoadPlayerSlots(ObjectGuid guid)
{
    std::array<uint32, EQUIPMENT_SLOT_END> localSlots;
    localSlots.fill(0);

    QueryResult result = CharacterDatabase.Query("SELECT Slot, FakeEntry FROM mod_transmog WHERE Owner = {}", guid.GetCounter());
    if (result)
    {
        do
        {
            uint8 slot = (*result)[0].Get<uint8>();
            uint32 fakeEntry = (*result)[1].Get<uint32>();
            if (slot >= EQUIPMENT_SLOT_END)
                continue;
            if (fakeEntry == HIDDEN_ITEM_ID && !TransmogRules_IsArmorSlot(slot))
                continue;
            if (fakeEntry == HIDDEN_ITEM_ID || sObjectMgr->GetItemTemplate(fakeEntry))
                localSlots[slot] = fakeEntry;
        } while (result->NextRow());
    }

    std::unique_lock<std::shared_mutex> lock(slotMapMutex);
    slotMap[guid] = localSlots;
}

void Transmog::UnloadPlayerSlots(ObjectGuid guid)
{
    std::unique_lock<std::shared_mutex> lock(slotMapMutex);
    slotMap.erase(guid);
}

void Transmog::SetSlotAppearance(Player* player, uint8 slot, uint32 fakeEntry)
{
    if (slot >= EQUIPMENT_SLOT_END)
        return;

    ObjectGuid guid = player->GetGUID();

    {
        std::unique_lock<std::shared_mutex> lock(slotMapMutex);
        slotMap[guid][slot] = fakeEntry;
    }

    if (fakeEntry == 0)
        CharacterDatabase.Execute("DELETE FROM mod_transmog WHERE Owner = {} AND Slot = {}", guid.GetCounter(), slot);
    else
        CharacterDatabase.Execute("REPLACE INTO mod_transmog (Owner, Slot, FakeEntry) VALUES ({}, {}, {})", guid.GetCounter(), slot, fakeEntry);
}

uint32 Transmog::GetSlotAppearance(ObjectGuid guid, uint8 slot) const
{
    if (slot >= EQUIPMENT_SLOT_END)
        return 0;

    std::shared_lock<std::shared_mutex> lock(slotMapMutex);
    auto it = slotMap.find(guid);
    if (it == slotMap.end())
        return 0;
    return it->second[slot];
}

void Transmog::ApplySlot(Player* player, uint8 slot, Item* item)
{
    if (slot >= EQUIPMENT_SLOT_END)
        return;

    uint16 index = GetVisibleItemIndex(slot);
    uint32 entry = GetVisibleEntryForSlot(player, slot, item);
    uint32 fakeEntry = item ? GetSlotAppearance(player->GetGUID(), slot) : 0;

    player->SetUInt32Value(index, entry);

    if (!item)
    {
        player->SetUInt32Value(index + 1, 0);
        return;
    }

    if (fakeEntry == HIDDEN_ITEM_ID && TransmogRules_IsArmorSlot(slot))
    {
        player->SetUInt16Value(index + 1, 0, 0);
        player->SetUInt16Value(index + 1, 1, 0);
    }
    else
    {
        player->SetUInt16Value(index + 1, 0, item->GetEnchantmentId(PERM_ENCHANTMENT_SLOT));
        player->SetUInt16Value(index + 1, 1, item->GetEnchantmentId(TEMP_ENCHANTMENT_SLOT));
    }
}

void Transmog::RefreshSlot(Player* player, uint8 slot)
{
    if (slot >= EQUIPMENT_SLOT_END)
        return;

    Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
    player->SetVisibleItemSlot(slot, item);
}

void Transmog::RefreshAllSlots(Player* player)
{
    for (uint8 slot = EQUIPMENT_SLOT_START; slot < EQUIPMENT_SLOT_END; ++slot)
        RefreshSlot(player, slot);
}

void Transmog::ClearAllSlots(Player* player)
{
    ObjectGuid guid = player->GetGUID();
    {
        std::unique_lock<std::shared_mutex> lock(slotMapMutex);
        auto it = slotMap.find(guid);
        if (it != slotMap.end())
            it->second.fill(0);
    }
    CharacterDatabase.Execute("DELETE FROM mod_transmog WHERE Owner = {}", guid.GetCounter());
}

uint8 Transmog::GetSelectedSlot(ObjectGuid guid) const
{
    std::shared_lock<std::shared_mutex> lock(slotMapMutex);
    auto it = selectionCache.find(guid);
    if (it == selectionCache.end())
        return EQUIPMENT_SLOT_END;
    return it->second;
}

void Transmog::SetSelectedSlot(ObjectGuid guid, uint8 slot)
{
    std::unique_lock<std::shared_mutex> lock(slotMapMutex);
    selectionCache[guid] = slot;
}

void Transmog::ClearSelection(ObjectGuid guid)
{
    std::unique_lock<std::shared_mutex> lock(slotMapMutex);
    selectionCache.erase(guid);
}
