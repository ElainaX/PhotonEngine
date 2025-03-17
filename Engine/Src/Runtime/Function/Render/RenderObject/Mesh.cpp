#include "Mesh.h"

namespace photon 
{

	Mesh::Mesh(MeshDesc desc)
	{
		type = desc.type;
		vertexRawData = desc.vertexRawData;
		indexRawData = desc.indexRawData;
		name = desc.name;
	}

}