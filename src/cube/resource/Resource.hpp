#ifndef  CUBE_RESOURCE_RESOURCE_HPP
# define CUBE_RESOURCE_RESOURCE_HPP

# include "fwd.hpp"

# include <cube/api.hpp>

# include <memory>

namespace cube { namespace resource {

	typedef unsigned int id_type;

	/**
	 * @brief Base class for all resource kind.
	 */
	class CUBE_API Resource
		: public std::enable_shared_from_this<Resource>
	{
	private:
		id_type     _id;
		Manager*    _manager;

	public:
		virtual
		~Resource();

		inline
		id_type id() const noexcept
		{ return _id; }

		inline
		bool managed() const noexcept
		{ return _id != 0; }

	protected:
		inline
		Resource() noexcept
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

