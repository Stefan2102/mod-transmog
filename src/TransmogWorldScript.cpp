#include "Transmog.h"

class TransmogWorldScript : public WorldScript
{
public:
    TransmogWorldScript() : WorldScript("TransmogWorldScript", {
        WORLDHOOK_ON_STARTUP
    }) { }

    void OnStartup() override
    {
        sTransmog->LoadConfig();

        CharacterDatabase.Execute("DELETE FROM mod_transmog WHERE NOT EXISTS (SELECT 1 FROM characters WHERE characters.guid = mod_transmog.Owner)");
    }
};

void AddSC_TransmogWorldScript()
{
    new TransmogWorldScript();
}
