#pragma once

#include <array>
#include <cstdint>
#include <map>
#include <set>
#include <string>
#include <string_view>
#include <vector>

namespace dingosdk::mods
{

    // A 128-bit GUID for uniquely identifying assets
    using AssetGuid = std::array<std::byte, 16>;

    // Convert GUID to hex string for logging/display
    std::string guid_to_string(const AssetGuid &guid);
    // Parse hex string back to GUID, returns empty if invalid
    AssetGuid string_to_guid(std::string_view hex);

    // Identifies an asset by its bundle and name
    struct AssetIdentifier
    {
        std::string bundle; // e.g. "win32/levels/mp_skate_park"
        std::string name;   // lower-case asset name, may include kind prefix (e.g. "world/player/head_01")

        bool operator<(const AssetIdentifier &other) const
        {
            if (bundle != other.bundle)
                return bundle < other.bundle;
            return name < other.name;
        }
        bool operator==(const AssetIdentifier &other) const = default;
    };

    // Information about a single asset in a mod
    struct AssetInfo
    {
        AssetIdentifier id;
        AssetGuid guid;
        std::string kind;           // e.g. "MeshAsset", "TextureAsset", "SkeletonAsset"
        std::string category;       // User-facing category: "Cosmetic", "Map", "Shader", etc.
        std::string display_name;   // Human-readable name for UI
        std::string bundle_name;    // Game bundle this asset merges into
        std::uint64_t size_bytes{}; // Approximate size in archive
    };

    // A dependency relationship: asset A imports/uses asset B
    struct AssetDependency
    {
        AssetGuid depends_on_guid{}; // The file GUID of the asset being used
        std::string reason;          // Why (e.g. "imported in EBX", "referenced in shader")
    };

    // Metadata about a single mod's assets, exported during dump
    struct ModAssetMetadata
    {
        std::string mod_name;
        std::string mod_version;

        // All assets found in the mod, keyed by GUID for fast lookup
        std::map<AssetGuid, AssetInfo> assets_by_guid;
        // And by identifier for easier querying
        std::map<AssetIdentifier, AssetGuid> assets_by_id;

        // Dependencies: for each asset, what it depends on
        std::map<AssetGuid, std::vector<AssetDependency>> dependencies;
        // Reverse dependencies: for each asset, what depends on it
        std::map<AssetGuid, std::set<AssetGuid>> reverse_dependencies;

        // Find asset by GUID
        const AssetInfo *find_asset(const AssetGuid &guid) const
        {
            auto it = assets_by_guid.find(guid);
            return it != assets_by_guid.end() ? &it->second : nullptr;
        }

        // Find asset by identifier
        const AssetGuid *find_guid(const AssetIdentifier &id) const
        {
            auto it = assets_by_id.find(id);
            return it != assets_by_id.end() ? &it->second : nullptr;
        }
    };

    // Configuration specifying which assets a user wants to disable for a mod
    struct ModAssetSelection
    {
        std::string mod_name;
        // GUIDs of assets the user wants disabled (any related dependencies must be computed at merge time)
        std::set<AssetGuid> disabled_assets;

        // Optional: user overrides (for future expansion - one asset replaces another)
        struct AssetOverride
        {
            AssetGuid replaced; // Asset being overridden
            std::string with;   // Path or reference to replacement (e.g. custom texture path)
        };
        std::vector<AssetOverride> overrides;
    };

    // The merged selection state for all active mods
    struct MergeAssetFiltering
    {
        // Map of mod name -> its selections
        std::map<std::string, ModAssetSelection> selections;

        // Find disabled assets for a mod
        const ModAssetSelection *get_selection(std::string_view mod_name) const
        {
            auto it = selections.find(std::string(mod_name));
            return it != selections.end() ? &it->second : nullptr;
        }
    };

} // namespace dingosdk::mods
