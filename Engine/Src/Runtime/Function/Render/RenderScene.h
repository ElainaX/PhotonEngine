#pragma once

#include "RenderCamera.h"
#include "Light.h"
#include "RenderObject/RenderItem.h"
#include "RenderObject/Model.h"
#include "RenderObject/RenderMeshCollection.h"

#include <memory>
#include <unordered_map>

namespace photon 
{

	class RenderScene
	{
	public:
		RenderScene(std::shared_ptr<RenderCamera> mainCamera)
		{
			Initialize(mainCamera);
		}

		void Initialize(std::shared_ptr<RenderCamera> mainCamera);

		CommonRenderItem* AddCommonRenderItem(std::shared_ptr<Mesh> mesh, Material* mat, Shader* shader, RenderLayer renderLayer, StaticFrameResourceEditor frameResourceEditor);
		void AddModel(std::shared_ptr<Model> model, Shader* shader, RenderLayer renderLayer, StaticFrameResourceEditor frameResourceEditor);
		DirLight* AddDirectionalLight(Vector3 strength, Vector3 dir);
		PointLight* AddPointLight(Vector3 strength, Vector3 position, float falloffStart, float falloffEnd);
		SpotLight* AddSpotLight(Vector3 strength, Vector3 position, Vector3 dir, float falloffStart, float falloffEnd, float spotPower);

		void SetMainRenderCamera(RenderCamera* mainCam);
		RenderCamera* AddRenderCamera(std::shared_ptr<RenderCamera> camera, bool bSetMainCamera = false);
		void RemoveRenderCamera(RenderCamera* camera);

		RenderCamera* GetMainCamera();
		std::vector<CommonRenderItem*> GetCommonRenderItems(RHI* rhi, bool needModelRitems = false);
		StaticFrameResourceEditor* GetCommonRItemFrameResourceEditor(uint64_t gameObjectId);

		std::unordered_map<std::shared_ptr<Model>, std::vector<std::shared_ptr<CommonRenderItem>>>& GetModelRenderItems();


		std::vector<DirLight> directionalLights;
		std::vector<PointLight> pointLights;
		std::vector<SpotLight> spotLights;


	private:
		std::shared_ptr<CommonRenderItem> CreateCommonRenderItem(std::shared_ptr<Mesh> mesh, Material* mat, Shader* shader, RenderLayer renderLayer, StaticFrameResourceEditor frameResourceEditor);


		std::unordered_map<uint64_t, StaticFrameResourceEditor> m_StaticFrameResourceEditors;

		std::shared_ptr<RenderMeshCollection> m_MeshCollection;
		std::vector<std::shared_ptr<CommonRenderItem>> m_RenderItems;
		std::unordered_map<std::shared_ptr<Model>, std::vector<std::shared_ptr<CommonRenderItem>>> m_ModelRenderItems;
		std::vector<std::shared_ptr<RenderCamera>> m_Cameras;

	};


}