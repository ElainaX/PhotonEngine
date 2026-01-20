#pragma once


namespace photon 
{
	class RHI;
	class RenderResourceData;
	class EG_FrameContext;
	class PassBlackboard;

	class RenderPass
	{
	public:
		//virtual void Initialize(RHI* rhi) = 0;
		virtual void PrepareContext(RenderResourceData* frame) = 0;
		virtual void Draw(EG_FrameContext* frame, PassBlackboard* bb) = 0;

	protected:
		RHI* m_Rhi;
	};
}