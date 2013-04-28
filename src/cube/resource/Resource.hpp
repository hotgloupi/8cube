#ifndef  CUBE_RESOURCE_RESOURCE_HPP
# define CUBE_RESOURCE_RESOURCE_HPP

# include "fwd.hpp"

# include <memory>

namespace cube { namespace resource {

	typedef unsigned int id_type;

	/**
	 * @brief Base class for all resource kind.
	 */
	class Resource
		: public std::enable_shared_from_this<Resource>
	{
	private:
		id_type     _id;
		Manager*    _manager;

	public:
		virtual
		~Resource();

		id_type id() const
		{ return _id; }

		bool managed() const
		{ return _id != 0; }

	protected:
		Resource()
			: _id{0}
		{}

	private:
		void manage(Manager& manager,
		            id_type const id);
		void manage();

		friend Manager;
	};

}}

#endif

