#include "ScriptEngine.h"
#include "Common/Common.h"
#include "Logger/Logger.h"
#include <External/AngelScript/scriptany/scriptany.h>
#include <External/AngelScript/scriptstdstring/scriptstdstring.h>
#include <External/AngelScript/scriptarray/scriptarray.h>
#include <External/AngelScript/scriptdictionary/scriptdictionary.h>
#include <External/AngelScript/scriptfile/scriptfile.h>
#include <External/AngelScript/scriptfile/scriptfilesystem.h>
#include <External/AngelScript/scripthelper/scripthelper.h>
#include <External/AngelScript/scriptmath/scriptmath.h>
#include <External/AngelScript/scriptmath/scriptmathcomplex.h>
#include <libassert/assert.hpp>

namespace Ryu::Scripting
{
	void ScriptEngine::MessageCallback(const AS::asSMessageInfo* info, MAYBE_UNUSED void* data)
	{
		switch (info->type)
		{
		case AS::asMSGTYPE_ERROR:
			LOG_ERROR(RYU_USE_LOG_CATEGORY(ScriptEngine), "{} ({}, {}) : {}", 
				info->section, info->row, info->col, info->message);
			break;

		case AS::asMSGTYPE_WARNING:
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
		m_engine = AS::asCreateScriptEngine(); 
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
		i32 result = m_engine->SetMessageCallback(asFUNCTION(ScriptEngine::MessageCallback), 0, AS::asCALL_CDECL);
		ASSERT(result >= 0, "Failed to set message callback");

		AS::RegisterStdString(m_engine);
		AS::RegisterScriptAny(m_engine);
		AS::RegisterScriptArray(m_engine, false);
		AS::RegisterStdStringUtils(m_engine);
		AS::RegisterScriptDictionary(m_engine);
		AS::RegisterScriptFile(m_engine);
		AS::RegisterScriptFileSystem(m_engine);
		AS::RegisterScriptDateTime(m_engine);
		AS::RegisterExceptionRoutines(m_engine);
		AS::RegisterScriptMath(m_engine);
		AS::RegisterScriptMathComplex(m_engine);
	}
}
