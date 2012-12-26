#include "exports.hpp"

#include "../renderer/VertexBuffer.hpp"
#include "../renderer/VertexBufferAttribute.hpp"


		///////////////////////////////////////////////////////////////////////
		// VertexBuffer
/*
		struct VertexBuffer
		{
			template<typename T>
			static
			void
			_push_static_content(renderer::VertexBuffer& self,
			                     ContentKind kind,
			                     boost::python::list list,
			                     size_t len)
			{
				std::unique_ptr<T, void(*)(void*)> array{
					static_cast<T*>(::malloc(len * sizeof (T))),
					&std::free
				};
				for (size_t i = 0; i < len; ++i)
				{
					boost::python::object elem = list[i];
					array.get()[i] = boost::python::extract<T>(elem);
				}
				self.push_static_content(kind, std::move(array), len);
			}

			static
			void
			push_static_content(renderer::VertexBuffer& self,
			                    ContentKind kind,
			                    boost::python::list list)
			{
				size_t len = boost::python::len(list);
				if (len == 0)
					throw std::runtime_error("empty content");

				boost::python::object first = list[0];

#define _CHECK(type) \
				if (boost::python::extract<type>(first).check()) \
					return _push_static_content<type>(self, kind, list, len)

				switch (kind)
				{
					case ContentKind::color:
						_CHECK(cube::gl::color::Color3f);
						_CHECK(cube::gl::color::Color4f);
						break;
					case ContentKind::index:
						_CHECK(uint8_t);
						_CHECK(uint16_t);
						_CHECK(uint32_t);
						break;
					default:
						_CHECK(cube::gl::vector::Vector2f);
						_CHECK(cube::gl::vector::Vector3f);

				}
#undef _CHECK

				throw std::runtime_error(
					"List content has to be a color or a vector, got '" +
					std::string(
						boost::python::extract<std::string>(
							boost::python::str(first)
						)
					) + "'."
				);
			}
		};
*/
namespace cube { namespace gl { namespace renderer_bindings {

	void export_vertex_buffer()
	{
		py::class_<
				VertexBuffer,
				std::auto_ptr<VertexBuffer>,
				boost::noncopyable,
				py::bases<BindableBase>
			>(
				"VertexBuffer",
				py::no_init
			)
		;
	}

}}}
