module;
#include <expected>
#include <string>

export module Ryu.Common:Result;

export namespace Ryu
{
	template <typename ReturnType>
	using Result = std::expected<ReturnType, std::string>;

	using VoidResult = std::expected<void, std::string>;

	struct MakeResultError
	{
		template <typename T>
		operator Result<T>() const
		{
			return std::unexpected(Message);
		}

		operator VoidResult() const
		{
			return std::unexpected(Message);
		}

		std::string Message;
	};
}
