#include "md_imgui_parser.h"

#include <fstream>
#include <sstream>
#include <stdarg.h>
#include <vector>

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

		template <typename T>
		void WriteKeyWithValue(std::string const &key, std::vector<T> vals);

		template <typename T>
		std::vector<T> ReadKeyWithValue(std::string const &key);
	}

	template <typename T>
	void Config::WriteKeyWithValue(std::string const &key, std::vector<T> vals)
	{
		std::string buffer = mdFileLoadedInMemory.str();
		std::string newStr(key + "=");
		
		for (auto v : vals)
		{
			newStr += std::to_string(v);
			newStr += ',';
		}
		newStr[newStr.size() - 1] = '\n';
		newStr[newStr.size()] = 0;
		

		int keyPos = buffer.find(key);
		if (keyPos >= 0)
		{
			int newLinePos = buffer.substr(keyPos + 1, buffer.size()).find_first_of('\n');
			if (mdFileLoadedInMemory.str().at(keyPos) != '\n')
				mdFileLoadedInMemory.str().at(keyPos + 1) = '\n';
			std::string firstHalf, secondHalf;
			if (newLinePos >= 0)
			{
				firstHalf = buffer.substr(0, keyPos);
				secondHalf = buffer.substr(keyPos + newLinePos + 2, buffer.size()); // + 1 from keyPos && + 1 from newLinePos
				mdFileLoadedInMemory.str(firstHalf + newStr + secondHalf);
			}
			else
			{
				mdFileLoadedInMemory.str() = mdFileLoadedInMemory.str().substr(0, keyPos) + newStr;
				mdFileLoadedInMemory.str().at(keyPos + newStr.size() + 1) = 0;
			}
		}
		else
		{
			if (mdFileLoadedInMemory.str().back() != '\n')
				mdFileLoadedInMemory.str(mdFileLoadedInMemory.str().substr(0, mdFileLoadedInMemory.str().size()) + '\n');

			std::string test = mdFileLoadedInMemory.str();
			mdFileLoadedInMemory.str(mdFileLoadedInMemory.str() + newStr);
			test = mdFileLoadedInMemory.str();
		}
	}

	template <typename T>
	std::vector<T> Config::ReadKeyWithValue(std::string const &key)
	{
		/*	Not finished. If val doesn;t have '.', its int otherwise it's float, if has letters, it's string.
			put constructing a new value from string till finish of the line(line MUST END WITH eol sign)
		*/
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
			//WriteKeyWithValue("test", std::vector<float>{10.f, 20.f, 12.12f});

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
		WriteKeyWithValue(name, std::vector<float>{val});
	}

	void Config::SaveInt(std::string const &name, int val)
	{
		WriteKeyWithValue(name, std::vector<int>{val});
	}

	void Config::SaveBool(std::string const &name, bool val)
	{
		WriteKeyWithValue(name, std::vector<int>{val});
	}

	void Config::SaveString(std::string const &name, std::string const &val)
	{
		WriteKeyWithValue(name, std::vector<std::string>{val});
	}

	void Config::SaveVec2(std::string const & name, ImVec2 vec)
	{
		WriteKeyWithValue(name, std::vector<float>{vec.x, vec.y});
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