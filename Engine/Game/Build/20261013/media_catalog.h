#pragma once
#include "Engine/Game/Build/fingerprint.h"
#include <cstdint>
#include "media_catalog_rvas.h"

// The game's media catalogue: thumbnails and labels of saved replay drafts (and the game's own
// park saves), kept as numbered folders with a manifest.json under <CatalogVFSRoot>/<user id>
// (Extension/Settings/replay_drafts.cpp; analysis/replay-editor.md, section 6).
namespace dingosdk::game::build::v20260929::media_catalog {
// The backend services object (engine.h backend_services) when it is the game's own.
// lookup(services): the registered "catalog" container, or null.

// add_container(services): makes the container and registers it. The services constructor
// calls it among its online registrations (0x282fa0), none of which run offline.

// user_added(listener, &user): the container's answer to a user signing in (its listener, at
// container + 0x00, is subscribed to "user_manager" when there is one). It makes the catalogue
// for the user's id (an engine string at user + 0x00) and keeps it at container + 0x00: the
// constructor (0x63be70) lists the folder and loads each entry (0x659450 -> 0x657480), then a
// posted task (0x659980) sets the catalogue's ready flag.

// In a catalogue: its folder (an engine string), its entries (0x000 bytes each) and whether
// its first load is done. Updating and deleting an entry need the flag.
}
