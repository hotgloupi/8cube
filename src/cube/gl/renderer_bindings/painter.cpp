#include "exports.hpp"
#include "PainterWithProxy.hpp"

#include "../renderer/Painter.hpp"
#include "../renderer/State.hpp"
#include "../renderer/ShaderProgram.hpp"
#include "../renderer/VertexBuffer.hpp"
#include "../renderer/VertexBufferAttribute.hpp"

#include <wrappers/boost/python.hpp>

namespace {

	namespace renderer = cube::gl::renderer;
	namespace renderer_bindings = cube::gl::renderer_bindings;

	struct Wrap
	{
		struct PainterBindProxy
		{
			typedef std::unique_ptr<void, std::function<void(void*)>> ProxyPtr;

			renderer::Painter&  _painter;
			boost::python::list _bindables;
			size_t              _len;
			ProxyPtr            _proxy;

			PainterBindProxy(renderer::Painter& painter,
			                 boost::python::list& bindables)
				: _painter(painter)
				, _bindables{bindables}
				, _len{(size_t)boost::python::len(bindables)}
				, _proxy{}
			{}

			// This shit convert a dynamic list to a static list of arguments.
			// Generating for 100 elements takes 60 seconds on my machine...
			// TODO enable bigger values for release builds ?
#define MAX_BIND_LIST_SIZE 40
			template<size_t size, typename... Args>
			inline
			typename std::enable_if<size == 0>::type
			_build_proxy(Args&&... args)
			{
				renderer::Bindable& bindable =
					boost::python::extract<renderer::Bindable&>(_bindables[size]);
				_build_proxy<size + 1>(std::forward<Args>(args)..., bindable);
			}

			template<size_t size, typename... Args>
			inline
			typename std::enable_if<0 < size && size < MAX_BIND_LIST_SIZE>::type
			_build_proxy(Args&&... args)
			{
				if (size < _len)
				{
					renderer::Bindable& bindable =
						boost::python::extract<renderer::Bindable&>(_bindables[size]);
					_build_proxy<size + 1>(std::forward<Args>(args)..., bindable);
				}
				else
				{
					_proxy = ProxyPtr{
						new renderer::Painter::Proxy<size>{
							_painter.with(std::forward<Args>(args)...)
						},
						[] (void* ptr) { delete (renderer::Painter::Proxy<size>*) ptr; }
					};
				}
			}

			template<size_t size, typename... Args>
			inline
			typename std::enable_if<size >= MAX_BIND_LIST_SIZE>::type
			_build_proxy(Args&&...)
			{
				std::abort(); // should not happen size checked in __enter__
			}

			renderer::Painter& __enter__()
			{
				if (_len > MAX_BIND_LIST_SIZE)
					throw renderer::Exception{"Bindable list too big"};

				_build_proxy<0>();
				return _painter;
			}

			void __exit__(PyObject*,
			              PyObject*,
			              PyObject*)
			{
				_proxy.reset();
			}
		};

		struct Painter
		{
			static
			PainterBindProxy* bind(renderer::Painter& self, boost::python::list bindables)
			{
				//self.bind(o);
				return new PainterBindProxy{self, bindables};
			}

			static
			void draw_list(renderer::Painter& self,
			               boost::python::list drawables)
			{
				namespace py = boost::python;
				std::vector<renderer::Drawable*> extracted;
				for (int i = 0, len = py::len(drawables); i < len; ++i)
				{
					py::extract<renderer::Drawable*> extractor{drawables[i]};
					if (!extractor.check())
						throw renderer::Exception{
							"Cannot convert argument to Drawable instance"
						};
					extracted.push_back(extractor());
				}
				self.draw(extracted);
			}
		};

		///////////////////////////////////////////////////////////////////////
		// PainterWithProxy

		struct PainterWithProxy
		{
			static
			void
			__exit__(renderer_bindings::PainterWithProxy& self,
			         PyObject*,
			         PyObject*,
			         PyObject*)
			{
				self.__exit__();
			}
		};
	};

} // !anonymous

namespace cube { namespace gl { namespace renderer_bindings {

	namespace py = boost::python;
	using namespace ::cube::gl::renderer;

	void export_painter()
	{
		py::class_<Painter, boost::noncopyable>(
				"Painter", py::no_init
			)
			.def(
				"bind",
				&Wrap::Painter::bind,
				py::args("drawables", "A list of drawables"),
				"Bound drawables can be unbound with the `unbind' method but are"
				" automatically unbound when the painter is deleted.",
				py::return_value_policy<
				    py::manage_new_object
				  , py::with_custodian_and_ward_postcall<0, 2>
				>()
			)
			.def(
				"draw_elements",
				&Painter::draw_elements
			)
			.def("draw", &Wrap::Painter::draw_list)
			.add_property(
				"state",
				py::make_function(
					&Painter::state,
					py::return_internal_reference<>()
				)
			)
		;

		///////////////////////////////////////////////////////////////////////////
		// PainterWithProxy

		py::class_<PainterWithProxy>(
				"PainterWithProxy", py::no_init
			)
			.def(
				"__enter__",
				&PainterWithProxy::__enter__,
				py::return_internal_reference<1>()
			)
			.def(
				"__exit__",
				&Wrap::PainterWithProxy::__exit__
			)
		;


		///////////////////////////////////////////////////////////////////////////
		// PainterBindProxy

		py::class_<Wrap::PainterBindProxy, boost::noncopyable>(
				"PainterBindProxy", py::no_init
			)
			.def(
				"__enter__",
				&Wrap::PainterBindProxy::__enter__,
				py::return_internal_reference<>()
			)
			.def(
				"__exit__",
				&Wrap::PainterBindProxy::__exit__
			)
		;
	}

}}}


