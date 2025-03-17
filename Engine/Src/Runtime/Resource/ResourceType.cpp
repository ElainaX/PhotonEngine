#include "ResourceType.h"


namespace photon 
{
	UINT64 Resource::s_Guid = 0;

	Resource::Resource()
	{
		guid = ++s_Guid;
	}

}