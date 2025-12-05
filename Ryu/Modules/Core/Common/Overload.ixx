module;

export module Ryu.Common:Overload;

export namespace Ryu::Common
{
	template<typename... T>
	struct Overload : T... { using T::operator()...; };

	template<typename... T> 
	Overload(T...) -> Overload<T...>;
}
