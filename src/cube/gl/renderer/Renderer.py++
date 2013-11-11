#include "PainterWithProxy.hpp"

#include <cube/api.hpp>
#include <cube/gl/exception.hpp>
#include <cube/gl/renderer/Renderer.hpp>
#include <cube/gl/renderer/ShaderGenerator.hpp>
#include <cube/gl/renderer/ShaderProgram.hpp>
#include <cube/gl/renderer/Texture.hpp>
#include <cube/gl/renderer/VertexBufferAttribute.hpp>
#include <cube/gl/renderer/VertexBuffer.hpp>

#include <wrappers/boost/python.hpp>

namespace {

	namespace renderer = cube::gl::renderer;
	namespace renderer_bindings = cube::gl::renderer_bindings;
	namespace py = boost::python;

	struct Wrap
	{
		struct Renderer
		{
			static
			renderer_bindings::PainterWithProxy
			begin(renderer::Renderer& self,
			      renderer::Mode mode)
			{
				return renderer_bindings::PainterWithProxy(self, mode);
			}

			static
			void
			clear(renderer::Renderer& self,
			      int buffer_bit)
			{
				self.clear(
					static_cast<cube::gl::renderer::BufferBit>(buffer_bit)
				);
			}

			static
			void
			viewport(renderer::Renderer& self,
			         int x, int y, int w, int h)
			{
				self.viewport(
					cube::gl::viewport::Viewport(x, y, w, h)
				);
			}

			template<typename T>
			static
			std::vector<std::unique_ptr<T>>
			_list_to_vector(boost::python::list& args)
			{
				size_t len = boost::python::len(args);
				std::vector<std::unique_ptr<T>> ptrs;
				for (size_t i = 0; i < len; ++i)
				{
					std::auto_ptr<T> ptr = boost::python::extract<std::auto_ptr<T>>(args[i]);
					if (ptr.get() == nullptr)
						throw cube::gl::exception::Exception{
							"Found a null pointer in the list!"
						};
					ptrs.emplace_back(ptr.release());
					args[i] = boost::python::object{};
				}
				return ptrs;
			}

			static
			renderer::ShaderProgramPtr
			new_shader_program(renderer::Renderer& self,
			                   boost::python::list args)
			{
				return self.new_shader_program(
					_list_to_vector<renderer::Shader>(args)
				);
			}

			static
			renderer::ShaderGeneratorProxy
			generate_shader(renderer::Renderer& self,
			                renderer::ShaderType const type)
			{
				return self.generate_shader(type);
			}

			static
			renderer::VertexBufferPtr
			new_vertex_buffer(renderer::Renderer& self,
			                  boost::python::list args)
			{
				return self.new_vertex_buffer(
					_list_to_vector<renderer::VertexBufferAttribute>(args)
				);
			}

			static
			renderer::VertexBufferPtr
			new_index_buffer(renderer::Renderer& self,
			                 std::auto_ptr<renderer::VertexBufferAttribute>& attr)
			{
				return self.new_index_buffer(
					renderer::VertexBufferAttributePtr{attr.release()}
				);
			}

		};

	};

	struct vector_of_string_from_python
	{
		vector_of_string_from_python()
		{
		}

		static
		void* convertible(PyObject* ptr)
		{
			// is convertible from list and tuple. if len(ptr) > 0,
			if (PyList_Check(ptr) || PyTuple_Check(ptr))
				return ptr;
			return nullptr;
		}

		static
		void construct(PyObject* ptr,
		               py::converter::rvalue_from_python_stage1_data* data)
		{
			typedef PyObject* (*extractor_t)(PyObject*, ssize_t);

			extractor_t extractor;
			Py_ssize_t len;

			if (PyList_Check(ptr))
			{
				extractor = &PyList_GetItem;
				len = PyList_Size(ptr);
			}
			else if (PyTuple_Check(ptr))
			{
				extractor = &PyTuple_GetItem;
				len = PyTuple_Size(ptr);
			}
			else
				throw cube::gl::exception::Exception{
					"convertible has messed up"
				};

			typedef py::converter::rvalue_from_python_storage<std::vector<std::string>> storage_t;

			// Grab pointer to memory into which to construct the new QString
			void* storage = ((storage_t*) data)->storage.bytes;

			assert(storage != nullptr && "Boost.Python gave a null storage");

			// in-place construct the new std::vector<std::string>.
			auto vec = new (storage) std::vector<std::string>{};

			for (Py_ssize_t i = 0; i < len; ++i)
			{
				PyObject* item = (*extractor)(ptr, i);
				if (!PyUnicode_Check(item))
					throw cube::gl::exception::Exception{
						"Items has to be of type unicode"
					};
				char* str = PyBytes_AsString(PyUnicode_AsUTF8String(item));
				if (str == nullptr)
					throw py::error_already_set{};
				vec->emplace_back(str);
			}

			// Stash the memory chunk pointer for later use by boost.python
			data->convertible = storage;
		}
	};

} //!anonymous

