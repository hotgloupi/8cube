#ifndef  CUBE_GL_RENDERER_VERTEXBUFFERATTRIBUTE_HPP
# define CUBE_GL_RENDERER_VERTEXBUFFERATTRIBUTE_HPP

# include "fwd.hpp"

# include <cube/gl/content_traits.hpp>

# include <etc/assert.hpp>
# include <etc/log/component.hpp>
# include <etc/memory.hpp>
# include <etc/types.hpp>

# include <functional>
# include <vector>

namespace cube { namespace gl { namespace renderer {

	namespace detail {

		template<typename T>
		struct extract_content_type
		{
			typedef typename content_traits<T>::component_type component_t;
			static ContentType const value = MakeContentType<component_t>::value;
		};

		template<typename T, typename Deleter>
		struct WrapDeleter
		{
			Deleter deleter;

			WrapDeleter(Deleter deleter)
				: deleter(deleter)
			{}

			void operator ()(void* ptr)
			{
				(this->deleter)((T*)ptr);
			}
		};
		typedef std::unique_ptr<void, std::function<void(void*)>> BufferPtr;
	}

	/**
	 * @brief Represent an attribute for a vertex buffer.
	 *
	 * A VertexBufferAttribute is used to construct a VertexBuffer. It stores
	 * all needed information to send to the rendering system a memory chunk
	 * of any ContentKind.
	 *
	 * When constructed with a unique_ptr, the memory is entirely managed, but
	 * all other constructors let you manage it yourself.
	 *
	 *
	 * @note This class is "unsafe" in some ways for performance reasons. @see
	 *       each constructor for warnings about memory management.
	 */
	class CUBE_API VertexBufferAttribute
	{
		ETC_LOG_COMPONENT("cube.gl.renderer.VertexBufferAttribute");
	public:
		ContentKind const           kind;
		ContentType const           type;
		ContentHint const           hint;
		etc::size_type const        arity;
		etc::size_type              nb_elements;

		// size = nb_elements * sizeof(type)
		size_t                      buffer_size;

	private:
		void*                       _buffer;
		std::function<void(void*)>  _deleter;

	public:
		void* buffer() const { return _buffer; }

	private:
		VertexBufferAttribute(ContentKind const kind,
		                      ContentType const type,
		                      ContentHint const hint,
		                      etc::size_type const arity,
		                      etc::size_type const nb_elements,
		                      size_t const buffer_size,
		                      void* buffer,
		                      std::function<void(void*)> deleter) ETC_NOEXCEPT;

	public:
		/**
		 * @brief Contruct an attribute.
		 *
		 * @note The buffer memory is managed.
		 * @note `nb_elements' is not the buffer size.
		 *
		 * @warning The memory is managed by the class, meaning that it won't
		 * be freed as long as the attribute is alive. If you need to save some
		 * memory, you need to call `reset()' by hand.
		 */
		template<typename T, typename Deleter>
		VertexBufferAttribute(ContentKind const kind,
		                      std::unique_ptr<T, Deleter>&& data,
		                      etc::size_type const nb_elements,
		                      ContentHint const hint = ContentHint::static_content)
			: VertexBufferAttribute(
				kind,
				detail::extract_content_type<T>::value,
				hint,
				content_traits<T>::arity,
				nb_elements,
				nb_elements * sizeof(T),
				data.get(),
				detail::WrapDeleter<T, Deleter>(data.get_deleter())
			)
		{data.release();}

		/**
		 * @brief Construct an attribute from a raw pointer.
		 *
		 * @note The buffer memory is *not* managed.
		 * @note `nb_elements' is not the buffer size.
		 *
		 * @warning You must ensure that the pointer is still valid when this
		 *          attribute is used (constructing a VertexBuffer).
		 *
		 * @warning The pointer is forgotten as soon as possible, you should
		 *          not rely on this class to keep a reference to it.
		 */
		template<typename T>
		VertexBufferAttribute(ContentKind const kind,
		                      T const* data,
		                      etc::size_type const nb_elements,
		                      ContentHint const hint = ContentHint::static_content)
			: VertexBufferAttribute{
				kind,
				detail::extract_content_type<T>::value,
				hint,
				content_traits<T>::arity,
				nb_elements,
				nb_elements * sizeof(T),
				const_cast<T*>(data), // The data won't be deleted
				nullptr
			}
		{}

