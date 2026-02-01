#pragma once
#include "Editor/Panels/IEditorPanel.h"
#include "Asset/AssetHandle.h"

namespace Ryu::Asset { class AssetRegistry; }

namespace Ryu::Editor
{
	class AssetsPanel : public IEditorPanel
	{
	public:
		static constexpr auto Name = "Assets";

		enum class FilterMode { All, Engine, Project };
		enum class AssetCategory { All, Meshes, Textures };  // TODO: Maybe move this category into the asset system?

	public:
		AssetsPanel(EditorApp* app, Asset::AssetRegistry* assetRegistry);
		void OnImGuiRender() override;
	
	private:
		void RenderToolbar();
		void RenderFolderTree();
		void RenderAssetList();
		bool PassesFilter(const std::string& displayName, bool isEngine) const;


	private:
		Asset::AssetRegistry* m_assetRegistry;
		FilterMode            m_filter;
		AssetCategory         m_category;
		f32                   m_thumbnailSize;
		Asset::AssetId        m_selectedAsset;
	};
}
