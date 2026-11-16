#pragma once
#include "engine.h"
#include "Engine/Game/Build/fingerprint.h"
#include <cstdint>
#include "profile_rvas.h"

namespace dingosdk::game::build::v20261013::profile {
// Named-asset lookup used by the native TextureAsset renderer (144020f90).




// Exact installed retail image.





















































// Native NEWS (MOTD) decoded values and allocator operations. The native
// MotdMessage -> MotdData conversion that fills a post is at 1407b42d0.



























// Native challenge catalog and local begin/end response boundary.











// Native object creation, observation and restore contracts.























// Typed native user-value service: gameplay settings bypass the expression wrappers.












// Applied gameplay-cache getters/setters (independent of the backend service).













// Play-event history manager (load/ready/send targets above).
// uint32 registered system offset of the onboarding manager in a level context,
// read by the native context accessor 145955cc0.


}
