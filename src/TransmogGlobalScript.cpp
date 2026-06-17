#include "Transmog.h"

// ==========================================
// CLASS DECLARATION
// ==========================================

class TransmogGlobalScript : public GlobalScript
{
public:
    TransmogGlobalScript() : GlobalScript("TransmogGlobalScript", {
        GLOBALHOOK_ON_MIRRORIMAGE_DISPLAY_ITEM
    }) { }

    void OnMirrorImageDisplayItem(Item const* item, uint32& display) override
    {
        if (!sTransmog->Enable)
            return;

        uint32 fakeEntry = sTransmog->GetSlotAppearance(item->GetOwnerGUID(), item->GetSlot());
        if (fakeEntry == 0)
            return;

        if (fakeEntry == HIDDEN_ITEM_ID)
        {
            if (TransmogRules_IsArmorSlot(item->GetSlot()))
                display = 0;
            return;
        }

        ItemTemplate const* sourceTemplate = sObjectMgr->GetItemTemplate(fakeEntry);
        if (sourceTemplate)
            display = sourceTemplate->DisplayInfoID;
    }
};

// ==========================================
// LOADER
// ==========================================

void AddSC_TransmogGlobalScript()
{
    new TransmogGlobalScript();
}
