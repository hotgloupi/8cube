#include "exports.hpp"

#include "../color.hpp"
#include "../exception.hpp"
#include "../renderer/VertexBufferAttribute.hpp"

#include <etc/to_string.hpp>

#include <wrappers/boost/python.hpp>

namespace cube { namespace gl { namespace renderer_bindings {

	namespace py = boost::python;
	using namespace ::cube::gl::renderer;

	typedef cube::gl::exception::Exception Exception;

	namespace {

		template<typename T>
		VertexBufferAttributePtr
		_make_vb_attr(ContentKind kind,
		              py::list list,
		              etc::size_type const len,
		              ContentHint const hint)
		{
			std::unique_ptr<T, void(*)(void*)> array{
				static_cast<T*>(::malloc(len * sizeof (T))),
				&std::free
			};
			for (etc::size_type i = 0; i < len; ++i)
			{
				boost::python::object elem = list[i];
				array.get()[i] = boost::python::extract<T>(elem);
			}
			return make_vertex_buffer_attribute(kind, std::move(array), len, hint);
		}

		VertexBufferAttributePtr
		make_vb_attr(ContentKind kind,
		             boost::python::list list,
		             ContentHint const hint = ContentHint::static_content)
		{
			etc::size_type const len = boost::python::len(list);
			if (len == 0)
				throw Exception{
					"Empty content"
				};

			boost::python::object first = list[0];

			switch (kind)
			{
#define _CHECK(type)                                                          \
			if (boost::python::extract<type>(first).check())                  \
				return _make_vb_attr<type>(kind, list, len, hint)             \
/**/
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
#undef _CHECK
			}

			throw Exception{
				"List content has to be a color or a vector, got '" +
				std::string(
					boost::python::extract<std::string>(
						boost::python::str(first)
					)
				) + "'."
			};

			throw "Never reached";
			return VertexBufferAttributePtr{nullptr};
		}

		std::string __str__(std::auto_ptr<VertexBufferAttribute> const& self_)
		{
			if (self_.get() == nullptr)
				return "<VertexBufferAttribute (null)>";
			auto self = *self_.get();
			return etc::to_string(
				"<VertexBufferAttribute",
				self.kind,
				self.type,
				self.hint,
				"arity=" + etc::to_string(self.arity),
				"nb_elements=" + etc::to_string(self.nb_elements),
				etc::iomanip::nosep(),
				'>'
			);
		}

	} // !anonymous

	void export_vertex_buffer_attribute()
	{
		py::class_<
		    VertexBufferAttribute
		  , std::auto_ptr<VertexBufferAttribute>
		  , boost::noncopyable
		>("VertexBufferAttribute", py::no_init)
			.def("__str__", &__str__)
		;

		py::def(
			"make_vertex_buffer_attribute",
			&make_vb_attr
		);
	}

}}}
