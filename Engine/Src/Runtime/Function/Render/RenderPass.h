#pragma once



namespace photon 
{
	class RenderResourceData;

	class RenderPass
	{
	public:
		virtual void Draw() = 0;
	};
}