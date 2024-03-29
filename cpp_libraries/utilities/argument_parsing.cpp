#include "argument_parsing.h"

#include "converting.h"

using namespace converting;

namespace argument_parsing
{
	std::map<std::wstring, std::wstring> argument_parser::parse(int argc, char* argv[])
	{
		std::map<std::wstring, std::wstring> result;

		size_t offset = 0;
		std::wstring argument_id;
		for (int index = 1; index < argc; ++index)
		{
			argument_id = converter::to_wstring(argv[index]);
			offset = argument_id.find(L"--", 0);
			if (offset != 0)
			{
				continue;
			}

			if (argument_id.compare(L"--help") == 0)
			{
				result.insert({ argument_id, L"" });
				continue;
			}

			if (index + 1 >= argc)
			{
				break;
			}

			auto target = result.find(argument_id);
			if (target == result.end())
			{
				result.insert({ argument_id, converter::to_wstring(argv[index + 1]) });
				++index;

				continue;
			}
			
			target->second = converter::to_wstring(argv[index + 1]);
			++index;
		}

		return result;
	}
	
	std::map<std::wstring, std::wstring> argument_parser::parse(int argc, wchar_t* argv[])
	{
		std::map<std::wstring, std::wstring> result;

		size_t offset = 0;
		std::wstring argument_id;
		for (int index = 1; index < argc; ++index)
		{
			argument_id = argv[index];
			offset = argument_id.find(L"--", 0);
			if (offset != 0)
			{
				continue;
			}

			if (argument_id.compare(L"--help") == 0)
			{
				result.insert({ argument_id, L"" });
				continue;
			}

			if (index + 1 >= argc)
			{
				break;
			}

			auto target = result.find(argument_id);
			if (target == result.end())
			{
				result.insert({ argument_id, argv[index + 1] });
				++index;

				continue;
			}

			target->second = argv[index + 1];
			++index;
		}

		return result;
	}
}