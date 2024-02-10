#pragma once

// Include the modloader header, which allows us to tell the modloader which mod this is, and the version etc.
#include "modloader/shared/modloader.hpp"

#include "TMPro/TextMeshPro.hpp"

// beatsaber-hook is a modding framework that lets us call functions and fetch field values from in the game
// It also allows creating objects, configuration, and importantly, hooking methods to modify their values
#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/config/config-utils.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-functions.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"

// Define these functions here so that we can easily read configuration and log information from other files
Configuration& getConfig();
Logger& getLogger();

extern TMPro::TextMeshPro* mainText;
extern TMPro::TextMeshPro* bottomText;
extern std::vector<std::string> currentTextEntry;
extern std::string textPath;
extern void OverwriteFile(std::string contents);
static std::vector<std::vector<std::string>> allEntries;
static bool HasFakeGlow;
extern void setText_Simple(std::string str1, std::string str2 = "");