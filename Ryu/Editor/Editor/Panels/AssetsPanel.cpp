#include "Editor/Panels/AssetsPanel.h"
#include "Asset/AssetRegistry.h"
#include <ImGui/imgui.h>

namespace Ryu::Editor
{
	namespace
	{
		static std::array<char, 256> s_searchBuffer{};
		static bool s_isSearching = false;
	}

	AssetsPanel::AssetsPanel(EditorApp* app, Asset::AssetRegistry* assetRegistry)
		: IEditorPanel(app)
		, m_assetRegistry(assetRegistry)
		, m_filter(FilterMode::All)
		, m_category(AssetCategory::All)
		, m_thumbnailSize(80.0f)
		, m_selectedAsset(Asset::INVALID_ASSET_ID)
	{
	}
	
	void AssetsPanel::OnImGuiRender()
	{
		if (!IsOpen)
		{
			return;
		}

		if (ImGui::Begin("Assets##AssetsPanel", &IsOpen, ImGuiWindowFlags_MenuBar))
		{
			RenderToolbar();

			static f32 treeWidth = 150.0f;

			// Left pane - folder tree
			ImGui::BeginChild("FolderTree##AssetsPanel", ImVec2(treeWidth, 0), ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeX);
			RenderFolderTree();
			ImGui::EndChild();

			ImGui::SameLine();

			// Right pane - asset grid
			ImGui::BeginChild("AssetGrid##AssetsPanel", ImVec2(0, 0), ImGuiChildFlags_Borders);
			RenderAssetList();
			ImGui::EndChild();
		}
		ImGui::End();
	}
	
	void AssetsPanel::RenderToolbar()
	{
		if (ImGui::BeginMenuBar())
		{
			ImGui::TextUnformatted("Filter:");
			ImGui::SameLine();

			auto FilterButton = [this](const char* label, FilterMode mode)
			{
				bool selected = (m_filter == mode);
				if (selected) 
				{
					ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
				}
				
				if (ImGui::SmallButton(label)) 
				{
					m_filter = mode;
				}
				
				if (selected) 
				{
					ImGui::PopStyleColor();
				}
			};

			FilterButton("All", FilterMode::All);
			ImGui::SameLine();
			FilterButton("Engine", FilterMode::Engine);
			ImGui::SameLine();
			FilterButton("Project", FilterMode::Project);

			ImGui::SameLine();
			ImGui::Separator();
			ImGui::SameLine();

			ImGui::SetNextItemWidth(200.0f);
			ImGui::InputTextWithHint("##Search", "Search assets...", s_searchBuffer.data(), s_searchBuffer.size());

			ImGui::EndMenuBar();
		}
	}
	
	void AssetsPanel::RenderFolderTree()
	{
		ImGuiTreeNodeFlags baseFlags = ImGuiTreeNodeFlags_OpenOnArrow
			| ImGuiTreeNodeFlags_OpenOnDoubleClick
			| ImGuiTreeNodeFlags_SpanAvailWidth
			| ImGuiTreeNodeFlags_Leaf
			| ImGuiTreeNodeFlags_NoTreePushOnOpen;

		auto CategoryNode = [this, baseFlags](const char* label, AssetCategory cat)
		{
			ImGuiTreeNodeFlags flags = baseFlags;
			if (m_category == cat) 
			{
				flags |= ImGuiTreeNodeFlags_Selected;
			}

			ImGui::TreeNodeEx(label, flags);
			if (ImGui::IsItemClicked())
			{
				m_category = cat;
			}
		};

		CategoryNode("All Assets##AssetsPanel", AssetCategory::All);
		ImGui::Separator();
		CategoryNode("Meshes##AssetsPanel", AssetCategory::Meshes);
		CategoryNode("Textures##AssetsPanel", AssetCategory::Textures);
	}

	void AssetsPanel::RenderAssetList()
	{
		constexpr ImGuiTableFlags tableFlags = ImGuiTableFlags_Resizable
			| ImGuiTableFlags_RowBg
			| ImGuiTableFlags_ScrollY
			| ImGuiTableFlags_Sortable
			| ImGuiTableFlags_BordersInnerV;

		if (!ImGui::BeginTable("AssetTable", 4, tableFlags))
		{
			return;
		}

		ImGui::TableSetupScrollFreeze(0, 1);
		ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
		ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 80.0f);
		ImGui::TableSetupColumn("State", ImGuiTableColumnFlags_WidthFixed, 70.0f);
		ImGui::TableSetupColumn("Source", ImGuiTableColumnFlags_WidthFixed, 60.0f);
		ImGui::TableHeadersRow();

