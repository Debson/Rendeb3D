#include "md_imgui.h"

#include "model.h"
#include "debug.h"
#include "graphics_types.h"
#include "md_imgui_parser.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "../external/imgui/imgui_internal.h"


#include <math.h>

namespace ImGui 
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
	static ImVector<Node> nodes;

	static inline ImVec2 distance(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2((lhs.x + rhs.x) / 2.f, (lhs.y + rhs.y) / 2.f); }

	// Really dumb data structure provided for the example.
	// Note that we storing links are INDICES (not ID) to make example code shorter, obviously a bad idea for any general purpose code.

	void ImGui::AnimationController(md::engine::graphics::Animations *animVec, md::engine::graphics::transition_t *currTrans)
	{

		struct NodeLink
		{
			int     InputIdx, InputSlot, OutputIdx, OutputSlot;
			bool	dir; // true = right, false = left

			NodeLink(int input_idx, int input_slot, int output_idx, int output_slot) { InputIdx = input_idx; InputSlot = input_slot; OutputIdx = output_idx; OutputSlot = output_slot; }
		};

		ImGui::SetNextWindowSize(ImVec2(700, 600), ImGuiSetCond_FirstUseEver);
		std::string windowName = "Example: Custom Node Graph";
		if (!ImGui::Begin(windowName.c_str()))
		{
			ImGui::End();
			return;
		}

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
					auto it = std::find_if(animVec->begin(), animVec->end(), [&](std::pair<std::string, std::shared_ptr<md::engine::graphics::anim_t>> const & ref)
					{ return ref.first == t.mNextAnimName; });
					if (it != animVec->end())
					{
						slots[std::distance((*animVec).begin(), it)]++;
					}

					slots[animCounter]++;
				}

				animCounter++;
			}

			animCounter = 0;
			ImVec2 startPos(40, 50);
			Config::BeginConfig(windowName);
			for (auto & i : *animVec)
			{
				ImVec2 pos;
				if(Config::ReadVec2(i.first, &pos))
				{ 
					nodes.push_back(Node(animCounter, i.first.c_str(), pos, 0.5f, ImColor(255, 100, 100), slots[animCounter]));
				}
				else
				{
					nodes.push_back(Node(animCounter, i.first.c_str(), startPos, 0.5f, ImColor(255, 100, 100), slots[animCounter]));
					Config::SaveVec2(i.first, startPos);
					startPos.y += 100.f;
				}

				animCounter++;
			}
			Config::EndConfig();
			Config::UpdateConfig();

			delete slots;

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
		std::vector<bool> drawn(links.Size);


		int activeLinkID = -1;
		int activeNodeFirstID = -1;
		int activeNodeSecondID = -1;

		// Process of finding active link, when transition is playing
		if (currTrans)
		{
			for (auto & anim : *animVec)
			{
				for (auto & trans : anim.second->mTransitions)
				{
					if (currTrans == &trans)
					{
						auto it = find_if(animVec->begin(), animVec->end(), [&]
						(std::pair<std::string, std::shared_ptr<md::engine::graphics::anim_t>> const & ref) { return ref.first == anim.first; });
						activeNodeFirstID = std::distance(animVec->begin(), it);
						break;
					}
				}
			}
			for (auto & node : nodes)
			{
				if (currTrans->mNextAnimName == node.Name)
					activeNodeSecondID = node.ID;
			}
		}

		for (int i = 0; i < links.Size; i++)
		{
			if (links[i].InputIdx == activeNodeFirstID && links[i].OutputIdx == activeNodeSecondID)
				activeLinkID = i;
		}

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
				if (links[i].OutputIdx == link->InputIdx && node_out->Slots > 1 && (!drawn[link_idx] && !drawn[i]))
				{
					// out = red
					// in = yellow
					float h = -6;
					float a = 8.f;
					ImVec2 p1_space = p1 + ImVec2(spacing, spacing);
					ImVec2 p2_space = p2 + ImVec2(spacing, spacing);
					ImVec2 centre = distance(p1_space, p2_space);
					// Triangle height and base/2 dimensions
					// Coordinates of triangle
					ImVec2 pa, pb, pc;
					// Create a vector from two points
					ImVec2 vec = p2_space - p1_space;
					// That's a vector parallel to OX
					ImVec2 vecOX(INT_MAX, 0);
					// cos(x) = (a * b) / sqrt(a^2) * sqrt(b^2)    where a and b are vectors that angle between them need to be found
					float ab = vec.x * vecOX.x + vec.y + vecOX.y;
					float a_length = sqrt(vec.x * vec.x + vec.y * vec.y);
					float b_length = sqrt(vecOX.x * vecOX.x + vecOX.y * vecOX.y);
					float cos = ab / (a_length * b_length);
					// Calculate A, bcoordinates
					pa = centre + ImVec2(-a * sqrt(1 - cos * cos), (p1_space.y < centre.y ? 1 : -1) * a * cos);
					pb = centre + ImVec2(a * sqrt(1 - cos * cos), (p1_space.y < centre.y ? -1 : 1) * a * cos);
					// Calculate C coordinate
					float dist = sqrt((p2_space.x - p1_space.x) * (p2_space.x - p1_space.x) + (p2_space.y - p1_space.y) * (p2_space.y - p1_space.y));
					pc.x = centre.x + (h / dist) * (p2_space.x - p1_space.x);
					pc.y = centre.y + (h / dist) * (p2_space.y - p1_space.y);
					
					pa.x -= (2.5f *h  / dist) * (p2_space.x - p1_space.x);
					pa.y -= (2.5f *h  / dist) * (p2_space.y - p1_space.y);

					pb.x -= (2.5f *h  / dist) * (p2_space.x - p1_space.x);
					pb.y -= (2.5f *h  / dist) * (p2_space.y - p1_space.y);

					draw_list->AddLine(p1_space, p2_space, IM_COL32(150, 150, 150, 255), 2.0f);

					if (i == activeLinkID)
					{
						float p = currTrans->mTimeElapsed / currTrans->mDuration;
						p1_space.x = p2_space.x - p * (p2_space.x - p1_space.x);
						p1_space.y = p2_space.y - p * (p2_space.y - p1_space.y);
						draw_list->AddLine(p1_space, p2_space, IM_COL32(0, 255, 0, 255), 2.0f);

						md_log("out! %f", p);
					}
					
					draw_list->AddTriangleFilled(pa, pb, pc, IM_COL32(150, 150, 150, 255));

					p1_space = p1 + ImVec2(-spacing, -spacing);
					p2_space = p2 + ImVec2(-spacing, -spacing);
					h *= -1;
					centre = distance(p1_space, p2_space);
					// Triangle height and base/2 dimensions
				
					// Create a vector from two points
					vec = p2_space - p1_space;
					
					// cos(x) = (a * b) / sqrt(a^2) * sqrt(b^2)    where a and b are vectors that angle between them need to be found
					ab = vec.x * vecOX.x + vec.y + vecOX.y;
					a_length = sqrt(vec.x * vec.x + vec.y * vec.y);
					b_length = sqrt(vecOX.x * vecOX.x + vecOX.y * vecOX.y);
					cos = ab / (a_length * b_length);
					// Calculate A, bcoordinates
					pa = centre + ImVec2(-a * sqrt(1 - cos * cos), (p1_space.y < centre.y ? 1 : -1) * a * cos);
					pb = centre + ImVec2(a * sqrt(1 - cos * cos), (p1_space.y < centre.y ? -1 : 1) * a * cos);
					// Calculate C coordinate
					dist = sqrt((p2_space.x - p1_space.x) * (p2_space.x - p1_space.x) + (p2_space.y - p1_space.y) * (p2_space.y - p1_space.y));
					pc.x = centre.x + (h / dist) * (p2_space.x - p1_space.x);
					pc.y = centre.y + (h / dist) * (p2_space.y - p1_space.y);


					pa.x -= (2.5f *h  / dist) * (p2_space.x - p1_space .x);
					pa.y -= (2.5f *h / dist) * (p2_space.y - p1_space.y);

					pb.x -= (2.5f *h  / dist) * (p2_space.x - p1_space.x);
					pb.y -= (2.5f *h  / dist) * (p2_space.y - p1_space.y);

					draw_list->AddLine(p1_space , p2_space, IM_COL32(150, 150, 150, 255), 2.0f);

					// Draw transition progress
					
					if (link_idx == activeLinkID)
					{
						float p = currTrans->mTimeElapsed / currTrans->mDuration;
						p2_space.x = p1_space.x + (p * dist / dist) * (p2_space.x - p1_space.x);
						p2_space.y = p1_space.y + (p * dist / dist) * (p2_space.y - p1_space.y);
						draw_list->AddLine(p1_space, p2_space, IM_COL32(0, 255, 0, 255), 2.0f);

						md_log("in! %f", p);
					}
					
					draw_list->AddTriangleFilled(pa, pb, pc, IM_COL32(150, 150, 150, 255));

					drawn[link_idx] = true;
					drawn[i] = true;

					singleTransitionLine = false;
					break;
				}
			}

			if (singleTransitionLine && !drawn[link_idx])
			{
				ImVec4 color(150, 150, 150, 255);
				float h = 6;
				float a = 8.f;
				auto anim = animVec->at(node_out->Name);
				/*bool isOut = true;
				for (auto & trans : anim->mTransitions)
				{
					if (trans.mNextAnimName == anim->mName)
					{
						isOut = false;
						break;
					}
				}
				if (anim->mTransitions.empty())
					isOut = false;
				if (isOut)
				{
					color = ImVec4(200, 200, 100, 255);
				}*/

				draw_list->AddLine(p1, p2, IM_COL32(color.x, color.y, color.z, color.w), 2.0f);

				// Calculate the coordinates of centre between two points;
				ImVec2 centre = distance(p1, p2);
				// Triangle height and base/2 dimensions
				// Coordinates of triangle
				ImVec2 pa, pb, pc;
				// Create a vector from two points
				ImVec2 vec = p2 - p1;
				// That's a vector parallel to OX
				ImVec2 vecOX(INT_MAX, 0);
				// cos(x) = (a * b) / sqrt(a^2) * sqrt(b^2)    where a and b are vectors that angle between them need to be found
				float ab = vec.x * vecOX.x + vec.y + vecOX.y;
				float a_length = sqrt(vec.x * vec.x + vec.y * vec.y);
				float b_length = sqrt(vecOX.x * vecOX.x + vecOX.y * vecOX.y);
				float cos = ab / (a_length * b_length);
				// Calculate A, bcoordinates
				pa = centre + ImVec2(-a * sqrt(1 - cos * cos), (p1.y < centre.y ? 1 : -1) * a * cos);
				pb = centre + ImVec2(a * sqrt(1 - cos * cos), (p1.y < centre.y ? -1 : 1) * a * cos);
				// Calculate C coordinate
				float dist = sqrt((p2.x - p1.x) * (p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y));

				pa.x -= (2.5 * h / dist) * (p2.x - p1.x);
				pa.y -= (2.5 * h / dist) * (p2.y - p1.y);

				pb.x -= (2.5 * h / dist) * (p2.x - p1.x);
				pb.y -= (2.5 * h / dist) * (p2.y - p1.y);

				pc.x = centre.x + (h / dist) * (p2.x - p1.x);
				pc.y = centre.y + (h / dist) * (p2.y - p1.y);

				/*		C
				       / \
					  /	  \
					 /	   \
					A-------B
					Direction triangle
				*/

				drawn[link_idx] = true;
				draw_list->AddTriangleFilled(pa, pb, pc, IM_COL32(150, 150, 150, 255));

				// Draw transition progress
				if (link_idx == activeLinkID)
				{
					float p = currTrans->mTimeElapsed / currTrans->mDuration;
					p2.x = p1.x + (p * dist / dist) * (p2.x - p1.x);
					p2.y = p1.y + (p * dist / dist) * (p2.y - p1.y);
					draw_list->AddLine(p1, p2, IM_COL32(0, 255, 0, 255), 2.0f);

					md_log("out! %f", p);
				}
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
			/*ImGui::SliderFloat("##value", &node->Value, 0.0f, 1.0f, "Alpha %.2f");
			ImGui::ColorEdit3("##color", &node->Color.x);*/
			auto anim = animVec->at(node->Name);
			float progress = anim->mTimeElapsed / anim->mDuration;
			ImGui::ProgressBar(progress, ImVec2(150, 10.f), "");
			ImGui::Text("Num of transition IN/OUT: %d", anim->mTransitions.size());
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
			{
				node->Pos = node->Pos + ImGui::GetIO().MouseDelta;
				ImGui::Config::BeginConfig(windowName);
				ImGui::Config::SaveVec2(node->Name, node->Pos, true);
				ImGui::Config::EndConfig();
				ImGui::Config::UpdateConfig();
			}
			
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
}