#include "ScriptRegistration.h"
#include "Engine/Engine.h"
#include "Logger/Logger.h"
#include "Input/KeyCode.h"
#include <angelscript.h>

#define REGISTER_CHECK(X) r = X; ASSERT(r >= 0)
#define DEFINE_API(X) void RYU_CONCAT(ScriptRegistration::Register, RYU_CONCAT(X, API))(asIScriptEngine* engine)
#define CALL_API(X) RYU_CONCAT(Register, RYU_CONCAT(X, API))(engine)


namespace  // Anonymous namespace for proxy functions to get register
{
	i32 r = 0;

	void RyuLog(Ryu::Logging::LogLevel level, const std::string& message)
	{
		RYU_LOG_CATEGORY(Script);

		switch (level)
		{
			using enum Ryu::Logging::LogLevel;

		case Trace: 
			LOG_TRACE(RYU_USE_LOG_CATEGORY(Script), "{}", message);
			return;
		
		case Debug: 
			LOG_DEBUG(RYU_USE_LOG_CATEGORY(Script), "{}", message);
			return;
		
		case Info: 
			LOG_INFO(RYU_USE_LOG_CATEGORY(Script), "{}", message);
			return;
		
		case Warn: 
			LOG_WARN(RYU_USE_LOG_CATEGORY(Script), "{}", message);
			return;
		
		default:
			LOG_INFO(RYU_USE_LOG_CATEGORY(Script), "<LogLevel {} Not Supported> {}", Ryu::EnumToString(level), message);
			return;
		}
	}
}


namespace Ryu::Engine
{	
	void ScriptRegistration::Register(asIScriptEngine* engine)
	{
		CALL_API(Logger);
		CALL_API(Input);
		CALL_API(Engine);
		CALL_API(Debug);
		CALL_API(World);
		CALL_API(GameInstance);
	}

	DEFINE_API(Logger)
	{
		REGISTER_CHECK(engine->RegisterEnum("LogLevel"));
		REGISTER_CHECK(engine->RegisterEnumValue("LogLevel", "Trace", (i32)Ryu::Logging::LogLevel::Trace));
		REGISTER_CHECK(engine->RegisterEnumValue("LogLevel", "Debug", (i32)Ryu::Logging::LogLevel::Debug));
		REGISTER_CHECK(engine->RegisterEnumValue("LogLevel", "Info", (i32)Ryu::Logging::LogLevel::Info));
		REGISTER_CHECK(engine->RegisterEnumValue("LogLevel", "Warn", (i32)Ryu::Logging::LogLevel::Warn));
	}

