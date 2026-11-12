#include "mod_asset_filter.h"

#include "Engine/Core/Json/json.h"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <queue>
#include <iomanip>
#include <functional>
#include <map>
#include <optional>
#include <set>

// Note: This uses basic JSON parsing. For production, replace with a proper JSON library
// like nlohmann/json. These functions handle the mod-selections.json and asset metadata files.

namespace
{
    // Simple JSON string helper - escape special characters
    std::string escape_json_string(std::string_view input)
    {
        std::string result;
        for (const auto ch : input)
        {
            switch (ch)
            {
            case '"':
                result += "\\\"";
                break;
            case '\\':
                result += "\\\\";
                break;
            case '\b':
                result += "\\b";
                break;
            case '\f':
                result += "\\f";
                break;
            case '\n':
                result += "\\n";
                break;
            case '\r':
                result += "\\r";
                break;
            case '\t':
                result += "\\t";
                break;
            default:
                if (static_cast<unsigned char>(ch) < 0x20)
                {
                    result += "\\u";
                    result += std::to_string(static_cast<unsigned>(ch));
                }
                else
                {
                    result += ch;
                }
            }
        }
        return result;
    }

    // Simple JSON value extractor
    std::optional<std::string> extract_json_string(const std::string &json, const std::string &key)
    {
        const auto key_pattern = "\"" + key + "\"";
        const auto key_pos = json.find(key_pattern);
        if (key_pos == std::string::npos)
            return std::nullopt;

        const auto colon_pos = json.find(':', key_pos);
        if (colon_pos == std::string::npos)
            return std::nullopt;

        auto quote_pos = json.find('"', colon_pos);
        if (quote_pos == std::string::npos)
            return std::nullopt;

        const auto end_quote = json.find('"', quote_pos + 1);
        if (end_quote == std::string::npos)
            return std::nullopt;

        return json.substr(quote_pos + 1, end_quote - quote_pos - 1);
    }
} // namespace

namespace dingosdk::mods
{

    std::string guid_to_string(const AssetGuid &guid)
    {
        std::ostringstream oss;
        for (std::size_t i = 0; i < guid.size(); ++i)
        {
            const auto byte = static_cast<unsigned char>(guid[i]);
            oss << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(byte);
        }
        return oss.str();
    }

    AssetGuid string_to_guid(std::string_view hex)
    {
        AssetGuid guid{};
        if (hex.size() != 32)
            return guid; // Invalid

        for (std::size_t i = 0; i < 16; ++i)
        {
            const auto byte_str = hex.substr(i * 2, 2);
            try
            {
                int value = std::stoi(std::string(byte_str), nullptr, 16);
                guid[i] = static_cast<std::byte>(value & 0xFF);
            }
            catch (...)
            {
                return AssetGuid{}; // Invalid hex
            }
        }
        return guid;
    }

