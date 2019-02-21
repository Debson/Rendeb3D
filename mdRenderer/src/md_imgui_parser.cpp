#include "md_imgui_parser.h"

#include <fstream>
#include <sstream>
#include <stdarg.h>

#include "debug.h"

namespace ImGui
{
	namespace Config
	{
		std::fstream mdFile;
		std::string mdFileName = "imgui.ini";
		std::string mdCurrentWinName("");
		std::stringstream mdFileLoadedInMemory;
		std::stringstream mdOriginalFileContent;

		void SetCursorAtBlankLine();
		int ValueExists(std::string const &name);
		void ReplaceValuesFloat(int argCount, int posOfKeyInBuffer, std::string const & name, ...);
	}

	void Config::SetCursorAtBlankLine()
	{
		mdFileLoadedInMemory.clear();
		mdFileLoadedInMemory.seekg(0, std::ios::beg);

		std::string buffer;
		while (std::getline(mdFileLoadedInMemory, buffer))
		{
			if (buffer.find(mdCurrentWinName) != std::string::npos)
			{
				while (std::getline(mdFileLoadedInMemory, buffer) && !buffer.empty()) {}
				
			}
		}

		
	}

	int Config::ValueExists(std::string const &name)
	{
		return mdFileLoadedInMemory.str().find(name);
	}

	void Config::ReplaceValuesFloat(int argCount, int posOfKeyInBuffer, std::string const & name, ...)
	{
		std::string newKey = name + "=";
		va_list argList;
		va_start(argList, argCount);
		for (int i = 0; i < argCount; i++)
		{
			newKey += std::to_string(va_arg(argList, float)) + ',';
		}
		va_end(argList);
		newKey += '\n';

		std::string first, second;
		first = mdFileLoadedInMemory.str().substr(0, posOfKeyInBuffer);

		second = mdFileLoadedInMemory.str().substr(posOfKeyInBuffer, mdFileLoadedInMemory.str().size());
		second = second.substr(0, second.find_first_of('\n'));
		int secSize = second.size();
		second = mdFileLoadedInMemory.str().substr(posOfKeyInBuffer + secSize + 1, mdFileLoadedInMemory.str().size());

		mdFileLoadedInMemory.seekp(posOfKeyInBuffer);

		mdFileLoadedInMemory.clear();

		mdFileLoadedInMemory.str("");
		mdFileLoadedInMemory << first + newKey + second;
	}


	void Config::SetName(std::string const &name)
	{
		
	}

	std::string Config::GetName()
	{
		return "";
	}

	void Config::BeginConfig(std::string const &windowName)
	{
		/*if (mdFile.is_open())
			md_error("ImGui Config Parser ERROR!: EndConfig() wasn't called.");*/

		mdCurrentWinName = windowName;

		if (mdFileLoadedInMemory.str().empty())
		{
			mdFile.open(mdFileName);
			if (!mdFile.is_open())
				md_error("ImGui Config Parser ERROR: Could not open file %s", mdFileName);
			mdFileLoadedInMemory << mdFile.rdbuf();
			mdOriginalFileContent << mdFileLoadedInMemory.str();
			mdFile.close();
		}
	}

	void Config::SaveFloat(std::string const &name, float val)
	{
		int pos = ValueExists(name);
		if (pos >= 0)
		{
			mdFileLoadedInMemory.seekp(pos);
		}
		else
		{
			SetCursorAtBlankLine();
		}
		mdFileLoadedInMemory.clear();
		std::stringstream ss;
		ss << name << "=" << val << "\n";
		mdFileLoadedInMemory << ss.str();
	}

	void Config::SaveInt(std::string const &name, float val)
	{

	}

	void Config::SaveBool(std::string const &name, float val)
	{

	}

	void Config::SaveString(std::string const &name, float val)
	{

	}

	void Config::SaveVec2(std::string const & name, ImVec2 vec)
	{
		int pos = ValueExists(name);
		if (pos >= 0)
		{
			///ReplaceValuesFloat(2, pos, name, vec.x, vec.y);
			std::string first, second;
			first = mdFileLoadedInMemory.str().substr(0, pos);

			second = mdFileLoadedInMemory.str().substr(pos, mdFileLoadedInMemory.str().size());
			second = second.substr(0, second.find_first_of('\n'));
			int secSize = second.size();
			second = mdFileLoadedInMemory.str().substr(pos + secSize + 1, mdFileLoadedInMemory.str().size());

			std::stringstream ss;
			ss << name << '=' << vec.x << ',' << vec.y << '\n';
			mdFileLoadedInMemory.str("");
			mdFileLoadedInMemory << first + ss.str() + second;
		}
		else
		{
			SetCursorAtBlankLine();

			mdFileLoadedInMemory.clear();
			std::stringstream ss;
			ss << name << "=" << vec.x << "," << vec.y << "\n";
			mdFileLoadedInMemory << ss.str();
			std::string test = mdFileLoadedInMemory.str();
		}

		
	}

	void Config::ReadFloat(std::string const &name, float *val)
	{
		mdFileLoadedInMemory.clear();
		mdFileLoadedInMemory.seekg(0, std::ios::beg);
		std::string buffer;
		while (std::getline(mdFileLoadedInMemory, buffer))
		{
			if (buffer.find(mdCurrentWinName) != std::string::npos)
			{
				while (buffer.find(name) == std::string::npos &&std::getline(mdFileLoadedInMemory, buffer) && buffer.find(name) == std::string::npos) {}
				int pos = buffer.find('=') + 1;
				std::stringstream ss;
				ss << buffer.substr(pos, buffer.size());
				ss >> *val;
				break;
			}
		}
	}

	bool Config::ReadVec2(std::string const &name, ImVec2 *val)
	{
		
		int pos = mdFileLoadedInMemory.str().find(name);
		if (pos >= 0)
		{
			std::string buffer = mdFileLoadedInMemory.str().substr(pos, mdFileLoadedInMemory.str().size());
			buffer = buffer.substr(0, buffer.find_first_of('\n'));
			int pos = buffer.find('=') + 1;
			int comma = buffer.find_first_of(',') + 1;
			std::string temp = buffer;
			std::stringstream ss;
			ss << buffer.substr(pos, comma - pos - 1);
			ss >> val->x;
			ss.str("");
			ss.clear();
			ss << buffer.substr(comma, buffer.size() - comma);
			ss >> val->y;
			return true;
		}

		return false;
	}

	void Config::EndConfig()
	{
		mdCurrentWinName = std::string("");
	}

	void Config::UpdateConfig()
	{
		if (mdFileLoadedInMemory.str() != mdOriginalFileContent.str())
		{
			mdFile.open(mdFileName, std::fstream::out | std::fstream::trunc);
			if (!mdFile.is_open())
				md_error("ImGui Config Parser ERROR: Could not open file %s", mdFileName);
			mdFile << mdFileLoadedInMemory.str();
			mdFile.close();
		}
	}
}