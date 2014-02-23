#ifndef  CUBE_RESOURCE_RESOURCE_HPP
# define CUBE_RESOURCE_RESOURCE_HPP

# include "fwd.hpp"

# include <cube/api.hpp>
# include <cube/exception.hpp>

# include <etc/compiler.hpp>

namespace cube { namespace resource {

	typedef unsigned int id_type;

	/**
	 * @brief Base class for all resource kind.
	 */
	class CUBE_API Resource
		: public std::enable_shared_from_this<Resource>
	{
	private:
		// _id and _manager are set whenever a resource is managed.
		id_type     _id;
		Manager*    _manager;

	public:
		/// Non managed resource.
		inline
		Resource() ETC_NOEXCEPT
			: _id{0}
			, _manager{nullptr}
		{}

		virtual
		~Resource();

		/// id of a managed resource or 0.
		inline id_type id() const ETC_NOEXCEPT
		{ return _id; }

		/// True when managed.
		inline bool managed() const ETC_NOEXCEPT
		{ return _id != 0; }

	// Pretty ways to get the underlying pointer.
	private:
		enum PtrKind { shared_ptr_only, some_ptr };
		template<PtrKind kind>
		struct cast_to
		{
			Resource* _resource;
			cast_to(Resource* resource) : _resource{resource} {}
			template<typename T>
			operator std::shared_ptr<T>() const
			{
				try
				{ return std::dynamic_pointer_cast<T>(_resource->shared_from_this()); }
				catch (std::bad_weak_ptr const&) {}
				if (kind == shared_ptr_only)
					throw exception::Exception{"Not shared"};
				return std::shared_ptr<T>{
					static_cast<T*>(_resource),
					[] (T*) {}
				};
			}
		};
	public:
		/// Build a shared_ptr or throw if the resource is not associated to
		/// a shared pointer.
		cast_to<shared_ptr_only> shared_ptr()
		{ return cast_to<shared_ptr_only>(this); }

		/// Build a shared_ptr whether or not the resource was associated to
		/// a shared pointer. In the latter case, you are responsible to keep
		/// this instance alive while any ptr instance is in use.
		cast_to<some_ptr> ptr()
		{ return cast_to<some_ptr>(this); }

	private:
		id_type manage(Manager& manager,
		               id_type const id);
		void manage();

		friend Manager;
	};

}}

#endif

