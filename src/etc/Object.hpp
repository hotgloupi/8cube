#ifndef  ETC_OBJECT_HPP
# define ETC_OBJECT_HPP

# include <memory>
# include <cstring>

# include <map>

# include <etc/types.hpp>

namespace etc {

	struct Object;

	namespace detail {

		// Fondamental types.
		enum class builtin
		{
			null_type = 0,
			int_type,
			float_type,
			string_type,
			array_type,
			dict_type,
		};

		// Object storage type.
		template<int type_id> struct storage;

		// Initialize an object from the type T.
		template<typename T> struct to_object;

		// Access to internal type and storage.
		struct access_object;
	}

	struct Object
	{
	protected:
		int type_id;
		void* storage;
		friend struct detail::access_object;

	public:
		Object();
		Object(Object const& other);
		Object(Object&& other);

		explicit Object(char const* value);
		template<typename T>
		explicit Object(T&& value)
			: type_id((int)detail::to_object<T>::type_id)
			, storage(nullptr)
		{
			typedef typename std::remove_cv<
				typename std::remove_reference<T>::type
			>::type type;
			detail::to_object<type>::init_storage(*this, std::forward<T>(value));
		}

		~Object();

	public:
		Object& operator =(Object const& other);
		Object& operator =(Object&& other);

		Object operator +(Object const& other) const;
		Object operator -(Object const& other) const;
		Object operator *(Object const& other) const;
		Object operator /(Object const& other) const;

		Object operator +=(Object const& other);
		Object operator -=(Object const& other);
		Object operator *=(Object const& other);
		Object operator /=(Object const& other);

		operator bool() const;
		bool operator !() const;
		bool operator ==(Object const& other) const;
		bool operator !=(Object const& other) const;
		bool operator <(Object const& other) const;
		bool operator >(Object const& other) const;
		bool operator <=(Object const& other) const;
		bool operator >=(Object const& other) const;

		Object& operator [](Object const& key);
		Object const& operator [](Object const& key) const;
	};

}

# include "Object.inl"

#endif
