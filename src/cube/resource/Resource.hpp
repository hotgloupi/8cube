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
		id_type _id;

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
		void id(id_type const id)
		{ _id = id; }

		friend Manager;
	};

}}

#endif

