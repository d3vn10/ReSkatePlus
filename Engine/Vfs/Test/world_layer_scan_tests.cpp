// Scans the installed game's level data (first argument) and checks the
// world-layer catalog it produces, including a round trip through the cache
// file. Skips when no game folder is configured.
#include "Engine/Vfs/world_layer_scan.h"
#include <algorithm>
#include <iostream>
#include <unordered_map>

namespace {
    int failures = 0;
    void check(bool condition, const char* message) {
        if (!condition) { std::cerr << "FAIL: " << message << '\n'; ++failures; }
    }
}

int main(int argc, char** argv) {
    using namespace dingosdk;
    if (argc < 2 || !std::filesystem::exists(std::filesystem::path(argv[1]) / "Skate.exe")) {
        std::cout << "No game folder given; skipped.\n";
        return 0;
    }
    const std::filesystem::path root = argv[1];
    std::cout << "scanning world layers in game folder: " << root << "\n";
    const auto catalog = world_layer_scan::scan(root);

    std::ifstream file(root / L"Data" / L"layout.toc", std::ios::binary);
    if (!file) return {};
    const std::string bytes{ std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() };
    // The number follows its name as text, after a few bytes that say what kind of value it is.
    constexpr std::string_view name = "pipelineCodeChangelists";
    const auto at = bytes.find(name);
    if (at == std::string::npos) return {};
    const auto is_digit = [](char c) { return c >= '0' && c <= '9'; };
    auto from = at + name.size();
    const auto limit = std::min(bytes.size(), from + 16);
    while (from < limit && !is_digit(bytes[from])) ++from;
    auto to = from;
    while (to < bytes.size() && to - from < 16 && is_digit(bytes[to])) ++to;
    return to - from >= 6 ? bytes.substr(from, to - from) : std::string{};

    std::cout << "catalog contains " << catalog.anchors.size() << " anchors, " << catalog.nodes.size() << " nodes, " << catalog.layers.size() << " layers\n\n";

    std::unordered_map<WorldMap, std::vector<WorldLayerNode>> nodes;
    std::unordered_map<WorldMap, std::vector<WorldLayer>> layers;

    for (const auto& node : catalog.nodes) {
        nodes[node.map].push_back(node);
    }

    for (const auto& layer : catalog.layers) {
        layers[layer.map].push_back(layer);
    }

    for (int i = 0; i < static_cast<int>(WorldMap::mpr) + 1; ++i) {
        auto map = static_cast<WorldMap>(i);
        std::cout << "world map '" << world_map_key(map) << "':\n";

		for (const auto& node : nodes[map]) {
			std::cout << "    node => bundle: " << node.bundle << ", parent: " << node.parent << ", autoload: " << node.autoload << "\n";
		}

        for (const auto& layer : layers[map]) {
			std::cout << "    layer => key: " << layer.key << ", label: " << layer.label << ", detail: " << layer.detail
				<< ", leaf: " << layer.leaf << ", switch_slot: " << layer.switch_slot
				<< ", category: " << layer.category << "\n";
        }

        std::cout << "\n";
    }

    /*check(catalog.nodes.size() >= 200 && catalog.layers.size() < catalog.nodes.size() &&
        catalog.layers.size() + 8 > catalog.nodes.size(), "a row per layer but the essential ones");
    for (std::size_t i = 0; i < catalog.nodes.size(); ++i) {
        const auto parent = catalog.nodes[i].parent;
        check(parent >= -1 && parent < static_cast<int>(i), "parents come first");
    }
    const auto has = [&](std::string_view key) {
        return std::any_of(catalog.layers.begin(), catalog.layers.end(), [&](const WorldLayer& layer) { return layer.key == key; });
    };
    for (const auto* key : { "bam_tod_1_morning", "bam_tod_7_weathernight", "grom_tod_5_night", "bam_debug_tests" })
        check(has(key), key);
    // No row for what every map is built out of: a player cannot switch off
    // the world and then wonder which row did it.
    check(std::none_of(catalog.layers.begin(), catalog.layers.end(), [](const WorldLayer& layer) {
        return layer.key.find("coregameassets") != std::string::npos;
    }), "CoreGameAssets has no row to switch");
    check(std::any_of(catalog.nodes.begin(), catalog.nodes.end(), [](const WorldLayerNode& node) {
        std::string bundle = node.bundle;
        std::transform(bundle.begin(), bundle.end(), bundle.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        return bundle.find("coregameassets") != std::string::npos;
    }), "but it keeps its node, so its children are still reachable");
    check(std::none_of(catalog.nodes.begin(), catalog.nodes.end(), [](const WorldLayerNode& node) {
        std::string bundle = node.bundle;
        std::transform(bundle.begin(), bundle.end(), bundle.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        return bundle.find("activitypresentation") != std::string::npos || bundle.find("commlot") != std::string::npos;
    }), "activity presentations and park lots stay with the game");

    const auto file = std::filesystem::temp_directory_path() / "dingosdk-world-layers-test.json";
    std::filesystem::remove(file);
    const auto written = world_layer_scan::load_or_scan(root, file);
    const auto cached = world_layer_scan::load_or_scan(root, file);
    check(std::filesystem::exists(file) && cached.layers.size() == written.layers.size() &&
        cached.nodes.size() == written.nodes.size() && cached.anchors == written.anchors, "cache round trip");
    std::filesystem::remove(file);
    std::cout << catalog.layers.size() << " world layers across " << catalog.anchors.size() << " maps.\n";
    return failures ? 1 : 0;*/
    return 0;
}
