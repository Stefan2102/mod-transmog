#include "Transmog.h"

// ==========================================
// CLASS DECLARATION
// ==========================================

class TransmogWorldScript : public WorldScript
{
public:
    TransmogWorldScript() : WorldScript("TransmogWorldScript", {
        WORLDHOOK_ON_STARTUP
    }) { }

    void OnStartup() override
    {
        sTransmog->LoadConfig();

        CharacterDatabase.Execute("DELETE FROM mod_transmog_plus WHERE NOT EXISTS (SELECT 1 FROM characters WHERE characters.guid = mod_transmog_plus.Owner)");
    }
};

// ==========================================
// LOADER
// ==========================================

void AddSC_TransmogWorldScript()
{
    new TransmogWorldScript();
}
