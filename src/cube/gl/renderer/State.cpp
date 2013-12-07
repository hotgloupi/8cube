#include "State.hpp"

#include "Exception.hpp"
#include "Light.hpp"
#include "Painter.hpp"

#include <etc/assert.hpp>
#include <etc/log.hpp>
#include <etc/to_string.hpp>
#include <etc/stack_ptr.hpp>

#include <glm/gtc/matrix_inverse.hpp>

#include <unordered_set>

namespace cube { namespace gl { namespace renderer {

	ETC_LOG_COMPONENT("cube.gl.renderer.State");

	struct State::Impl
	{
		matrix_type                        model;
		matrix_type                        view;
		matrix_type                        projection;
		LightList                          lights;
		etc::stack_ptr<matrix_type>        mvp;
		etc::stack_ptr<matrix_type>        model_view;
		etc::stack_ptr<normal_matrix_type> normal;

		Impl()
			: mvp{etc::stack_ptr_no_init}
			, model_view{etc::stack_ptr_no_init}
			, normal{etc::stack_ptr_no_init}
		{}

		Impl(Impl const& other)
			: model(other.model)
			, view(other.view)
			, projection(other.projection)
			, lights{other.lights}
			, mvp{other.mvp}
			, model_view{other.model_view}
			, normal{other.normal}
		{}

		void clear()
		{
			this->model_view.clear();
			this->normal.clear();
			this->mvp.clear();
		}
	};

	State::State(Mode const mode)
		: mode(mode)
		, _this{new Impl{}}
	{}

	State::State(State&& other)
		ETC_NOEXCEPT_IF(
			std::is_nothrow_move_constructible<std::unique_ptr<State::Impl>>()
		)
		: mode{other.mode}
		, _this{std::move(other._this)}
	{}

	State::State(State const& other)
		: mode{other.mode}
		, _this{new Impl{*other._this}}
	{}

	State::~State()
	{}

	matrix_type const&
	State::matrix(MatrixKind kind) const
	{
		switch (kind)
		{
		case MatrixKind::model:
			return _this->model;
		case MatrixKind::view:
			return _this->view;
		case MatrixKind::projection:
			return _this->projection;
		case MatrixKind::mvp:
			return this->mvp();
		default:
			throw Exception{"Invalid matrix kind"};
		}
	}

	void
	State::matrix(MatrixKind kind, matrix_type const& other)
	{
		ETC_TRACE.debug("Set matrix", kind, "to", other);
		switch (kind)
		{
		case MatrixKind::model:
			this->model(other);
			break;
		case MatrixKind::view:
			this->view(other);
			break;
		case MatrixKind::projection:
			this->projection(other);
			break;
		case MatrixKind::mvp:
			throw Exception{"mvp Matrix is read only"};
		default:
			throw Exception{"Invalid matrix kind"};
		}
	}

	matrix_type const& State::model() const ETC_NOEXCEPT
	{ return _this->model; }

	matrix_type const& State::view() const ETC_NOEXCEPT
	{ return _this->view; }

	matrix_type const& State::projection() const ETC_NOEXCEPT
	{ return _this->projection; }

	matrix_type State::model_view() const ETC_NOEXCEPT
	{
		if (!_this->model_view)
			_this->model_view.reset(_this->view * _this->model);
		return *_this->model_view;
	}

	matrix_type const& State::mvp() const ETC_NOEXCEPT
	{
		if (!_this->mvp)
			_this->mvp.reset(_this->projection * this->model_view());
		return *_this->mvp;
	}

	State::normal_matrix_type State::normal() const ETC_NOEXCEPT
	{
		if (!_this->normal)
			_this->normal.reset(
				glm::inverseTranspose(normal_matrix_type(this->model_view()))
			);
		return *_this->normal;
	}

	State& State::model(matrix_type const& other) ETC_NOEXCEPT
	{ _this->model = other; _this->clear(); return *this; }

	State& State::view(matrix_type const& other) ETC_NOEXCEPT
	{ _this->view = other; _this->clear(); return *this; }

	State& State::projection(matrix_type const& other) ETC_NOEXCEPT
	{ _this->projection = other; _this->mvp.clear(); return *this; }

	State& State::scale(component_type const x,
	                    component_type const y,
	                    component_type const z) ETC_NOEXCEPT
	{ return this->model(matrix::scale(this->model(), vector_type(x, y, z))); }

	State& State::translate(component_type const x,
	                        component_type const y,
	                        component_type const z) ETC_NOEXCEPT
	{
		return this->model(
			matrix::translate(this->model(), vector_type(x, y, z))
		);
	}

	State& State::rotate(units::Angle const angle,
	                     vector_type const& axis) ETC_NOEXCEPT
	{ return this->model(matrix::rotate(this->model(), angle, axis)); }

	State& State::look_at(vector_type const& eye,
	                      vector_type const& center,
	                      vector_type const& up) ETC_NOEXCEPT
	{ return this->view(matrix::look_at(eye, center, up)); }

	State& State::perspective(component_type const fov,
	                          component_type const aspect,
	                          component_type const near,
	                          component_type const far) ETC_NOEXCEPT
	{
		ETC_ASSERT_NEQ(far, near);
		return this->projection(matrix::perspective(fov, aspect, near, far));
	}

	State& State::ortho(component_type const x,
	                    component_type const y,
	                    component_type const w,
	                    component_type const h) ETC_NOEXCEPT
	{ return this->projection(matrix::ortho(x, y, w, h)); }

	State::LightList const& State::lights() const ETC_NOEXCEPT
	{ return _this->lights; }

	void State::enable(Light const& light)
	{
		if (!light.bound())
			throw Exception{"Cannot enable an unbound light"};
		auto it = _this->lights.begin(), end = _this->lights.end();
		for (; it != end; ++it)
			if (&light == &it->get())
				throw Exception{"This light is already enabled"};
		_this->lights.emplace_back(light);
	}

	void State::disable(Light const& light)
	{
		auto it = _this->lights.begin(), end = _this->lights.end();
		for (; it != end; ++it)
			if (&light == &it->get())
			{
				_this->lights.erase(it);
				return;
			}
		throw Exception{"Light not found"};
	}

}}} // !cube::gl::renderer
