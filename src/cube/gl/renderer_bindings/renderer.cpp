#include "exports.hpp"

#include "PainterWithProxy.hpp"

#include "../renderer/Renderer.hpp"
#include "../renderer/ShaderProgram.hpp"
#include "../renderer/Texture.hpp"
#include "../renderer/VertexBuffer.hpp"
#include "../renderer/VertexBufferAttribute.hpp"

namespace {

	namespace renderer = cube::gl::renderer;
	namespace renderer_bindings = cube::gl::renderer_bindings;

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
			_tuple_to_vector(boost::python::tuple& args)
			{
				size_t len = boost::python::len(args);
				std::vector<std::unique_ptr<T>> ptrs;
				for (size_t i = 0; i < len; ++i)
				{
					std::auto_ptr<T> ptr =
						boost::python::extract<std::auto_ptr<T>>(args[i]);
					ptrs.emplace_back(ptr.release());
				}
				return ptrs;
			}

			static
			renderer::ShaderProgramPtr
			new_shader_program(renderer::Renderer& self,
			                   boost::python::tuple args)
			{
				return self.new_shader_program(
					_tuple_to_vector<renderer::Shader>(args)
				);
			}

			static
			renderer::VertexBufferPtr
			new_vertex_buffer(renderer::Renderer& self,
			                  boost::python::tuple args)
			{
				return self.new_vertex_buffer(
					_tuple_to_vector<renderer::VertexBufferAttribute>(args)
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

} //!anonymous

namespace cube { namespace gl { namespace renderer_bindings {

	void export_renderer()
	{
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
				"swap_buffers",
				&Renderer::swap_buffers
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
