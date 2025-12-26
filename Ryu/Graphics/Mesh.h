#pragma once
#include "Graphics/Core/GfxBuffer.h"

namespace Ryu::Gfx
{
	class CommandList;

	class Mesh
	{
	public:
		struct DrawInfo
		{
			D3D_PRIMITIVE_TOPOLOGY Topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			u32 VertexCountPerInstance      = 0;
			u32 IndexCountPerInstance       = 0;
			u32 InstanceCount               = 0;
			u32 StartVertexLocation         = 0;
			u32 StartIndexLocation          = 0;
			i32 BaseVertexLocation          = 0;
			u32 StartInstanceLocation       = 0;
		};

	public:
		Mesh() = default;
		Mesh(Device* device, const Buffer::Desc& vbDesc, const Buffer::Desc* ibDesc);

		[[nodiscard]] inline bool HasIndexBuffer() const         { return m_indexBuffer != nullptr; }
		[[nodiscard]] inline Buffer* GetVertexBuffer() const     { return m_vertexBuffer.get();     }
		[[nodiscard]] inline Buffer* GetIndexBuffer() const      { return m_indexBuffer.get();      }
		[[nodiscard]] inline const DrawInfo& GetDrawInfo() const { return m_drawInfo;               }
		[[nodiscard]] inline DrawInfo& GetDrawInfo()             { return m_drawInfo;               }
		inline void SetDrawInfo(const DrawInfo& info)            { m_drawInfo = info;               }

		void UploadBufferData(const CommandList& cmdList, void* vbData, void* ibData);
		void SetPipelineBuffers(const CommandList& cmdList, u32 slot);

	private:
		DrawInfo m_drawInfo;
		std::unique_ptr<Buffer> m_vertexBuffer;
		std::unique_ptr<Buffer> m_indexBuffer;
	};
}
