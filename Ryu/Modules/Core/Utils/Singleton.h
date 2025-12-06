#pragma once

// Header file for Ryu.Core.Utils:Singleton macro

#define RYU_SINGLETON_DECLARE(ChildClassName) friend class ::Ryu::Utils::Singleton<ChildClassName>