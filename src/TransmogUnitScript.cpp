#include "Transmog.h"
#include "ByteBuffer.h"
#include "UpdateFields.h"

// ==========================================
// CLASS DECLARATION
// ==========================================

class TransmogUnitScript : public UnitScript
{
public:
    TransmogUnitScript() : UnitScript("TransmogUnitScript", true, {
        UNITHOOK_SHOULD_TRACK_VALUES_UPDATE_POS_BY_INDEX,
        UNITHOOK_ON_PATCH_VALUES_UPDATE
    }) { }

    bool ShouldTrackValuesUpdatePosByIndex(Unit const* unit, uint8, uint16 index) override
    {
        return unit->IsPlayer() && index >= PLAYER_VISIBLE_ITEM_1_ENTRYID && index <= PLAYER_VISIBLE_ITEM_19_ENTRYID && (index & 1);
    }

    void OnPatchValuesUpdate(Unit const* unit, ByteBuffer& valuesUpdateBuf, BuildValuesCachePosPointers& posPointers, Player*) override
    {
        if (!unit->IsPlayer())
            return;

        Player const* player = unit->ToPlayer();
        if (!player)
            return;

        for (auto const& pair : posPointers.other)
        {
            if (!pair.second)
                continue;

            uint16 index = pair.first;
            if (index < PLAYER_VISIBLE_ITEM_1_ENTRYID || index > PLAYER_VISIBLE_ITEM_19_ENTRYID || !(index & 1))
                continue;

            uint8 slot = (index - PLAYER_VISIBLE_ITEM_1_ENTRYID) / 2;
            if (slot >= EQUIPMENT_SLOT_END)
                continue;

            Item const* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
            uint32 entry;

            if (sTransmog->Enable)
                entry = sTransmog->GetVisibleEntryForSlot(player, slot, item);
            else
                entry = item ? item->GetEntry() : 0;

            valuesUpdateBuf.put(pair.second, entry);
        }
    }
};

// ==========================================
// LOADER
// ==========================================

void AddSC_TransmogUnitScript()
{
    new TransmogUnitScript();
}