		// Render mesh entries
		if (m_category == AssetCategory::All || m_category == AssetCategory::Meshes)
		{
			m_assetRegistry->Meshes().ForEach([this](Asset::MeshHandle handle, const Asset::MeshCache::Entry& entry)
			{
				bool isEngine = entry.IsProcedural;
				if (!PassesFilter(entry.Name, isEngine)) return;

				ImGui::TableNextRow();
				ImGui::PushID(static_cast<int>(handle.Id));

				// Name column
				ImGui::TableNextColumn();
				bool isSelected = (m_selectedAsset == handle.Id);
				if (ImGui::Selectable(entry.Name.c_str(), isSelected, ImGuiSelectableFlags_SpanAllColumns))
				{
					m_selectedAsset = handle.Id;
				}

				// Drag source
				if (ImGui::BeginDragDropSource())
				{
					ImGui::SetDragDropPayload("Mesh", &handle.Id, sizeof(Asset::AssetId));
					ImGui::Text("Mesh: %s", entry.Name.c_str());
					ImGui::EndDragDropSource();
				}

				// Context menu
				if (ImGui::BeginPopupContextItem())
				{
					ImGui::TextDisabled("%s", entry.Name.c_str());
					ImGui::Separator();
					if (ImGui::MenuItem("Copy Handle ID"))
					{
						char buf[32];
						snprintf(buf, sizeof(buf), "%llu", static_cast<unsigned long long>(handle.Id));
						ImGui::SetClipboardText(buf);
					}
					if (!isEngine && ImGui::MenuItem("Delete")) { /* TODO */ }
					ImGui::EndPopup();
				}

				// Type column
				ImGui::TableNextColumn();
				ImGui::TextUnformatted("Mesh");

				// State column
				ImGui::TableNextColumn();
				ImGui::TextUnformatted(EnumToString(entry.State).data());

				// Source column
				ImGui::TableNextColumn();
				if (isEngine)
					ImGui::TextColored(ImVec4(0.6f, 0.8f, 1.0f, 1.0f), "Engine");
				else
					ImGui::TextUnformatted("Project");

				ImGui::PopID();
			});
		}

		// Render texture entries
		if (m_category == AssetCategory::All || m_category == AssetCategory::Textures)
		{
			m_assetRegistry->Textures().ForEach([this](Asset::TextureHandle handle, const Asset::TextureCache::Entry& entry)
			{
				bool isEngine = entry.IsProcedural;
				if (!PassesFilter(entry.Name, isEngine)) return;

				ImGui::TableNextRow();
				ImGui::PushID(static_cast<int>(handle.Id));

				// Name column
				ImGui::TableNextColumn();
				bool isSelected = (m_selectedAsset == handle.Id);
				if (ImGui::Selectable(entry.Name.c_str(), isSelected, ImGuiSelectableFlags_SpanAllColumns))
				{
					m_selectedAsset = handle.Id;
				}

				// Drag source
				if (ImGui::BeginDragDropSource())
				{
					ImGui::SetDragDropPayload("Texture", &handle.Id, sizeof(Asset::AssetId));
					ImGui::Text("Texture: %s", entry.Name.c_str());
					ImGui::EndDragDropSource();
				}

				// Context menu
				if (ImGui::BeginPopupContextItem())
				{
					ImGui::TextDisabled("%s", entry.Name.c_str());
					ImGui::Separator();
					if (ImGui::MenuItem("Copy Handle ID"))
					{
						char buf[32];
						snprintf(buf, sizeof(buf), "%llu", static_cast<unsigned long long>(handle.Id));
						ImGui::SetClipboardText(buf);
					}
					if (!isEngine && ImGui::MenuItem("Delete"))
					{
						// TODO: Delete the asset (prevent deletion of engine assets)
					}
					ImGui::EndPopup();
				}

				// Type column
				ImGui::TableNextColumn();
				ImGui::TextUnformatted("Texture");

				// State column
				ImGui::TableNextColumn();
				ImGui::TextUnformatted(EnumToString(entry.State).data());

				// Source column
				ImGui::TableNextColumn();
				if (isEngine)
				{
					ImGui::TextColored(ImVec4(0.6f, 0.8f, 1.0f, 1.0f), "Engine");
				}
				else
				{
					ImGui::TextUnformatted("Project");
				}

				ImGui::PopID();
			});
		}

		ImGui::EndTable();

		// Deselect on empty click
		if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsAnyItemHovered())
		{
			m_selectedAsset = Asset::INVALID_ASSET_ID;
		}
	}
	
	bool AssetsPanel::PassesFilter(const std::string& displayName, bool isEngine) const
	{
		if ((m_filter == FilterMode::Engine && !isEngine) 
			|| (m_filter == FilterMode::Project && isEngine))
		{
			return false;
		}
		
		// Search filter — use strlen to get actual string length
		const char* searchCStr = s_searchBuffer.data();
		if (searchCStr[0] != '\0')
		{
			std::string nameLower = displayName;
			std::string searchLower(searchCStr);  // Stops at null terminator

			std::transform(nameLower.begin(), nameLower.end(), nameLower.begin(), ::tolower);
			std::transform(searchLower.begin(), searchLower.end(), searchLower.begin(), ::tolower);

			if (nameLower.find(searchLower) == std::string::npos)
				return false;
		}

		return true;
	}
}
