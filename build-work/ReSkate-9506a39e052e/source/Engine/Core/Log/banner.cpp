#include "logging_internal.h"
#include <atomic>

namespace dingosdk::logging {
void startup_banner() noexcept {
    detail::PreserveError preserve;
    static std::atomic_flag shown = ATOMIC_FLAG_INIT;
    if (shown.test_and_set()) return;
    write(Level::info, Channel::runtime, R"banner(
8 888888888o.   8 8888888888     d888888o.   8 8888     ,88'          .8.    8888888 8888888888 8 8888888888         8 8888      
8 8888    `88.  8 8888         .`8888:' `88. 8 8888    ,88'          .888.         8 8888       8 8888               8 8888      
8 8888     `88  8 8888         8.`8888.   Y8 8 8888   ,88'          :88888.        8 8888       8 8888               8 8888      
8 8888     ,88  8 8888         `8.`8888.     8 8888  ,88'          . `88888.       8 8888       8 8888               8 8888      
8 8888.   ,88'  8 888888888888  `8.`8888.    8 8888 ,88'          .8. `88888.      8 8888       8 888888888888 8888888 8888888888
8 888888888P'   8 8888           `8.`8888.   8 8888 88'          .8`8. `88888.     8 8888       8 8888         8888888 8888888888
8 8888`8b       8 8888            `8.`8888.  8 888888<          .8' `8. `88888.    8 8888       8 8888               8 8888      
8 8888 `8b.     8 8888        8b   `8.`8888. 8 8888 `Y8.       .8'   `8. `88888.   8 8888       8 8888               8 8888      
8 8888   `8b.   8 8888        `8b.  ;8.`8888 8 8888   `Y8.    .888888888. `88888.  8 8888       8 8888               8 8888      
8 8888     `88. 8 888888888888 `Y8888P ,88P' 8 8888     `Y8. .8'       `8. `88888. 8 8888       8 888888888888       8 8888      
)banner");
    log(Level::info, Channel::runtime, "Skate SDK | development build | Windows x64 | MSVC {} | {} {}",
        _MSC_VER, __DATE__, __TIME__);
    try {
        const auto config = status();
        write(Level::info, Channel::runtime, L"Log: " + (config.directory / L"ReSkate.log").wstring());
        log(Level::info, Channel::runtime, "Log level: {} | Windows console: {}",
            name(config.level), config.external_console ? "on" : "off");
        write(Level::info, Channel::runtime,
            "Sources: C=client, S=server, U=UI, E=engine, F=filesystem, M=materials/rendering, A=audio; SDK=tools/runtime.");
    } catch (...) {}
}
}