		/**
		 * @brief Construct an attribute from a static array.
		 *
		 * @note The buffer memory is *not* managed.
		 *
		 * @warning You must ensure that the array is still valid when this
		 *          attribute is used (constructing a VertexBuffer).
		 *
		 * @warning The pointer is forgotten as soon as possible, you should
		 *          not rely on this class to keep a reference to it.
		 */
		template<typename T, etc::size_type nb_elements>
		VertexBufferAttribute(ContentKind const kind,
		                      T (&data)[nb_elements],
		                      ContentHint const hint = ContentHint::static_content)
			: VertexBufferAttribute{kind, data, nb_elements, hint}
		{}

		/**
		 * @brief Construct an attribute from a vector.
		 *
		 * @warning The memory is directly referenced from the vector content,
		 *          so the vector must stay alive until the attribute is used
		 */
		template<typename T>
		VertexBufferAttribute(ContentKind const kind,
		                      std::vector<T> const& data,
		                      ContentHint const hint = ContentHint::static_content)
			: VertexBufferAttribute(
				kind,
				&data[0],
				static_cast<etc::size_type>(data.size()),
				hint
			)
		{}

	public:
		template<typename T>
		void set(etc::size_type const index,
		         T const& value)
		{
			ETC_ASSERT_NEQ(_buffer, nullptr);
			ETC_ASSERT_EQ(this->arity, content_traits<T>::arity);
			ETC_ASSERT_LT(index, this->nb_elements);
			static_cast<T*>(_buffer)[index] = value;
		}

		template<typename T>
		T const& get(etc::size_type const index) const
		{
			ETC_ASSERT_NEQ(_buffer, nullptr);
			ETC_ASSERT_EQ(this->arity, content_traits<T>::arity);
			ETC_ASSERT_LT(index, this->nb_elements);
			return static_cast<T*>(_buffer)[index];
		}

		/**
		 * @brief Reset the buffer to a new value.
		 */
	public:
		template<typename T>
		void reset_buffer(T* data, etc::size_type const nb_elements)
		{
			if (_deleter)
			{
				_deleter(_buffer);
				_deleter = nullptr;
			}
			_buffer = data;
			this->nb_elements = nb_elements;
			this->buffer_size = nb_elements * sizeof(T);
		}

		template<typename T, etc::size_type nb_elements>
		void reset_buffer(T (&data)[nb_elements])
		{
			this->reset_buffer(data, nb_elements);
		}

		template<typename T, typename Deleter>
		void reset_buffer(std::unique_ptr<T, Deleter>&& data,
		                  etc::size_type const nb_elements)
		{
			this->reset_buffer(data.release(), nb_elements);
			_deleter = detail::WrapDeleter<T, Deleter>{data.get_deleter()};
		}

		~VertexBufferAttribute();
	};

	template<typename... Args>
	VertexBufferAttributePtr
	make_vertex_buffer_attribute(Args&&... args)
	{
		return std::make_shared<VertexBufferAttribute>(
			std::forward<Args>(args)...
		);
	}

	std::vector<VertexBufferAttributePtr>
	inline
	make_vertex_buffer_attributes()
	{
		return std::vector<VertexBufferAttributePtr>{};
	}

	template<typename... Args>
	std::vector<VertexBufferAttributePtr>
	make_vertex_buffer_attributes(VertexBufferAttributePtr&& attr, Args&&... args)
	{
		auto attributes = make_vertex_buffer_attributes(
				std::forward<Args>(args)...
		);
		attributes.emplace_back(std::move(attr));
		return attributes;
	}

}}}

#endif
