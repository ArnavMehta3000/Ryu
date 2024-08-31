#pragma once
#include <CoreDLLDefines.h>
#include <Core/Plugin/IPlugin.h>
#include <Core/ObjectMacros.h>

#define PLUGIN_API RYU_API

RYU_API_FUNC(::Ryu::IPlugin*, CreatePlugin);
RYU_API_FUNC(void, DestroyPlugin, ::Ryu::IPlugin*);
