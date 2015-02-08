#ifndef  CUBE_GL_RENDERER_VERTEXBUFFER_HPP
# define CUBE_GL_RENDERER_VERTEXBUFFER_HPP

# include "Exception.hpp"

# include "constants.hpp"
# include "Bindable.hpp"
# include "VertexBufferAttribute.hpp"
# include "../content_traits.hpp"

# include <cube/resource/Resource.hpp>

# include <cstdlib>
# include <memory>
# include <vector>

namespace cube { namespace gl { namespace renderer {

	class CUBE_API VertexBuffer
		: public Bindable
		, public resource::Resource
	{
	public:
		typedef VertexBufferAttribute       Attribute;
		typedef VertexBufferAttributePtr    AttributePtr;
		typedef std::vector<AttributePtr>   AttributeList;

	/**************************************************************************
	 * Abstract behaviors.
	 */
	protected:
		AttributeList _attributes;

	public:
		AttributeList const& attributes() const
		{ return _attributes; }

	public:
		VertexBuffer(AttributeList&& attributes)
			: _attributes{std::move(attributes)}
		{}

		virtual
		~VertexBuffer()
		{}


		void reload(etc::size_type const idx)
		{
			if (idx >= _attributes.size())
				throw Exception(
					etc::to_string(idx, ">=", _attributes.size())
				);
			_reload(idx);
		}

	protected:
		virtual void _reload(etc::size_type const idx) = 0;
		friend struct Bindable::InternalGuard<VertexBuffer>;

	//	template<typename T>
	//	inline void
	//	push_static_content(ContentKind kind,
	//	                    T const* data,
	//	                    unsigned int nb_elements,
	//	                    bool managed = false)
	//	{
	//		typedef typename content_traits<T>::component_type component_t;

	//		this->_push_content(
	//			kind,
	//			MakeContentType<component_t>::value,
	//			ContentHint::static_content,
	//			content_traits<T>::arity,
	//			nb_elements,
	//			data,
	//			nb_elements * sizeof (T),
	//			managed
	//		);
	//	}

	//	template<typename T>
	//	inline void
	//	push_static_content(ContentKind kind,
	//	                    std::unique_ptr<T, void(*)(void*)>&& data,
	//	                    unsigned int nb_elements)
	//	{
	//		this->push_static_content(kind, data.get(), nb_elements, true);
	//		data.release();
	//	}

	//protected:
	//	inline void
	//	_push_content(ContentKind kind,
	//	              ContentType type,
	//	              ContentHint hint,
	//	              unsigned int nb_elements,
	//	              unsigned int arity,
	//	              void const* ptr,
	//	              size_t size,
	//	              bool managed)
	//	{
	//		Attribute attr = {
	//			kind,
	//			type,
	//			hint,
	//			nb_elements,
	//			arity,
	//			ptr,
	//			size,
	//			managed,
	//		};
	//		_attributes.push_back(attr);
	//	}

	};

//	namespace detail {
//
//		/**
//		 * Interleaved content pack together types.
//		 * Usage:
//		 *  typedef InterleavedContent<
//		 *      ContentKind, std::array<Vector3f, 10>,
//		 *      ContentKind, std::array<Vector3f, 10>,
//		 *      ContentKind, std::array<Color3f, 10>,
//		 *  > content_t;
//		 *  make_interleaved_content(
//		 *      ContentKind, std::array<Vector3f, 10>,
//		 *      ContentKind, std::array<Vector3f, 10>,
//		 *      ContentKind, std::array<Color3f, 10>,
//		 */
//
//		template<typename... T>
//		struct InterleavedContent;
//
//		// end recursion here
//		template<>
//		struct InterleavedContent<>
//		{
//			InterleavedContent(size_t index) {}
//		};
//
//		// front is an array
//		template<typename _DataPtr, typename... _Tail>
//		struct InterleavedContent<_DataPtr, size_t, _Tail...>
//		{
//		public:
//			typedef typename remove_cv<
//				typename remove_pointer<
//					typename std::remove_cv<
//						typename std::remove_reference<_DataPtr>::type
//					>::type
//				>::type
//			>::type element_t;
//
//			typedef InterleavedContent<_Tail...> tail_t;
//
//		public:
//			element_t   value;
//			tail_t      tail;
//
//			static_assert(
//				sizeof(Tail) == 0 || Tail::array_size == array_size,
//				"Interleaved content can't pack arrays with different sizes"
//			);
//
//		public:
//			InterleavedContent(size_t index, _DataPtr front, size_t sz, _Tail... values)
//				: value(front[index])
//				, tail(index, values)
//			{}
//		};
//
//		// front is a content kind
//		template<typename... _Tail>
//		struct InterleavedContent<ContentKind, _Tail...>
//			: InterleavedContent<_Tail...>
//		{
//			static_assert(
//				sizeof... (_Tail) != 0,
//				"A content kind must be followed by data"
//			);
//
//			typedef InterleavedContent<_Tail...> Tail;
//		public:
//
//			InterleavedContent(size_t index, ContentKind kind, _Tail... values)
//				: InterleavedContent<_Tail...>(index, values...)
//			{}
//
//			struct IndicesDescription
//			{
//				void const* ptr;
//				size_t      nb_elements;
//				ContentType content_type;
//			};
//
//			/**
//			 * Find indices infos (pointer, data type and array size).
//			 * note: the number of bytes is sizeof(data_type) * array_size.
//			 */
//			template<typename __Front, typename __Tail>
//			inline IndicesDescription
//				get_indices(ContentKind kind,
//			                __Front ptr,
//			                size_t size,
//			                __Tail... values)
//				{
//					if (kind == ContentKind::index)
//						return IndicesDescription{
//							ptr,
//							size
//							MakeContentType<Tail::element_t>::value,
//						};
//					return get_indices(values...);
//				}
//
//			inline IndicesDescription
//				get_indices()
//				{
//					assert(false && "no indices array!");
//					throw false;
//				}
//		};
//
//	} // !detail

//		/**
//		 * push_interleaved(ContentKind kind1, T const* data1, size_t N, [kind, data, N]...)
//		 * data1 is a pointer to VectorXX, ColorXX, etc.
//		 */
//		template<typename... T>
//		void push_interleaved_content(T... values)
//		{
//			typedef detail::InterleavedContent<T...> InterleavedContent;
//
//			auto indices = InterleavedContent::get_indices();
//			void* data = indices.
//			return this->_push_interleaved_content(values...)
//		}
//	private:
//		template<typename T, size_t N, typename... Tail>
//		void _push_interleaved_content(ContentKind kind,
//		                               std::array<T, N> const& data,
//		                               Tail... values)
//		{
//			return this->_push_interleaved_content(values...)
//		}
//	};

}}} // !cube::gl::renderer

#endif
