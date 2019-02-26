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

		void OpenConfig();

		template <typename T>
		void WriteKeyWithValue(std::string const &key, std::vector<T> vals, bool replaceExisting);

		
		std::vector<float> ReadKeyWithValue(std::string const &key);
		std::vector<std::string> ReadKeyWithString(std::string const &key);
	}
	
	void Config::OpenConfig()
	{
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
	
	template <typename T>
	void Config::WriteKeyWithValue(std::string const &key, std::vector<T> vals, bool replaceExisting)
	{
		OpenConfig();

		std::string buffer = mdFileLoadedInMemory.str();
		std::string newStr(key + "=");
		std::stringstream ss;
		for (auto v : vals)
		{
			ss.str("");
			ss << v;
			newStr += ss.str();
			newStr += ',';
		}
		newStr[newStr.size() - 1] = '\n';
		newStr[newStr.size()] = 0;
		

		int keyPos = buffer.find(key);
		if (keyPos >= 0 && replaceExisting)
		{
			// If key already exists && needs to be replaced with new values
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
		else if(keyPos < 0)
		{
			// If key doesn't exist
			if (mdFileLoadedInMemory.str().back() != '\n')
				mdFileLoadedInMemory.str(mdFileLoadedInMemory.str().substr(0, mdFileLoadedInMemory.str().size()) + '\n');

			std::string test = mdFileLoadedInMemory.str();
			mdFileLoadedInMemory.str(mdFileLoadedInMemory.str() + newStr);
			test = mdFileLoadedInMemory.str();
		}
	}
	
	std::vector<float> Config::ReadKeyWithValue(std::string const &key)
	{
		OpenConfig();

		std::vector<float> vals;
		float val;
		int pos = mdFileLoadedInMemory.str().find(key);
		if (pos >= 0)
		{
			// Extract the part with numbers only
			std::string buffer = mdFileLoadedInMemory.str().substr(pos, mdFileLoadedInMemory.str().size());
			int nl_pos = buffer.find_first_of('\n');
			buffer = buffer.substr(key.size() + 1, nl_pos - key.size() - 1);

			int comma_count = std::count(buffer.begin(), buffer.end(), ',');
			int start_pos = 0;
			// There is always (comma_count + 1) numbers that need to be read
			for (int i = 0; i < comma_count + 1; i++)
			{
				std::stringstream ss;
				int comma_pos = buffer.find_first_of(',') + 1;
				ss << buffer.substr(start_pos, comma_pos - 1);
				ss >> val;
				vals.push_back(val);
				start_pos = comma_pos;
			}

			return vals;
		}

		return vals;
	}

	std::vector<std::string> Config::ReadKeyWithString(std::string const &key)
	{
		OpenConfig();

		std::vector<std::string> vals;
		std::string val;
		int pos = mdFileLoadedInMemory.str().find(key);
		if (pos >= 0)
		{
			std::string buffer = mdFileLoadedInMemory.str().substr(pos, mdFileLoadedInMemory.str().size());
			buffer = buffer.substr(0, buffer.find_first_of('\n'));
			int pos = buffer.find('=') + 1;
			int comma = buffer.find_first_of(',') + 1;
			std::string temp = buffer;
			std::stringstream ss;
			ss << buffer.substr(pos, comma - pos - 1);
			ss >> val;
			vals.push_back(val);
			ss.str("");
			ss.clear();
			ss << buffer.substr(comma, buffer.size() - comma);
			ss >> val;
			vals.push_back(val);

			return vals;
		}

		return vals;
	}

	void Config::SetName(std::string const &name)
	{
		
	}

	std::string Config::GetName()
	{
		return "";
	}

	void Config::SaveFloat(std::string const &name, float val, bool replaceExisting)
	{
		WriteKeyWithValue(name, std::vector<float>{val}, replaceExisting);
	}

	void Config::SaveInt(std::string const &name, int val, bool replaceExisting)
	{
		WriteKeyWithValue(name, std::vector<int>{val}, replaceExisting);
	}

	void Config::SaveBool(std::string const &name, bool val, bool replaceExisting)
	{
		WriteKeyWithValue(name, std::vector<int>{val}, replaceExisting);
	}

	void Config::SaveString(std::string const &name, std::string const &val, bool replaceExisting)
	{
		WriteKeyWithValue(name, std::vector<std::string>{val}, replaceExisting);
	}

	void Config::SaveVec2(std::string const & name, ImVec2 vec, bool replaceExisting)
	{
		WriteKeyWithValue(name, std::vector<float>{vec.x, vec.y}, replaceExisting);
	}

	bool Config::ReadFloat(std::string const &name, float *val)
	{
		auto vals = ReadKeyWithValue(name);
		if (!vals.empty())
		{
			*val = vals[0];
			return true;
		}

		return false;
	}

	bool Config::ReadVec2(std::string const &name, ImVec2 *val)
	{
		auto vals = ReadKeyWithValue(name);
		if (!vals.empty())
		{
			val->x = vals[0];
			val->y = vals[1];
			return true;
		}

		return false;
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