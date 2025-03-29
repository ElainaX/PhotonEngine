#pragma once
#include "Resource/Texture/Texture2D.h"
#include "Resource/Texture/Buffer.h"
#include "DirectXTK/WICTextureLoader12.h"
#include "RHI.h"
#include <filesystem>
#include <utility>

namespace photon 
{
	class ResourceLoader
	{
	public:
		ResourceLoader(RHI* rhi)
			: m_Rhi(rhi){}

		std::pair<std::shared_ptr<Buffer>, std::shared_ptr<Texture2D>> LoadTexture(const std::filesystem::path& filepath);

		RHI* m_Rhi;
	};


}