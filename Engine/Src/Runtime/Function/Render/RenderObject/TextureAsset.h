#pragma once
#include "AssetMeta.h"
#include "Function/Render/Resource/TextureCpuData.h"

namespace photon
{


	struct TextureAsset
	{
		AssetMeta meta;

		TextureImportSettings importSettings;
		TextureCpuData cpuData;

		bool cpuDataResident = true;

		bool HasCpuData() const
		{
			return cpuDataResident && !cpuData.Empty();
		}

		void ReleaseCpuData()
		{
			cpuData.Clear();
			cpuDataResident = false;
		}
	};
}
