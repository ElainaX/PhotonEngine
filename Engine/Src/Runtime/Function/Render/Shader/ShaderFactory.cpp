#include "ShaderFactory.h"

namespace photon
{
	ShaderProgramLoadDesc ShaderFactory::BuildPbrShaderDesc()
	{
        ShaderProgramLoadDesc desc = ShaderProgramLoadDesc::GraphicsDefault();
        desc.enableDebugInfo = true;
        desc.parameters =
        {
            { "ObjectCB",    ShaderParameterType::Cbv,     ShaderParameterScope::Object,     0, 0, 1 },
            { "PassCB",      ShaderParameterType::Cbv,     ShaderParameterScope::Pass,   1, 0, 1 },
            { "MaterialCB",  ShaderParameterType::Cbv,     ShaderParameterScope::Material, 2, 0, 1 },
            { "BaseColorTex",ShaderParameterType::Srv,     ShaderParameterScope::Material, 0, 0, 1 },
            { "NormalTex",   ShaderParameterType::Srv,     ShaderParameterScope::Material, 1, 0, 1 },
            { "RoughnessTex",   ShaderParameterType::Srv,     ShaderParameterScope::Material, 2, 0, 1 },
            { "DepthStencilTextures",   ShaderParameterType::Srv,     ShaderParameterScope::Pass, 3, 0, 1 },
        };

        desc.constantBuffers =
        {
            { "ObjectCB",     0, 0, sizeof(ObjectData),        ShaderParameterScope::Object     },
            { "PassCB",   1, 0, sizeof(PassData),      ShaderParameterScope::Pass   },
            { "MaterialCB", 2, 0, sizeof(PbrMaterialData), ShaderParameterScope::Material },
        };

        return desc;
	}
}
