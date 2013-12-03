#include "State.hpp"

#include "Exception.hpp"
#include "Light.hpp"
#include "Painter.hpp"

#include <etc/assert.hpp>
#include <etc/log.hpp>
#include <etc/to_string.hpp>

#include <glm/gtc/matrix_inverse.hpp>

#include <unordered_set>

namespace cube { namespace gl { namespace renderer {

	ETC_LOG_COMPONENT("cube.gl.renderer.State");

	struct State::Impl
	{
		matrix_type           model;
		matrix_type           view;
		matrix_type           projection;
		matrix_type           mvp;
		bool                  mvp_dirty;
		LightList             lights;
		Impl() : mvp_dirty{false} {}
		Impl(Impl const& other)
			: model(other.model)
			, view(other.view)
			, projection(other.projection)
			, mvp(other.mvp)
			, mvp_dirty{other.mvp_dirty}
			, lights{other.lights}
		{}
	};

	State::State(Mode const mode)
		: mode(mode)
		, _this{new Impl{}}
	{}

	State::State(State&& other)
		noexcept(
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
			_this->model = other;
			break;
		case MatrixKind::view:
			_this->view = other;
			break;
		case MatrixKind::projection:
			_this->projection = other;
			break;
		case MatrixKind::mvp:
			throw Exception{"mvp Matrix is read only"};
		default:
			throw Exception{"Invalid matrix kind"};
		}
		_this->mvp_dirty = true;
	}

	matrix_type const& State::model() const noexcept
	{ return _this->model; }

	matrix_type const& State::view() const noexcept
	{ return _this->view; }

	matrix_type const& State::projection() const noexcept
	{ return _this->projection; }

	matrix_type const& State::mvp() const noexcept
	{
		if (_this->mvp_dirty)
		{
			_this->mvp_dirty = false;
			_this->mvp = _this->projection * _this->view * _this->model;
		}
		return _this->mvp;
	}

	State::normal_matrix_type State::normal() const noexcept
	{
		return glm::inverseTranspose(glm::mat3(_this->view * _this->model));
	}

	State& State::model(matrix_type const& other) noexcept
	{ _this->model = other; _this->mvp_dirty = true; return *this; }

	State& State::view(matrix_type const& other) noexcept
	{ _this->view = other; _this->mvp_dirty = true; return *this; }

	State& State::projection(matrix_type const& other) noexcept
	{ _this->projection = other; _this->mvp_dirty = true; return *this; }

	State& State::scale(component_type const x,
	                    component_type const y,
	                    component_type const z) noexcept
	{ return this->model(matrix::scale(this->model(), vector_type(x, y, z))); }

	State& State::translate(component_type const x,
	                        component_type const y,
	                        component_type const z) noexcept
	{
		return this->model(
			matrix::translate(this->model(), vector_type(x, y, z))
		);
	}

	State& State::rotate(units::Angle const angle,
	                     vector_type const& axis) noexcept
	{ return this->model(matrix::rotate(this->model(), angle, axis)); }

	State& State::look_at(vector_type const& eye,
	                      vector_type const& center,
	                      vector_type const& up) noexcept
	{ return this->view(matrix::look_at(eye, center, up)); }

	State& State::perspective(component_type const fov,
	                          component_type const aspect,
	                          component_type const near,
	                          component_type const far) noexcept
	{
		ETC_ASSERT_NEQ(far, near);
		return this->projection(matrix::perspective(fov, aspect, near, far));
	}

	State& State::ortho(component_type const x,
	                    component_type const y,
	                    component_type const w,
	                    component_type const h) noexcept
	{ return this->projection(matrix::ortho(x, y, w, h)); }

	State::LightList const& State::lights() const noexcept
	{ return _this->lights; }

	void State::enable(Light const& light)
	{
		if (not light.bound())
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
