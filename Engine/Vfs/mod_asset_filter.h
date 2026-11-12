#pragma once

#include "mod_asset_metadata.h"

#include <filesystem>
#include <functional>
#include <set>
#include <string_view>

namespace dingosdk::mods
{

    // Load asset metadata JSON exported by mod_dump
    [[nodiscard]] ModAssetMetadata load_asset_metadata(const std::filesystem::path &metadata_file);

    // Save asset metadata to JSON (used by mod_dump)
    void save_asset_metadata(const std::filesystem::path &output_file, const ModAssetMetadata &metadata);

    // Load mod asset selections from a JSON file (e.g., .reskate/mod-selections.json)
    [[nodiscard]] MergeAssetFiltering load_asset_selections(const std::filesystem::path &selections_file);

    // Save mod asset selections to JSON
    void save_asset_selections(const std::filesystem::path &output_file, const MergeAssetFiltering &filtering);

    // Given a set of disabled assets and the metadata, compute the full set of assets that should be excluded
    // This includes the disabled assets themselves plus anything that depends on them (transitively)
    // A callback can be provided to log which assets are being excluded and why
    [[nodiscard]] std::set<AssetGuid> resolve_disabled_assets(
        const ModAssetMetadata &metadata,
        const std::set<AssetGuid> &user_disabled,
        const std::function<void(const AssetGuid &guid, std::string_view reason)> &on_excluded = {});

    // Check if an asset should be filtered out during merge
    bool is_asset_disabled(const AssetGuid &guid, const std::set<AssetGuid> &disabled_assets);

    // Filter a bundle's asset list based on disabled assets, returning which assets remain
    [[nodiscard]] std::set<std::string> filter_bundle_assets(std::string_view bundle_name,
                                                             const std::set<std::string> &all_assets,
                                                             const ModAssetMetadata &metadata,
                                                             const std::set<AssetGuid> &disabled_assets);

    // Check for circular dependencies in the metadata
    // Returns empty if no cycles, or a list of cycles found (each cycle is a vector of asset identifiers)
    [[nodiscard]] std::vector<std::vector<AssetIdentifier>> detect_dependency_cycles(const ModAssetMetadata &metadata);

} // namespace dingosdk::mods
