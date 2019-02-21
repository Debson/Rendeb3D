#ifndef MD_IMGUI_PARSER_H
#define MD_IMGUI_PARSER_H

#include <iostream>
#include "../external/imgui/imgui.h"

namespace ImGui
{
	namespace Config
	{
		/* Assume for now that config name is "imgui.ini" as a default */

		void SetName(std::string const &name);

		std::string GetName();

		// Start writing under the "Window Name" values in config
		void BeginConfig(std::string const &windowName);
		
		void SaveFloat(std::string const &name, float val);

		void SaveInt(std::string const &name, float val);

		void SaveBool(std::string const &name, float val);

		void SaveString(std::string const &name, float val);

		void SaveVec2(std::string const & name, ImVec2 vec);

		void ReadFloat(std::string const &name, float *val);

		bool ReadVec2(std::string const &name, ImVec2 *val);

		void EndConfig();

		void UpdateConfig();
	}

}

#endif // !MD_IMGUI_PARSER_H
