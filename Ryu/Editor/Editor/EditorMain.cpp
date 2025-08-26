#include "Common/Assert.h"
#include "Engine/Setup/EngineMain.h"
#include "Engine/Setup/Setup.h"
#include "Editor/Application/EditorApp.h"
#include "Threading/JobSystem.h"
#include <print>
#include <numeric>

using namespace Ryu;
using namespace Ryu::Editor;
using namespace Ryu::MT;

using namespace std::chrono_literals;

void SimpleJobTest()
{
	JobSystem jobs(4);  // 4 worker threads

	std::atomic<int> counter{ 0 };
	std::vector<std::shared_ptr<JobHandle>> handles;

	for (int i = 0; i < 10; ++i)
	{
		std::shared_ptr<JobHandle> handle = jobs.Submit([&counter, i]
		{
			std::this_thread::sleep_for(100ms); // Simulate work
			counter++;
			std::println("Job {} completed on thread {}", i, std::this_thread::get_id());
		});
		handles.push_back(handle);
	}

	jobs.WaitForAll(handles);
	std::println("All jobs done, counter: {}", counter.load());
}

void TestJobDependencies()
{
	JobSystem jobs(4);

	std::vector<int> results(4, 0);

	// Job A: Initialize data
	auto job_a = jobs.Submit([&results]
	{
		std::this_thread::sleep_for(200ms);
		results[0] = 10;
		std::println("Job A: Initialized with {}", results[0]);
	});

	// Job B: Depends on A
	auto job_b = jobs.Submit([&results]
	{
		std::this_thread::sleep_for(150ms);
		results[1] = results[0] * 2;
		std::println("Job B: Calculated {}", results[1]);
	}, { job_a });

	// Job C: Depends on A
	auto job_c = jobs.Submit([&results]
	{
		std::this_thread::sleep_for(100ms);
		results[2] = results[0] + 5;
		std::println("Job C: Calculated {}", results[2]);
	}, { job_a });

	// Job D: Depends on B and C
	auto job_d = jobs.Submit([&results]
	{
		results[3] = results[1] + results[2];
		std::println("Job D: Calculated {}", results[3]);
	}, { job_b, job_c });

	job_d->Wait();
}

void TestParallelProcessing()
{
	JobSystem jobs;

	std::vector<int> data(1000);
	std::iota(data.begin(), data.end(), 1);

	auto start = std::chrono::high_resolution_clock::now();

	// Process data in parallel - square each number
	jobs.ForEach(data.begin(), data.end(), 
		[](int& value) 
		{
			value = value * value;
			// Simulate some work
			std::this_thread::sleep_for(1ms);
		});

	auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

	std::println("Processed {} items in {}ms", data.size(), duration.count());
	std::println("First 10 items:");
	for (int i = 0; i < 10; ++i)
	{
		std::println("Item {}: {}", i, data[i]);
	}
	std::println("Finished processing");
}

RYU_MAIN()
{
	RYU_DBG_TRACK_MEM();

	/*SimpleJobTest();
	TestJobDependencies();
	TestParallelProcessing();
	return 0;*/

	try
	{
		if (!Engine::Setup())
		{
			Engine::Shutdown();
			return 1;
		}

		// Create editor window
		Window::Window::Config windowConfig{ .Title = "Ryu Editor", .Type = Window::WindowType::Borderless };
		auto editorWindow = std::make_shared<Window::Window>(windowConfig);
		App::App::InitWindow(*editorWindow);

		// Create editor application
		auto editorApp = std::make_shared<Editor::EditorApp>(editorWindow);
		Engine::Engine::Get().RunApp(editorApp);

		Engine::Shutdown();
	}
	catch (const AssertException& e)
	{
		RYU_LOG_FATAL("{}", e.what());

		Engine::Shutdown();
	}

	return 0;
}
