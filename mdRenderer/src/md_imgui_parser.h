#ifndef MD_IMGUI_PARSER_H
#define MD_IMGUI_PARSER_H

#include <iostream>
#include "../external/imgui/imgui.h"

namespace ImGui
{
	namespace Config
	{
		/*
			Still requires better error checking. It's good enought for now.			
		*/

		/* Assume for now that config name is "imgui.ini" as a default */

		void SetName(std::string const &name);

		std::string GetName();
		
		void SaveFloat(std::string const &name, float val, bool replaceExisting = false);

		void SaveInt(std::string const &name, int val, bool replaceExisting = false);

		void SaveBool(std::string const &name, bool val, bool replaceExisting = false);

		void SaveString(std::string const &name, std::string const &val, bool replaceExisting = false);

		void SaveVec2(std::string const & name, ImVec2 vec, bool replaceExisting = false);

		bool ReadFloat(std::string const &name, float *val);

		bool ReadVec2(std::string const &name, ImVec2 *val);

		void UpdateConfig();
	}

}

#endif // !MD_IMGUI_PARSER_H
