#include "Graphics/Mesh.h"
#include "Graphics/Core/GfxCommandList.h"

namespace Ryu::Gfx
{
	Mesh::Mesh(Device* device, const Buffer::Desc& vbDesc, const Buffer::Desc* ibDesc)
	{
		m_vertexBuffer = std::make_unique<Buffer>(device, vbDesc);
		if (ibDesc)
		{
			m_indexBuffer = std::make_unique<Buffer>(device, *ibDesc);
		}
	}
	
	void Mesh::UploadBufferData(const CommandList& cmdList, void* vbData, void* ibData)
	{
		if (m_vertexBuffer && vbData)
		{
			if (m_vertexBuffer->NeedsUpload())
			{
				m_vertexBuffer->UploadData(cmdList, vbData);
			}
		}

		if (m_indexBuffer && ibData)
		{
			if (m_indexBuffer->NeedsUpload())
			{
				m_indexBuffer->UploadData(cmdList, ibData);
			}
		}
	}
	
	void Mesh::SetPipelineBuffers(const CommandList& cmdList, u32 slot)
	{
		cmdList.SetTopology(m_drawInfo.Topology);

		if (m_vertexBuffer)
		{
			cmdList.SetVertexBuffer(slot, *m_vertexBuffer);
		}

		if (m_indexBuffer)
		{
			cmdList.SetIndexBuffer(*m_indexBuffer);
		}
	}
}
