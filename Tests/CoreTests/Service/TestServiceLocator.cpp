#include "../../../Ryu/Core/Service/ServiceLocator.h"
#include <memory>
#include <print>

enum ErrorCode
{
	Success = 0,
	Failed,
	RegisterAgainFailed,
	ResolveFailed,
	GetSumFailed,
	UnregisterFailed
};

class ITestService : public Ryu::IService
{
public:
	virtual void AddNumbers(int a, int b) = 0;
	virtual int GetSum() const = 0;
};

class TestService : public Ryu::ServiceBase<ITestService>
{
public:
	void AddNumbers(int a, int b) override final
	{
		m_sum = a + b;
	}

	int GetSum() const override final { return m_sum; }
private:
	int m_sum = 0;
};

// Test service locator
int main()
{
	std::shared_ptr<Ryu::ServiceLocator> serviceLocator = std::make_shared<Ryu::ServiceLocator>();
	
	// Fail by default
	ErrorCode errorCode = ErrorCode::Failed;

	// Register the service the first time
	serviceLocator->RegisterService(std::make_shared<TestService>());

	try
	{
		// Try registering again...should throw an exception
		serviceLocator->RegisterService(std::make_shared<TestService>());
	}
	catch (const std::runtime_error&)
	{
		errorCode = ErrorCode::Success;
	}

	// Caught the expected axception
	if (errorCode != ErrorCode::Success)
	{
		return ErrorCode::RegisterAgainFailed;
	}

	if (auto testService = serviceLocator->ResolveService<ITestService>())
	{
		testService->AddNumbers(1, 2);
		int sum = testService->GetSum();
		if (sum != 3)
		{
			return ErrorCode::GetSumFailed;
		}

		try
		{
			serviceLocator->UnregisterService<ITestService>();
		}
		catch (const std::runtime_error&)
		{
			return ErrorCode::UnregisterFailed;
		}
	}
	else
	{
		return ErrorCode::ResolveFailed;
	}


	return errorCode;
}
