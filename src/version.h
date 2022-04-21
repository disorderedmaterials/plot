#pragma once

#define MILDREDVERSION "0.1.0"
#define MILDREDSHORTHASH ""
#define MILDREDREPO "https://github.com/disorderedmaterials/plot.git"

#include <string_view>

namespace Mildred
{
namespace Version
{
// Return version
std::string_view version() { return MILDREDVERSION; }
// Return short hash
std::string_view shortHash() { return MILDREDSHORTHASH; }
// Return repo url
std::string_view repoUrl() { return MILDREDREPO; }
}; // namespace Version
}; // namespace Mildred