	DEFINE_API(Input)
	{
		REGISTER_CHECK(engine->RegisterEnum("KeyCode"));
		{
			// X-Macro to register keycodes
#define REG_KEY(VAL) REGISTER_CHECK(engine->RegisterEnumValue("KeyCode", RYU_STRING(VAL), (i32)Ryu::Input::KeyCode::VAL))
			REG_KEY(None);
			REG_KEY(LeftMouseButton);
			REG_KEY(RightMouseButton);
			REG_KEY(MiddleMouseButton);
			REG_KEY(Extra1MouseButton);
			REG_KEY(Extra2MouseButton);
			REG_KEY(Backspace);
			REG_KEY(Tab);
			REG_KEY(Clear);
			REG_KEY(Return);
			REG_KEY(Shift);
			REG_KEY(Ctrl);
			REG_KEY(Alt);
			REG_KEY(Pause);
			REG_KEY(CapsLock);
			REG_KEY(Escape);
			REG_KEY(Convert);
			REG_KEY(NonConvert);
			REG_KEY(Accept);
			REG_KEY(ModeChange);
			REG_KEY(Space);
			REG_KEY(PageUp);
			REG_KEY(PageDown);
			REG_KEY(End);
			REG_KEY(Home);
			REG_KEY(Left);
			REG_KEY(Up);
			REG_KEY(Right);
			REG_KEY(Down);
			REG_KEY(Select);
			REG_KEY(Print);
			REG_KEY(Execute);
			REG_KEY(PrintScreen);
			REG_KEY(Insert);
			REG_KEY(Delete);
			REG_KEY(Help);
			REG_KEY(Num0);
			REG_KEY(Num1);
			REG_KEY(Num2);
			REG_KEY(Num3);
			REG_KEY(Num4);
			REG_KEY(Num5);
			REG_KEY(Num6);
			REG_KEY(Num7);
			REG_KEY(Num8);
			REG_KEY(Num9);
			REG_KEY(A);
			REG_KEY(B);
			REG_KEY(C);
			REG_KEY(D);
			REG_KEY(E);
			REG_KEY(F);
			REG_KEY(G);
			REG_KEY(H);
			REG_KEY(I);
			REG_KEY(J);
			REG_KEY(K);
			REG_KEY(L);
			REG_KEY(M);
			REG_KEY(N);
			REG_KEY(O);
			REG_KEY(P);
			REG_KEY(Q);
			REG_KEY(R);
			REG_KEY(S);
			REG_KEY(T);
			REG_KEY(U);
			REG_KEY(V);
			REG_KEY(W);
			REG_KEY(X);
			REG_KEY(Y);
			REG_KEY(Z);
			REG_KEY(LeftWin);
			REG_KEY(RightWin);
			REG_KEY(Apps);
			REG_KEY(Sleep);
			REG_KEY(Numpad0);
			REG_KEY(Numpad1);
			REG_KEY(Numpad2);
			REG_KEY(Numpad3);
			REG_KEY(Numpad4);
			REG_KEY(Numpad5);
			REG_KEY(Numpad6);
			REG_KEY(Numpad7);
			REG_KEY(Numpad8);
			REG_KEY(Numpad9);
			REG_KEY(Multiply);
			REG_KEY(Add);
			REG_KEY(Separator);
			REG_KEY(Subtract);
			REG_KEY(Decimal);
			REG_KEY(Divide);
			REG_KEY(F1);
			REG_KEY(F2);
			REG_KEY(F3);
			REG_KEY(F4);
			REG_KEY(F5);
			REG_KEY(F6);
			REG_KEY(F7);
			REG_KEY(F8);
			REG_KEY(F9);
			REG_KEY(F10);
			REG_KEY(F11);
			REG_KEY(F12);
			REG_KEY(F13);
			REG_KEY(F14);
			REG_KEY(F15);
			REG_KEY(F16);
			REG_KEY(F17);
			REG_KEY(F18);
			REG_KEY(F19);
			REG_KEY(F20);
			REG_KEY(F21);
			REG_KEY(F22);
			REG_KEY(F23);
			REG_KEY(F24);
			REG_KEY(NumLock);
			REG_KEY(ScrollLock);
			REG_KEY(NumpadEqual);
			REG_KEY(LeftShift);
			REG_KEY(RightShift);
			REG_KEY(LeftControl);
			REG_KEY(RightControl);
			REG_KEY(LeftMenu);
			REG_KEY(RightMenu);
			REG_KEY(SemiColon);
			REG_KEY(Equals);
			REG_KEY(Comma);
			REG_KEY(Minus);
			REG_KEY(Period);
			REG_KEY(Slash);
			REG_KEY(Grave);
			REG_KEY(OpenSquareBracket);
			REG_KEY(BackSlash);
			REG_KEY(CloseSquareBracket);
			REG_KEY(Quote);
#undef REG_KEY
		}

		REGISTER_CHECK(engine->RegisterEnum("ModifierKeys"));
		{
#define REG_KEY(VAL) REGISTER_CHECK(engine->RegisterEnumValue("ModifierKeys", RYU_STRING(VAL), (i32)Ryu::Input::ModifierKeys::VAL))
			REG_KEY(None);
			REG_KEY(LeftShift);
			REG_KEY(RightShift);
			REG_KEY(LeftControl);
			REG_KEY(RightControl);
			REG_KEY(LeftAlt);
			REG_KEY(RightAlt);
#undef REG_KEY
		}
		

		REGISTER_CHECK(engine->RegisterEnum("MouseButton"));
		{
#define REG_KEY(VAL) REGISTER_CHECK(engine->RegisterEnumValue("MouseButton", RYU_STRING(VAL), (i32)Ryu::Input::MouseButton::VAL))
			REG_KEY(None);
			REG_KEY(LeftButton);
			REG_KEY(RightButton);
			REG_KEY(MiddleButton);
			REG_KEY(XButton1);
			REG_KEY(XButton2);
#undef REG_KEY
		}
	}

	DEFINE_API(Engine)
	{
		REGISTER_CHECK(engine->RegisterObjectType("Engine", 0, asOBJ_REF | asOBJ_NOCOUNT));
		REGISTER_CHECK(engine->RegisterGlobalFunction("Engine@ GetEngine()", asFUNCTION(::Ryu::Engine::Engine::Get), asCALL_CDECL));
		REGISTER_CHECK(engine->RegisterObjectMethod("Engine", "void Quit() const", asMETHOD(Ryu::Engine::Engine, Quit), asCALL_THISCALL));
	}

	DEFINE_API(Debug)
	{
		REGISTER_CHECK(engine->RegisterGlobalFunction("void RyuLog(LogLevel, string)", asFUNCTION(RyuLog), asCALL_CDECL));
	}

	DEFINE_API(World)
	{
		
	}

	DEFINE_API(GameInstance)
	{
		REGISTER_CHECK(engine->RegisterInterface("IGameInstance"));
		REGISTER_CHECK(engine->RegisterInterfaceMethod("IGameInstance", "void OnStart()"));
		REGISTER_CHECK(engine->RegisterInterfaceMethod("IGameInstance", "void OnEnd()"));
		REGISTER_CHECK(engine->RegisterInterfaceMethod("IGameInstance", "void OnUpdate(double)"));
	}
}

#undef CALL_API
#undef DEFINE_API
#undef REGISTER_CHECK
