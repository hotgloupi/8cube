#include "assimp_importer.hpp"

#include <boost/thread/tss.hpp>

namespace cube { namespace scene { namespace detail {

	Assimp::Importer& assimp_importer()
	{
		static boost::thread_specific_ptr<Assimp::Importer> ptr;
		if (ptr.get() == nullptr)
			ptr.reset(new Assimp::Importer);
		return *ptr;
	}

}}}
