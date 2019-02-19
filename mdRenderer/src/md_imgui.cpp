#include "md_imgui.h"

#include "model.h"
#include "debug.h"
#include "graphics_types.h"

namespace ImGui 
{

	float ProgressBar(const char *optionalPrefixText, float value, const float minValue, const float maxValue, const char *format, const ImVec2 &sizeOfBarWithoutTextInPixels, const ImVec4 &colorLeft, const ImVec4 &colorRight, const ImVec4 &colorBorder) 
	{
		if (value < minValue) value = minValue;
		else if (value > maxValue) value = maxValue;
		const float valueFraction = (maxValue == minValue) ? 1.0f : ((value - minValue) / (maxValue - minValue));
		const bool needsPercConversion = strstr(format, "%%") != NULL;

		ImVec2 size = sizeOfBarWithoutTextInPixels;
		if (size.x <= 0) size.x = ImGui::GetWindowWidth()*0.25f;
		if (size.y <= 0) size.y = ImGui::GetTextLineHeightWithSpacing(); // or without

		const ImFontAtlas* fontAtlas = ImGui::GetIO().Fonts;

		if (optionalPrefixText && strlen(optionalPrefixText) > 0) {
			ImGui::AlignFirstTextHeightToWidgets();
			ImGui::Text(optionalPrefixText);
			ImGui::SameLine();
		}

		if (valueFraction > 0) {
			ImGui::Image(fontAtlas->TexID, ImVec2(size.x*valueFraction, size.y), fontAtlas->TexUvWhitePixel, fontAtlas->TexUvWhitePixel, colorLeft, colorBorder);
		}
		if (valueFraction < 1) {
			if (valueFraction > 0) ImGui::SameLine(0, 0);
			ImGui::Image(fontAtlas->TexID, ImVec2(size.x*(1.f - valueFraction), size.y), fontAtlas->TexUvWhitePixel, fontAtlas->TexUvWhitePixel, colorRight, colorBorder);
		}
		ImGui::SameLine();

		ImGui::Text(format, needsPercConversion ? (valueFraction*100.f + 0.0001f) : value);
		return valueFraction;
	}

	void ImGui::AnimationController(md::engine::graphics::Animations const *animVec, const char *format, const ImVec2 &sizeOfBarWithoutTextInPixels, const ImVec4 &colorLeft, const ImVec4 &colorRight, const ImVec4 &colorBorder)
	{
		
		const float valueFraction = 0.5f;
		const bool needsPercConversion = strstr(format, "%%") != NULL;

		ImVec2 size(400, 400);
		if (size.x <= 0) size.x = ImGui::GetWindowWidth()*0.25f;
		if (size.y <= 0) size.y = ImGui::GetTextLineHeightWithSpacing(); // or without

		const ImFontAtlas* fontAtlas = ImGui::GetIO().Fonts;

		std::string txt = "test";
		if (txt.c_str() && strlen(txt.c_str()) > 0) {
			ImGui::AlignFirstTextHeightToWidgets();
			ImGui::Text(txt.c_str());
			ImGui::SameLine();
		}

		ImVec4 col(0.0, 0.0, 0.0, 0.5);
		ImVec4 colBorder(1.0, 0.0, 0.0, 0.5);
		
		// Draw Frame
		ImGui::BeginChild("Test", size, true);
		ImVec2 winPos = ImGui::GetWindowPos();
		ImVec2 p(100 + winPos.x, 180 + winPos.y);
		ImGui::GetWindowDrawList()->AddLine(p, ImVec2(p.x + 50, p.y + 50), IM_COL32(255, 0, 0, 255), 3.0f);
		ImGui::GetWindowDrawList()->AddCircle(p, 50.f, IM_COL32(255, 0, 0, 255), 36);

		ImGui::EndChild();



		
	}

#include <math.h> // fmodf