    ModAssetMetadata load_asset_metadata(const std::filesystem::path &metadata_file)
    {
        ModAssetMetadata metadata;

        if (!std::filesystem::exists(metadata_file))
        {
            return metadata; // Empty
        }

        std::ifstream in(metadata_file, std::ios::binary);
        if (!in)
        {
            throw std::runtime_error("Cannot read asset metadata file");
        }
        const std::string text((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
        const auto root = dingosdk::Json::parse(text);

        metadata.mod_name = root.value("mod_name", std::string{});
        metadata.mod_version = root.value("mod_version", std::string{});

        const auto &assets = root.at("assets");
        for (std::size_t i = 0; i < assets.size(); ++i)
        {
            const auto &entry = assets.at(i);
            AssetInfo info;
            info.guid = string_to_guid(entry.at("guid").string());
            info.id.bundle = entry.at("bundle").string();
            info.id.name = entry.at("name").string();
            info.kind = entry.value("kind", std::string{});
            info.category = entry.value("category", std::string{});
            info.display_name = entry.value("display_name", std::string{});
            info.bundle_name = entry.value("bundle_name", std::string{});
            info.size_bytes = entry.value("size_bytes", std::uint64_t{0});
            metadata.assets_by_id[info.id] = info.guid;
            metadata.assets_by_guid[info.guid] = std::move(info);
        }

        const auto &dependencies = root.at("dependencies");
        for (std::size_t i = 0; i < dependencies.size(); ++i)
        {
            const auto &entry = dependencies.at(i);
            const auto guid = string_to_guid(entry.at("guid").string());
            AssetDependency dependency;
            dependency.depends_on_guid = string_to_guid(entry.at("depends_on").string());
            dependency.reason = entry.value("reason", std::string{});
            metadata.reverse_dependencies[dependency.depends_on_guid].insert(guid);
            metadata.dependencies[guid].push_back(std::move(dependency));
        }

        return metadata;
    }

    void save_asset_metadata(const std::filesystem::path &output_file, const ModAssetMetadata &metadata)
    {
        std::ofstream out(output_file, std::ios::binary | std::ios::trunc);
        if (!out)
        {
            throw std::runtime_error("Cannot write asset metadata file");
        }

        out << "{\n";
        out << "  \"mod_name\": \"" << escape_json_string(metadata.mod_name) << "\",\n";
        out << "  \"mod_version\": \"" << escape_json_string(metadata.mod_version) << "\",\n";

        out << "  \"assets\": [";
        bool first = true;
        for (const auto &[guid, asset_info] : metadata.assets_by_guid)
        {
            out << (first ? "\n" : ",\n");
            out << "    {\"guid\": \"" << guid_to_string(guid) << "\", \"bundle\": \""
                << escape_json_string(asset_info.id.bundle) << "\", \"name\": \""
                << escape_json_string(asset_info.id.name) << "\", \"kind\": \"" << escape_json_string(asset_info.kind)
                << "\", \"category\": \"" << escape_json_string(asset_info.category) << "\", \"display_name\": \""
                << escape_json_string(asset_info.display_name) << "\", \"bundle_name\": \""
                << escape_json_string(asset_info.bundle_name) << "\", \"size_bytes\": " << asset_info.size_bytes << "}";
            first = false;
        }
        out << (first ? "],\n" : "\n  ],\n");

        out << "  \"dependencies\": [";
        first = true;
        for (const auto &[guid, deps] : metadata.dependencies)
        {
            for (const auto &dependency : deps)
            {
                out << (first ? "\n" : ",\n");
                out << "    {\"guid\": \"" << guid_to_string(guid) << "\", \"depends_on\": \""
                    << guid_to_string(dependency.depends_on_guid) << "\", \"reason\": \""
                    << escape_json_string(dependency.reason) << "\"}";
                first = false;
            }
        }
        out << (first ? "]\n" : "\n  ]\n");
        out << "}\n";
    }

    MergeAssetFiltering load_asset_selections(const std::filesystem::path &selections_file)
    {
        MergeAssetFiltering filtering;

        if (!std::filesystem::exists(selections_file))
        {
            return filtering; // Empty
        }

        std::ifstream in(selections_file, std::ios::binary);
        if (!in)
        {
            throw std::runtime_error("Cannot read asset selections file");
        }
        const std::string text((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
        const auto root = dingosdk::Json::parse(text);

        if (!root.contains("selections"))
            return filtering;
        for (const auto &[mod_name, mod_selection] : root.at("selections").items())
        {
            ModAssetSelection selection;
            selection.mod_name = mod_name;
            const auto &disabled = mod_selection.at("disabled_assets");
            for (std::size_t i = 0; i < disabled.size(); ++i)
            {
                selection.disabled_assets.insert(string_to_guid(disabled.at(i).string()));
            }
            filtering.selections[mod_name] = std::move(selection);
        }

        return filtering;
    }

    void save_asset_selections(const std::filesystem::path &output_file, const MergeAssetFiltering &filtering)
    {
        std::ofstream out(output_file, std::ios::binary | std::ios::trunc);
        if (!out)
        {
            throw std::runtime_error("Cannot write asset selections file");
        }

        dingosdk::Json root = dingosdk::Json::object();
        dingosdk::Json selections = dingosdk::Json::object();
        for (const auto &[mod_name, selection] : filtering.selections)
        {
            dingosdk::Json disabled = dingosdk::Json::array();
            for (const auto &guid : selection.disabled_assets)
            {
                disabled.push_back(guid_to_string(guid));
            }
            dingosdk::Json mod_entry = dingosdk::Json::object();
            mod_entry["disabled_assets"] = std::move(disabled);
            selections[mod_name] = std::move(mod_entry);
        }
        root["selections"] = std::move(selections);

        out << root.dump(2) << "\n";
    }

    std::set<AssetGuid> resolve_disabled_assets(
        const ModAssetMetadata &metadata,
        const std::set<AssetGuid> &user_disabled,
        const std::function<void(const AssetGuid &guid, std::string_view reason)> &on_excluded)
    {

        std::set<AssetGuid> result = user_disabled;
        std::queue<AssetGuid> to_process;

        // Start with user-disabled assets
        for (const auto &guid : user_disabled)
        {
            to_process.push(guid);
        }

        // BFS to find all dependent assets
        std::set<AssetGuid> visited;
        while (!to_process.empty())
        {
            const auto current_guid = to_process.front();
            to_process.pop();

            if (visited.count(current_guid))
                continue;
            visited.insert(current_guid);

            // Find all assets that depend on the current asset
            const auto it = metadata.reverse_dependencies.find(current_guid);
            if (it != metadata.reverse_dependencies.end())
            {
                for (const auto &dependent_guid : it->second)
                {
                    if (!result.count(dependent_guid))
                    {
                        result.insert(dependent_guid);
                        to_process.push(dependent_guid);

                        if (on_excluded)
                        {
                            on_excluded(dependent_guid, "depends on disabled asset");
                        }
                    }
                }
            }
        }

        return result;
    }

    bool is_asset_disabled(const AssetGuid &guid, const std::set<AssetGuid> &disabled_assets)
    {
        return disabled_assets.count(guid) > 0;
    }

    std::set<std::string> filter_bundle_assets(std::string_view bundle_name,
                                               const std::set<std::string> &all_assets,
                                               const ModAssetMetadata &metadata,
                                               const std::set<AssetGuid> &disabled_assets)
    {

        std::set<std::string> result;

        for (const auto &asset_name : all_assets)
        {
            // Build the identifier to look up in metadata
            AssetIdentifier id{.bundle = std::string(bundle_name), .name = asset_name};

            // Look up the GUID for this asset
            const auto *guid_ptr = metadata.find_guid(id);
            if (!guid_ptr)
            {
                // Asset not in metadata - keep it (conservative approach)
                result.insert(asset_name);
                continue;
            }

            // Check if disabled
            if (!is_asset_disabled(*guid_ptr, disabled_assets))
            {
                result.insert(asset_name);
            }
        }

        return result;
    }

    std::vector<std::vector<AssetIdentifier>> detect_dependency_cycles(const ModAssetMetadata &metadata)
    {

        std::vector<std::vector<AssetIdentifier>> cycles;
        std::map<AssetGuid, int> state; // 0 = unvisited, 1 = visiting, 2 = visited

        std::function<void(const AssetGuid &, std::vector<AssetGuid> &)> dfs =
            [&](const AssetGuid &guid, std::vector<AssetGuid> &path)
        {
            state[guid] = 1; // Mark as visiting
            path.push_back(guid);

            // Check all dependencies of this asset
            const auto it = metadata.dependencies.find(guid);
            if (it != metadata.dependencies.end())
            {
                for (const auto &dep : it->second)
                {
                    const auto &dep_guid = dep.depends_on_guid;
                    const auto dep_state = state[dep_guid];
                    if (dep_state == 1)
                    {
                        // Found a cycle - trace it
                        std::vector<AssetIdentifier> cycle;
                        bool in_cycle = false;
                        for (const auto &guidi : path)
                        {
                            if (guidi == dep_guid)
                                in_cycle = true;
                            if (in_cycle)
                            {
                                const auto *info = metadata.find_asset(guidi);
                                if (info)
                                    cycle.push_back(info->id);
                            }
                        }
                        // Add the dependency to complete the cycle
                        const auto *info = metadata.find_asset(dep_guid);
                        if (info)
                            cycle.push_back(info->id);
                        cycles.push_back(cycle);
                    }
                    else if (dep_state == 0)
                    {
                        dfs(dep_guid, path);
                    }
                }
            }

            path.pop_back();
            state[guid] = 2; // Mark as visited
        };

        // Run DFS from all unvisited assets
        for (const auto &[guid, _] : metadata.assets_by_guid)
        {
            if (state[guid] == 0)
            {
                std::vector<AssetGuid> path;
                dfs(guid, path);
            }
        }

        return cycles;
    }

} // namespace dingosdk::mods
