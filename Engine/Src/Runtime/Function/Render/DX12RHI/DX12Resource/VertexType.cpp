#include "VertexType.h"

namespace photon 
{
	VertexType VertexSimple::vertexType = VertexType::VertexSimple;
	VertexLayout VertexSimple::layout = BuildAndReturnLayout();
}