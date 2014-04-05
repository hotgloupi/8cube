#include "Object.hpp"

#include <etc/to_string.hpp>
#include <etc/exception.hpp>
#include <etc/test.hpp>

#include <map>
#include <typeinfo>

namespace etc {

	using exception::Exception;

	namespace {

		void dynamic_copy(Object& self, Object const& other)
		{
			ETC_ASSERT_EQ(
				detail::access_object::type_id(self),
				detail::access_object::type_id(other)
			);

			static const int int_type = (int)detail::builtin::int_type;
			static const int float_type = (int)detail::builtin::float_type;
			static const int string_type = (int)detail::builtin::string_type;
			switch (detail::access_object::type_id(self))
			{
			case (int) detail::builtin::null_type:
				break;
#define BUILTIN_INIT(__type_id) \
			case __type_id: \
				detail::storage<__type_id>::init_storage( \
					self, \
					detail::storage<__type_id>::value(other) \
				); \
				break; \
/**/

				BUILTIN_INIT(int_type);
				BUILTIN_INIT(float_type);
				BUILTIN_INIT(string_type);
				// XXX array_type
				// XXX dict_type
			default:
				// XXX provide a type registry
				ETC_ERROR(
					etc::to_string("Unhandled type", detail::access_object::type_id(self))
				);
			}

		}

		void dynamic_release(Object& self)
		{
			switch (detail::access_object::type_id(self))
			{
#define BUILTIN_RELEASE(__type_id) \
			case (int) detail::builtin::__type_id: \
				detail::storage< \
					(int) detail::builtin::__type_id \
				>::release_storage(self); \
				break; \
	/**/

				BUILTIN_RELEASE(null_type);
				BUILTIN_RELEASE(int_type);
				BUILTIN_RELEASE(float_type);
				BUILTIN_RELEASE(string_type);
				// XXX array_type
				// XXX dict_type
			default:
				// XXX provide a type registry
				ETC_ERROR(
					etc::to_string("Unhandled type", detail::access_object::type_id(self))
				);
			}
		}

	} // !anonymous

	Object::Object()
		: type_id((int) detail::builtin::null_type)
	{}

	Object::Object(Object const& other)
		: type_id(other.type_id)
	{ dynamic_copy(*this, other); }

	Object::Object(Object&& other)
		: type_id{other.type_id}
		, storage{other.storage}
	{ other.storage = nullptr; }

	Object::Object(char const* value)
		: type_id{(int) detail::builtin::string_type}
		, storage{nullptr}
	{
		if (value == nullptr)
			type_id = (int) detail::builtin::null_type;
		else
			detail::to_object<std::string>::init_storage(*this, value);
	}

	Object::~Object()
	{ dynamic_release(*this); }

	namespace {

		ETC_TEST_CASE(ctor)
		{
			Object();
			Object(12);
			Object(12.0f);
			Object((char const*)"LOL");
			Object("LOL");
			//Object(std::map<int, int>{});
		}

	}
}