namespace cube { namespace gl { namespace renderer_bindings {

	using namespace ::cube::gl::renderer;

	CUBE_API
	void export_renderer()
	{

		py::converter::registry::push_back(
			&vector_of_string_from_python::convertible,
			&vector_of_string_from_python::construct,
			py::type_id<std::vector<std::string>>()
		);

		typedef TexturePtr (Renderer::*new_texture_cb_t)(std::string const&);

		typedef TexturePtr (Renderer::*new_texture_raw_cb_t)(
			PixelFormat const,
			unsigned int,
			unsigned int,
			renderer::PixelFormat const,
			renderer::ContentPacking const,
			void const*
		);

		typedef ShaderPtr (Renderer::*new_fragment_shader_t)(
			std::vector<std::string> const&
		);

		typedef ShaderPtr (Renderer::*new_vertex_shader_t)(
			std::vector<std::string> const&
		);

		py::class_<Renderer, boost::shared_ptr<Renderer>, boost::noncopyable>(
				"Renderer", py::no_init
			)
			.def(
				"description",
				&Renderer::description,
				py::return_internal_reference<>()
			)
			.def(
				"viewport",
				&Wrap::Renderer::viewport
			)
			.def(
				"begin",
				&Wrap::Renderer::begin,
				py::return_value_policy<
					py::return_by_value,
					py::with_custodian_and_ward_postcall<1,0>
				>()
			)
			.add_property(
				"state",
				py::make_function(
					&Renderer::current_state,
					py::return_internal_reference<>()
				)
			)
			.def(
				"new_vertex_buffer",
				&Wrap::Renderer::new_vertex_buffer,
				py::return_value_policy<
					py::return_by_value,
					py::with_custodian_and_ward_postcall<1,0>
				>()
			)
			.def(
				"new_index_buffer",
				&Wrap::Renderer::new_index_buffer,
				py::return_value_policy<
					py::return_by_value,
					py::with_custodian_and_ward_postcall<1,0>
				>()
			)
			.def(
				"generate_shader",
				&Wrap::Renderer::generate_shader
				//py::return_value_policy<
				//	py::return_by_value,
				//	py::with_custodian_and_ward_postcall<1,0>
				//>()
			)
			.def(
				"new_shader_program",
				&Wrap::Renderer::new_shader_program,
				py::return_value_policy<
					py::return_by_value,
					py::with_custodian_and_ward_postcall<1,0>
				>()
			)
			.def(
				"new_fragment_shader",
				static_cast<new_fragment_shader_t>(&Renderer::new_fragment_shader),
				py::return_value_policy<
					py::return_by_value,
					py::with_custodian_and_ward_postcall<1,0>
				>()
			)
			.def(
				"new_vertex_shader",
				static_cast<new_vertex_shader_t>(&Renderer::new_vertex_shader),
				py::return_value_policy<
					py::return_by_value,
					py::with_custodian_and_ward_postcall<1,0>
				>()
			)
			.def(
				"new_texture",
				static_cast<new_texture_cb_t>(
					&Renderer::new_texture
				),
				py::return_value_policy<
					py::return_by_value,
					py::with_custodian_and_ward_postcall<1,0>
				>()
			)
			.def(
				"new_texture",
				static_cast<new_texture_raw_cb_t>(
					&Renderer::new_texture
				),
				py::return_value_policy<
					py::return_by_value,
					py::with_custodian_and_ward_postcall<1,0>
				>()
			)
			.def(
				"clear",
				&Wrap::Renderer::clear
			)
		;

		///////////////////////////////////////////////////////////////////////////
		// RendererType

		py::class_<RendererType, boost::noncopyable>(
				"RendererType",
				py::no_init
			)
			.def(
				"__str__",
				&RendererType::__str__
			)
		;
	}

}}}
