#include "Bindable.hpp"

#include "Exception.hpp"
#include "State.hpp"

#include <etc/assert.hpp>
#include <etc/test.hpp>

namespace cube { namespace gl { namespace renderer {

	Bindable::Bindable() noexcept
		: _bound{0}
		, _bound_state{nullptr}
	{}

	Bindable::~Bindable()
	{ ETC_ASSERT_EQ(_bound, 0); }

	State& Bindable::bound_state()
	{
		if (_bound_state == nullptr)
			throw Exception{"No bound state"};
		return *_bound_state;
	}

	Bindable::Guard::Guard(Bindable& bindable, State& state)
		: _bindable(bindable)
	{
		if (_bindable._bound_state != nullptr &&
		    _bindable._bound_state != &state)
			throw Exception{
				"Cannot bind the same object to two different state"
			};
		_bindable._bound_state = &state;
		_bindable._bind(Bindable::internal_method);
	}

	Bindable::Guard::~Guard()
	{
		_bindable._unbind(Bindable::internal_method);
		_bindable._bound_state = nullptr;
	}

	namespace {

		ETC_TEST_CASE(base)
		{
			struct Dummy : public Bindable
			{
				int called;
				Dummy() : called{0} {}
				void _bind() override { ++called; }
				void _unbind() noexcept override { --called; }
			};

			Dummy dummy;
			State state(Mode::_2d);
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
				void _unbind() noexcept override
				{ throw std::logic_error{"never called"}; }
			};

			Dummy dummy;
			State state(Mode::_2d);

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

	} // !anonymous

}}}
