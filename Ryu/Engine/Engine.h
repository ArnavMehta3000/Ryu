#pragma once
#include <Core/Includes.h>
#include <Engine/Core/Application.h>
#include <memory>
#include <utility>

namespace Ryu
{	
	class RYU_API Engine
	{
	public:
		~Engine();

		static Engine& Get()
		{
			static Engine instance;
			return instance;
		}

		template <typename T, typename... Args>
		void CreateApplication(Args&&... args)
		{
			if (m_application)
			{
				throw std::runtime_error("Application already exists");
			}

			m_application = std::make_unique<T>(std::forward<Args>(args)...);
		}

		void Run();

	private:
		Engine();

	private:
		std::unique_ptr<Application> m_application;
	};

	RYU_API_FUNC(Engine&, GetEngineInstance);
}