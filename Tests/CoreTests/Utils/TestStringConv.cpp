#include "../../../Ryu/Core/Utils/StringConv.h"

// Test to check if the ToWideStr and ToNarrowStr functions work
int main()
{
	const std::string str = "This is a test string";
	const std::wstring wstr = L"This is a test string";

	const std::wstring wide = Ryu::ToWideStr(str);
	const std::string narrow = Ryu::ToNarrowStr(wstr);

	const bool wideTest = wstr == wide;
	const bool narrowTest1 = str == narrow;	
	
	const bool testPassed = wideTest && narrowTest1;

	return testPassed == true ? 0 : 1;
}