	// NB: You can use math functions/operators on ImVec2 if you #define IMGUI_DEFINE_MATH_OPERATORS and #include "imgui_internal.h"
	// Here we only declare simple +/- operators so others don't leak into the demo code.
	static inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y); }
	static inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x - rhs.x, lhs.y - rhs.y); }

	static inline ImVec2 distance(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2((lhs.x + rhs.x) / 2.f, (lhs.y + rhs.y) / 2.f); }



	// Really dumb data structure provided for the example.
	// Note that we storing links are INDICES (not ID) to make example code shorter, obviously a bad idea for any general purpose code.


	void ImGui::ShowExampleAppCustomNodeGraph(md::engine::graphics::Animations *animVec)
	{

		// Dummy
		struct Node
		{
			int     ID;
			char    Name[32];
			ImVec2  Pos, Size;
			float   Value;
			ImVec4  Color;
			int		Slots;

			Node(int id, const char* name, const ImVec2& pos, float value, const ImVec4& color, int slotsCount) {
				ID = id; strncpy_s(Name, name, 31); Name[31] = 0; Pos = pos; Value = value; Color = color; Slots = slotsCount;
				
			}

			ImVec2 GetInputSlotPos(int slot_no) const { return ImVec2(Pos.x + Size.x / 2.f, Pos.y + Size.y / 2.f); }
			ImVec2 GetOutputSlotPos(int slot_no) const { return ImVec2(Pos.x + Size.x / 2.f, Pos.y + Size.y / 2.f); }
		};

		struct NodeLink
		{
			int     InputIdx, InputSlot, OutputIdx, OutputSlot;
			bool	dir; // true = right, false = left

			NodeLink(int input_idx, int input_slot, int output_idx, int output_slot) { InputIdx = input_idx; InputSlot = input_slot; OutputIdx = output_idx; OutputSlot = output_slot; }
		};

		ImGui::SetNextWindowSize(ImVec2(700, 600), ImGuiSetCond_FirstUseEver);
		if (!ImGui::Begin("Example: Custom Node Graph"))
		{
			ImGui::End();
			return;
		}
		

		static ImVector<Node> nodes;
		static ImVector<NodeLink> links;
		static bool inited = false;
		static ImVec2 scrolling = ImVec2(0.0f, 0.0f);
		static bool show_grid = true;
		static int node_selected = -1;
		if (!inited)
		{
			int animCounter = 0;
			int *slots = new int[(*animVec).size()];
			memset(slots, 0, sizeof(slots) * (*animVec).size());
			for (auto & i : *animVec)
			{
				int slotsCount = 0;
				for (auto & t : i.second->mTransitions)
				{
					auto it = std::find_if(animVec->begin(), animVec->end(), [&](std::pair<std::string, md::engine::graphics::anim_t*> const & ref)
					{ return ref.first == t.mNextAnimName; });
					if (it != animVec->end())
					{
						slots[std::distance((*animVec).begin(), it)]++;
					}

					slots[animCounter]++;
				}

				animCounter++;
			}

			for (size_t i = 0; i < (*animVec).size(); i++)
			{
				md_log("%d", slots[i]);
			}

			animCounter = 0;
			ImVec2 startPos(40, 50);
			for (auto & i : *animVec)
			{
				nodes.push_back(Node(animCounter, i.first.c_str(), ImVec2(startPos.x, startPos.y), 0.5f, ImColor(255, 100, 100), slots[animCounter]));

				startPos.y += 100.f;
				animCounter++;
			}

			animCounter = 0;
			for (auto & i : *animVec)
			{
				int transCounter = 0;
				for (auto & t : i.second->mTransitions)
				{
					int outID = 0;
					auto it = std::find_if(nodes.begin(), nodes.end(), [&](Node const & ref) { return ref.Name == t.mNextAnimName; });
					if (it != nodes.end())
					{
						outID = it->ID;
					}

					links.push_back(NodeLink(animCounter, transCounter, outID, 0));
					transCounter++;
				}
				
				animCounter++;
			}

			/*nodes.push_back(Node(0, "MainTex", ImVec2(40, 50), 0.5f, ImColor(255, 100, 100), 1, 1));
			nodes.push_back(Node(1, "BumpMap", ImVec2(40, 150), 0.42f, ImColor(200, 100, 200), 1, 1));
			nodes.push_back(Node(2, "Combine", ImVec2(270, 80), 1.0f, ImColor(0, 200, 100), 2, 2));
			links.push_back(NodeLink(0, 0, 2, 0));
			links.push_back(NodeLink(1, 0, 2, 1));*/
			inited = true;
		}

		// Draw a list of nodes on the left side
		bool open_context_menu = false;
		int node_hovered_in_list = -1;
		int node_hovered_in_scene = -1;
		ImGui::BeginChild("node_list", ImVec2(100, 0));
		ImGui::Text("Nodes");
		ImGui::Separator();
		for (int node_idx = 0; node_idx < nodes.Size; node_idx++)
		{
			Node* node = &nodes[node_idx];
			ImGui::PushID(node->ID);
			if (ImGui::Selectable(node->Name, node->ID == node_selected))
				node_selected = node->ID;
			if (ImGui::IsItemHovered())
			{
				node_hovered_in_list = node->ID;
				open_context_menu |= ImGui::IsMouseClicked(1);
			}
			ImGui::PopID();
		}
		ImGui::EndChild();

		ImGui::SameLine();
		ImGui::BeginGroup();

		const float NODE_SLOT_RADIUS = 4.0f;
		const ImVec2 NODE_WINDOW_PADDING(8.0f, 8.0f);

		// Create our child canvas
		ImGui::Text("Hold middle mouse button to scroll (%.2f,%.2f)", scrolling.x, scrolling.y);
		ImGui::SameLine(ImGui::GetWindowWidth() - 100);
		ImGui::Checkbox("Show grid", &show_grid);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1, 1));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, IM_COL32(60, 60, 70, 200));
		ImGui::BeginChild("scrolling_region", ImVec2(0, 0), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove);
		ImGui::PushItemWidth(120.0f);

		ImVec2 offset = ImGui::GetCursorScreenPos() + scrolling;
		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		// Display grid
		if (show_grid)
		{
			ImU32 GRID_COLOR = IM_COL32(200, 200, 200, 40);
			float GRID_SZ = 64.0f;
			ImVec2 win_pos = ImGui::GetCursorScreenPos();
			ImVec2 canvas_sz = ImGui::GetWindowSize();
			for (float x = fmodf(scrolling.x, GRID_SZ); x < canvas_sz.x; x += GRID_SZ)
				draw_list->AddLine(ImVec2(x, 0.0f) + win_pos, ImVec2(x, canvas_sz.y) + win_pos, GRID_COLOR);
			for (float y = fmodf(scrolling.y, GRID_SZ); y < canvas_sz.y; y += GRID_SZ)
				draw_list->AddLine(ImVec2(0.0f, y) + win_pos, ImVec2(canvas_sz.x, y) + win_pos, GRID_COLOR);
		}

		// Display links
		draw_list->ChannelsSplit(2);
		draw_list->ChannelsSetCurrent(0); // Background
		for (int link_idx = 0; link_idx < links.Size; link_idx++)
		{
			NodeLink* link = &links[link_idx];
			Node* node_inp = &nodes[link->InputIdx];
			Node* node_out = &nodes[link->OutputIdx];
			ImVec2 p1 = offset + node_inp->GetOutputSlotPos(link->InputSlot);
			ImVec2 p2 = offset + node_out->GetInputSlotPos(link->OutputSlot);
			
			int spacing = 10.f;
			bool singleTransitionLine = true;
			for (int i = 0; i < links.Size; i++)
			{
				if (links[i].OutputIdx == link->InputIdx && node_out->Slots > 1)
				{
					// out = red
					// in = yellow
					
					draw_list->AddLine(p1 + ImVec2(spacing, spacing), p2 + ImVec2(spacing, spacing), IM_COL32(255, 0, 0, 255), 2.0f);
					draw_list->AddLine(p1 + ImVec2(-spacing, -spacing), p2 + ImVec2(-spacing, -spacing), IM_COL32(200, 200, 100, 255), 2.0f);
					singleTransitionLine = false;
					break;
				}
			}
			if (singleTransitionLine)
			{
				ImVec4 color(200, 200, 100, 255);
				if (animVec->at(node_out->Name)->mTransitions.empty())
					color = ImVec4(255, 0, 0, 255);
				draw_list->AddLine(p1, p2, IM_COL32(color.x, color.y, color.z, color.w), 2.0f);
				ImVec2 centre = distance(p1, p2);
				float h = 15;
				float a = 10;
				float tga = (p1.y - p2.y) / (p1.x - p2.x);
				ImVec2 pa = centre + ImVec2(a / 2.f, 0.f);
				ImVec2 pb = centre - ImVec2(a / 2.f, 0.f);
				ImVec2 pc = centre + ImVec2(0.f, -h);
				draw_list->AddTriangleFilled(pa, pb, pc, IM_COL32(150, 150, 150, 150));
			}
		
		}

		// Display nodes
		for (int node_idx = 0; node_idx < nodes.Size; node_idx++)
		{
			Node* node = &nodes[node_idx];
			ImGui::PushID(node->ID);
			ImVec2 node_rect_min = offset + node->Pos;

			// Display node contents first
			draw_list->ChannelsSetCurrent(1); // Foreground
			bool old_any_active = ImGui::IsAnyItemActive();
			ImGui::SetCursorScreenPos(node_rect_min + NODE_WINDOW_PADDING);
			ImGui::BeginGroup(); // Lock horizontal position
			ImGui::Text("%s", node->Name);
			ImGui::SliderFloat("##value", &node->Value, 0.0f, 1.0f, "Alpha %.2f");
			ImGui::ColorEdit3("##color", &node->Color.x);
			ImGui::EndGroup();

			// Save the size of what we have emitted and whether any of the widgets are being used
			bool node_widgets_active = (!old_any_active && ImGui::IsAnyItemActive());
			node->Size = ImGui::GetItemRectSize() + NODE_WINDOW_PADDING + NODE_WINDOW_PADDING;
			ImVec2 node_rect_max = node_rect_min + node->Size;

			// Display node box
			draw_list->ChannelsSetCurrent(0); // Background
			ImGui::SetCursorScreenPos(node_rect_min);
			ImGui::InvisibleButton("node", node->Size);
			if (ImGui::IsItemHovered())
			{
				node_hovered_in_scene = node->ID;
				open_context_menu |= ImGui::IsMouseClicked(1);
			}
			bool node_moving_active = ImGui::IsItemActive();
			if (node_widgets_active || node_moving_active)
				node_selected = node->ID;
			if (node_moving_active && ImGui::IsMouseDragging(0))
				node->Pos = node->Pos + ImGui::GetIO().MouseDelta;

			ImU32 node_bg_color = (node_hovered_in_list == node->ID || node_hovered_in_scene == node->ID || (node_hovered_in_list == -1 && node_selected == node->ID)) ? IM_COL32(75, 75, 75, 255) : IM_COL32(60, 60, 60, 255);
			draw_list->AddRectFilled(node_rect_min, node_rect_max, node_bg_color, 4.0f);
			draw_list->AddRect(node_rect_min, node_rect_max, IM_COL32(100, 100, 100, 255), 4.0f);
			/*for (int slot_idx = 0; slot_idx < node->InputsCount; slot_idx++)
				draw_list->AddCircleFilled(offset + node->GetInputSlotPos(slot_idx), NODE_SLOT_RADIUS, IM_COL32(150, 150, 150, 150));
			for (int slot_idx = 0; slot_idx < node->OutputsCount; slot_idx++)
				draw_list->AddCircleFilled(offset + node->GetOutputSlotPos(slot_idx), NODE_SLOT_RADIUS, IM_COL32(150, 150, 150, 150));*/

			ImGui::PopID();
		}
		draw_list->ChannelsMerge();

		// Open context menu
		if (!ImGui::IsAnyItemHovered() && ImGui::IsMouseHoveringWindow() && ImGui::IsMouseClicked(1))
		{
			node_selected = node_hovered_in_list = node_hovered_in_scene = -1;
			open_context_menu = true;
		}
		if (open_context_menu)
		{
			ImGui::OpenPopup("context_menu");
			if (node_hovered_in_list != -1)
				node_selected = node_hovered_in_list;
			if (node_hovered_in_scene != -1)
				node_selected = node_hovered_in_scene;
		}

		// Draw context menu
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
		if (ImGui::BeginPopup("context_menu"))
		{
			Node* node = node_selected != -1 ? &nodes[node_selected] : NULL;
			ImVec2 scene_pos = ImGui::GetMousePosOnOpeningCurrentPopup() - offset;
			if (node)
			{
				ImGui::Text("Node '%s'", node->Name);
				ImGui::Separator();
				if (ImGui::MenuItem("Rename..", NULL, false, false)) {}
				if (ImGui::MenuItem("Delete")) 
				{ 
					for (int i = 0; i < links.Size; i++)
					{
						if (&nodes[links[i].InputIdx] == node)
						{
							links.erase(links.begin() + i);
							for (int j = 0; j < links.Size; j++)
							{
								if (links[j].InputIdx > i)
									links[j].InputIdx--;
								if (links[j].OutputIdx > i)
									links[j].OutputIdx--;
							}
							break;
						}
					}
					nodes.erase(node); 
				}
				if (ImGui::MenuItem("Copy", NULL, false, false)) {}
			}
			else
			{
				if (ImGui::MenuItem("Add")) { nodes.push_back(Node(nodes.Size, "New node", scene_pos, 0.5f, ImColor(100, 100, 200), 2)); }
				if (ImGui::MenuItem("Paste", NULL, false, false)) {}
			}
			ImGui::EndPopup();
		}
		ImGui::PopStyleVar();

		// Scrolling
		if (ImGui::IsWindowHovered() && !ImGui::IsAnyItemActive() && ImGui::IsMouseDragging(2, 0.0f))
			scrolling = scrolling + ImGui::GetIO().MouseDelta;

		ImGui::PopItemWidth();
		ImGui::EndChild();
		ImGui::PopStyleColor();
		ImGui::PopStyleVar(2);
		ImGui::EndGroup();

		ImGui::End();

		for (auto & i : nodes)
		{
			//for (auto j : i.Slots)
				//delete j;
		}
	}


	void TestProgressBar() {
		static float progress = 0; static float progressSign = 1.f;
		progress += progressSign * .0001f; if (progress >= 1.f || progress <= 0.f) progressSign *= -1.f;
		// No IDs needed for ProgressBars:
		ImGui::ProgressBar("ProgressBar", progress);
		ImGui::ProgressBar("ProgressBar", 1.f - progress);
		ImGui::ProgressBar("", 500 + progress * 1000, 500, 1500, "%4.0f (absolute value in [500,1500] and fixed bar size)", ImVec2(150, -1));
		ImGui::ProgressBar("", 500 + progress * 1000, 500, 1500, "%3.0f%% (same as above, but with percentage and new colors)", ImVec2(150, -1), ImVec4(0.7, 0.7, 1, 1), ImVec4(0.05, 0.15, 0.5, 0.8), ImVec4(0.8, 0.8, 0, 1));
	}
}