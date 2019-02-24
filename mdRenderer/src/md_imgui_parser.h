#ifndef MD_IMGUI_PARSER_H
#define MD_IMGUI_PARSER_H

#include <iostream>
#include "../external/imgui/imgui.h"

namespace ImGui
{
	namespace Config
	{
		/*
		
			fun WriteKeyWithVal(string, string, values...)
			if key doesn't exist, go to end of the file and inser it
			if exist, find pos of its first letter, then find first_of('\n') or if doesn't exist, 
			then assume that last pos will be eof
			this will allow to use one function with multiple values
			write overloaded function for float and int
		
		*/


		/* Assume for now that config name is "imgui.ini" as a default */

		void SetName(std::string const &name);

		std::string GetName();

		// Start writing under the "Window Name" values in config
		void BeginConfig(std::string const &windowName);
		
		void SaveFloat(std::string const &name, float val);

		void SaveInt(std::string const &name, int val);

		void SaveBool(std::string const &name, bool val);

		void SaveString(std::string const &name, std::string const &val);

		void SaveVec2(std::string const & name, ImVec2 vec);

		void ReadFloat(std::string const &name, float *val);

		bool ReadVec2(std::string const &name, ImVec2 *val);

		void EndConfig();

		void UpdateConfig();
	}

}

#endif // !MD_IMGUI_PARSER_H
