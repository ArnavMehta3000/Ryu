#include "ScriptEngine.h"
#include "Common/Common.h"
#include "Logger/Logger.h"
#include <angelscript.h>
#include <External/AngelScript/AddOns/scriptany/scriptany.h>
#include <External/AngelScript/AddOns/scriptstdstring/scriptstdstring.h>
#include <External/AngelScript/AddOns/scriptarray/scriptarray.h>
#include <External/AngelScript/AddOns/scriptdictionary/scriptdictionary.h>
#include <External/AngelScript/AddOns/scriptfile/scriptfile.h>
#include <External/AngelScript/AddOns/scriptfile/scriptfilesystem.h>
#include <External/AngelScript/AddOns/scripthelper/scripthelper.h>
#include <External/AngelScript/AddOns/scriptmath/scriptmath.h>
#include <External/AngelScript/AddOns/scriptmath/scriptmathcomplex.h>
#include <libassert/assert.hpp>

namespace Ryu::Scripting
{
	void ScriptEngine::MessageCallback(const asSMessageInfo* info, MAYBE_UNUSED void* data)
	{
		switch (info->type)
		{
		case asMSGTYPE_ERROR:
			LOG_ERROR(RYU_USE_LOG_CATEGORY(ScriptEngine), "{} ({}, {}) : {}",
				info->section, info->row, info->col, info->message);
			break;

		case asMSGTYPE_WARNING:
			LOG_WARN(RYU_USE_LOG_CATEGORY(ScriptEngine), "{} ({}, {}) : {}",
				info->section, info->row, info->col, info->message);
			break;

		default:
			LOG_INFO(RYU_USE_LOG_CATEGORY(ScriptEngine), "{} ({}, {}) : {}",
				info->section, info->row, info->col, info->message);
			break;
		}
	}

	ScriptEngine::ScriptEngine()
		: m_engine(nullptr)
	{
	}

	ScriptEngine::~ScriptEngine()
	{
		if (m_engine)
		{
			m_engine->ShutDownAndRelease();
		}
	}

	bool ScriptEngine::Init()
	{
		m_engine = asCreateScriptEngine();
		ASSERT(m_engine, "Failed to create script engine");

		ConfigureEngine();

		LOG_INFO(RYU_USE_LOG_CATEGORY(ScriptEngine), "Script engine initialized");
		return true;
	}

	void ScriptEngine::Shutdown()
	{
	}

	void ScriptEngine::ConfigureEngine()
	{
		i32 result = m_engine->SetMessageCallback(asFUNCTION(ScriptEngine::MessageCallback), 0, asCALL_CDECL);
		ASSERT(result >= 0, "Failed to set message callback");

		RegisterStdString(m_engine);
		RegisterScriptAny(m_engine);
		RegisterScriptArray(m_engine, false);
		RegisterStdStringUtils(m_engine);
		RegisterScriptDictionary(m_engine);
		RegisterScriptDateTime(m_engine);
		RegisterScriptFile(m_engine);
		RegisterScriptFileSystem(m_engine);
		RegisterExceptionRoutines(m_engine);
		RegisterScriptMath(m_engine);
		RegisterScriptMathComplex(m_engine);
	}
}
