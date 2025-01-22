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
QString version() { return MILDREDVERSION; }
// Return short hash
QString shortHash() { return MILDREDSHORTHASH; }
// Return repo url
QString repoUrl() { return MILDREDREPO; }
}; // namespace Version
}; // namespace Mildred
