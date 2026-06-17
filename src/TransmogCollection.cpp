#include "Transmog.h"

// ==========================================
// LOAD COLLECTION FOR ACCOUNT
// ==========================================

void Transmog::LoadCollectionForAccount(uint32 accountId)
{
    {
        std::shared_lock<std::shared_mutex> lock(collectionMutex);
        if (collectionCache.find(accountId) != collectionCache.end())
            return;
    }

    std::unordered_set<uint32> items;
    QueryResult result = CharacterDatabase.Query("SELECT item_template_id FROM mod_transmog_plus_appearances WHERE account_id = {}", accountId);
    if (result)
    {
        do
        {
            items.insert((*result)[0].Get<uint32>());
        } while (result->NextRow());
    }

    std::unique_lock<std::shared_mutex> lock(collectionMutex);
    if (collectionCache.find(accountId) == collectionCache.end())
        collectionCache.emplace(accountId, std::move(items));
}

// ==========================================
// UNREF COLLECTION FOR ACCOUNT
// ==========================================

void Transmog::UnrefCollectionForAccount(uint32 accountId)
{
    std::unique_lock<std::shared_mutex> lock(collectionMutex);
    auto refIt = collectionRefCounts.find(accountId);
    if (refIt == collectionRefCounts.end())
        return;

    if (--refIt->second == 0)
    {
        collectionRefCounts.erase(refIt);
        collectionCache.erase(accountId);
    }
}

// ==========================================
// ADD COLLECTED APPEARANCE
// ==========================================

bool Transmog::AddCollectedAppearance(uint32 accountId, uint32 itemId)
{
    std::unique_lock<std::shared_mutex> lock(collectionMutex);
    auto accountIt = collectionCache.find(accountId);
    if (accountIt == collectionCache.end())
    {
        collectionCache.emplace(accountId, std::unordered_set<uint32>{ itemId });
        return true;
    }

    auto result = accountIt->second.insert(itemId);
    return result.second;
}
