#pragma once


namespace photon 
{
	class RHI;
	class RenderResourceData;

	class RenderPass
	{
	public:
		//virtual void Initialize(RHI* rhi) = 0;
		virtual void PrepareContext(RenderResourceData* data) = 0;
		virtual void Draw() = 0;

	protected:
		RHI* m_Rhi;
	};
}