#include "Bindable.hpp"

#include "Exception.hpp"
#include "State.hpp"

#include <etc/assert.hpp>
#include <etc/test.hpp>
#include <etc/log.hpp>

namespace cube { namespace gl { namespace renderer {

	ETC_LOG_COMPONENT("cube.gl.renderer.Bindable");

	Bindable::Bindable() ETC_NOEXCEPT
		: _bound{0}
		, _bound_state{}
	{}

	Bindable::~Bindable()
	{ ETC_ASSERT_EQ(_bound, 0); }

	void Bindable::_bind(InternalMethod)
	{
		ETC_TRACE.debug("Bind", *this);
		_bound++;
		if (_bound == 1)
		{
			try { _bind(); }
			catch (...) { _bound = 0; throw; }
		}
	}

	void Bindable::_unbind(InternalMethod) ETC_NOEXCEPT
	{
		ETC_TRACE.debug("Unbind", *this);
		ETC_ASSERT_GT(_bound, 0);
		if (--_bound == 0) _unbind();
	}

	State& Bindable::bound_state()
	{
		if (_bound_state == nullptr)
			throw Exception{"No bound state"};
		return *_bound_state;
	}

	Bindable::Guard::Guard(Bindable& bindable,
	                       std::shared_ptr<State> const& state)
		: _bindable(&bindable)
	{
		if (state == nullptr)
			throw Exception{"Cannot bind with a null state"};
		if (_bindable->_bound_state != nullptr &&
		    _bindable->_bound_state != state)
			throw Exception{
				"Cannot bind the same object to two different state"
			};
		_bindable->_bound_state = state;
		_bindable->_bind(Bindable::internal_method);
	}

	Bindable::Guard::~Guard()
	{
		if (_bindable != nullptr)
		{
			_bindable->_unbind(Bindable::internal_method);
			_bindable->_bound_state = nullptr;
		}
	}

	namespace {

		ETC_TEST_CASE(base)
		{
			struct Dummy : public Bindable
			{
				int called;
				Dummy() : called{0} {}
				void _bind() override { ++called; }
				void _unbind() ETC_NOEXCEPT override { --called; }
			};

			Dummy dummy;
			auto state = std::make_shared<State>(Mode::_2d);
			ETC_ENFORCE_EQ(dummy.bound(), 0);
			ETC_ENFORCE_EQ(dummy.called, 0);
			{
				Bindable::Guard g(dummy, state);
				ETC_ENFORCE_EQ(dummy.bound(), 1);
				ETC_ENFORCE_EQ(dummy.called, 1);
				{
					Bindable::Guard g(dummy, state);
					ETC_ENFORCE_EQ(dummy.bound(), 2);
					ETC_ENFORCE_EQ(dummy.called, 1);
				}
				ETC_ENFORCE_EQ(dummy.bound(), 1);
				ETC_ENFORCE_EQ(dummy.called, 1);
			}
			ETC_ENFORCE_EQ(dummy.bound(), 0);
			ETC_ENFORCE_EQ(dummy.called, 0);
		}

		ETC_TEST_CASE(throwing_bind)
		{
			struct Dummy : public Bindable
			{
				int called;
				Dummy() : called{0} {}
				void _bind() override
				{ ++called; throw std::runtime_error{"lol"}; }
				void _unbind() ETC_NOEXCEPT override
				{ throw std::logic_error{"never called"}; }
			};

			Dummy dummy;
			auto state = std::make_shared<State>(Mode::_2d);

			try {
				Bindable::Guard g(dummy, state);
				ETC_ENFORCE(false && "Shouldn't be reached");
			} catch (std::runtime_error const& e) {
				ETC_ENFORCE_EQ(e.what(), std::string("lol"));
			} catch (...) {
				ETC_ENFORCE(false && "Shouldn't go there");
			}

			ETC_ENFORCE_EQ(dummy.called, 1);
			ETC_ENFORCE_EQ(dummy.bound(), 0);
		}

		ETC_TEST_CASE(bound_in_bind_method)
		{
			struct Dummy : public Bindable
			{
				void _bind() override { ETC_ENFORCE(this->bound()); }
				void _unbind() ETC_NOEXCEPT override {}
			};
			Dummy d;
			Bindable::Guard g(d, std::make_shared<State>(Mode::_2d));
		}

	} // !anonymous

}}}
