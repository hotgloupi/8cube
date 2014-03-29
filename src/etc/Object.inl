#ifndef  ETC_OBJECT_INL
# define ETC_OBJECT_INL

# include "Object.hpp"

namespace etc { namespace detail {

	struct access_object
	{
		static inline int& type_id(Object& o) { return o.type_id; }
		static inline int type_id(Object const& o) { return o.type_id; }
		static inline void*& storage(Object& o) { return o.storage; }
		static inline void* const& storage(Object const& o) { return o.storage; }
	};

	namespace store_method {

		template<typename Storage>
		struct none
		{
			static void release_storage(Object&) {}
		};

		template<typename Storage>
		struct inplace : public none<Storage>
		{
			static void init_storage(Object& object, Storage const& value)
			{ inplace::value(object) = value; }

			static Storage& value(Object& object)
			{ return  *(Storage*) &access_object::storage(object); }
			static Storage const& value(Object const& object)
			{ return  *(Storage const*) &access_object::storage(object); }
		};

		template<typename Storage>
		struct malloc {};

		template<typename Storage>
		struct new_
		{
			static void init_storage(Object& o, Storage const& value)
			{ *(std::string**) &access_object::storage(o) = new Storage(value); }

			static Storage& value(Object& object)
			{ return *(Storage*) access_object::storage(object); }
			static Storage const& value(Object const& object)
			{ return *(Storage const*) access_object::storage(object); }

			static void release_storage(Object& object)
			{ delete (Storage*) access_object::storage(object); }
		};

	}

	template<builtin b, typename Storage, template<class> class Method>
	struct make_storage
		: public Method<Storage>
	{
		static builtin const type_id = b;
		typedef Storage type;
	};

#define MAKE_STORAGE(__type_id, __storage, __method)                          \
	template<> struct storage<(int) builtin::__type_id>                   \
		: public make_storage<                                            \
			  builtin::__type_id                                          \
			, __storage                                                   \
			, store_method::__method                                      \
		  >                                                               \
	{};                                                                   \
/**/

	MAKE_STORAGE(null_type, void, none);
	MAKE_STORAGE(int_type, int64_t, inplace);
	MAKE_STORAGE(float_type, double, inplace);
	MAKE_STORAGE(string_type, std::string, new_);
#define IDENTITY(...) __VA_ARGS__
	MAKE_STORAGE(dict_type, IDENTITY(std::map<Object, Object>), new_);
#undef MAKE_STORAGE

#define CUBE_OBJECT_CONVERTER(__type, __type_id) \
	template<> struct to_object<__type> : storage<(int) __type_id> {}; \
/**/

	CUBE_OBJECT_CONVERTER(void, builtin::null_type);
	CUBE_OBJECT_CONVERTER(int16_t, builtin::int_type);
	CUBE_OBJECT_CONVERTER(int32_t, builtin::int_type);
	CUBE_OBJECT_CONVERTER(int64_t, builtin::int_type);
	CUBE_OBJECT_CONVERTER(float, builtin::float_type);
	CUBE_OBJECT_CONVERTER(double, builtin::float_type);
	CUBE_OBJECT_CONVERTER(std::string, builtin::string_type);

# define CUBE_OBJECT_MAKE_CONVERTER(__type, __type_id) \
	template<> struct to_object<__type> : storage<(int) __type_id> { \
		static void init_storage(Object&, __type); \
	}; \
	void to_object<__type>::init_storage(Object& object, __type value)\
/**/

}} // !etc::detail

#endif
