#include "str.hpp"

#include <etc/test.hpp>

namespace etc {

	str::str(std::string s)
		: _data{std::move(s)}
	{}

	str::str()
		: _data{}
	{}

	str::str(str const& other)
		: _data{other._data}
	{}

	str::str(str&& other)
		: _data{std::move(other._data)}
	{}

	str str::replace(std::string const& old,
	                 std::string const& new_) const
	{
		std::string res{_data}; // XXX slow as shit, replace with boost
		std::size_t pos = 0;
		do {
			pos = res.find(old, pos);
			if (pos == std::string::npos)
				return str{std::move(res)};
			res.replace(
				res.begin() + pos,
				res.begin() + pos + old.size(),
				new_
			);
			pos += new_.size();
		} while (true);
	}

	std::ostream& operator <<(std::ostream& out, str const& s)
	{ return out << s.data(); }

	namespace {

		ETC_TEST_CASE(ctor)
		{
			{ str s; ETC_TEST(s.empty()); }
			{ str s{"TEST"}; ETC_TEST(!s.empty()); ETC_TEST_EQ(s.data(), "TEST"); }
			{ str s{"TEST"}; str s2{s};  ETC_TEST_EQ(s2.data(), "TEST"); }
			{ str s{"TEST"}; str s2{std::move(s)}; ETC_TEST_EQ(s2.data(), "TEST"); }
		}

		ETC_TEST_CASE(bool_conversion)
		{
			str empty;
			if (empty) ETC_ERROR("Should be false for empty string");
			if (!(!empty)) ETC_ERROR("Should be false for empty string");
			if (empty == true) ETC_ERROR("Should be false for empty string");
			if (!empty == false) ETC_ERROR("Should be false for empty string");

			str not_empty{"TEST"};
			if (not_empty) {} else ETC_ERROR("Should be true");
			if (!(!not_empty)) {} else ETC_ERROR("Should be true");
			if (not_empty == true) {} else ETC_ERROR("Should be true");
			if (!not_empty == false) {} else ETC_ERROR("Should be true");
		}

		ETC_TEST_CASE(string_conversion)
		{
			{ str empty; std::string s(empty); ETC_TEST_EQ(s, ""); }
			{ std::string s(str{}); ETC_TEST_EQ(s, ""); }
			{ std::string s(str{"LOL"}); ETC_TEST_EQ(s, "LOL"); }
			{ std::string s; s = str("LOL"); ETC_TEST_EQ(s, "LOL"); }
			auto f1 = [] (std::string) {};
			auto f2 = [] (std::string&&) {};
			auto f3 = [] (std::string const&) {};
			{ f1(str{}); f2(str{}); f3(str{});  }
			{ str s; f1(s); f2(s); f3(s);  }
		}

		ETC_TEST_CASE(replace)
		{
			ETC_TEST_EQ(str("aaaa").replace("a", "b"), "bbbb");
			ETC_TEST_EQ(str("aaaa").replace("aa", "a"), "aa");
			ETC_TEST_EQ(str("aaaa").replace("aa", "a").replace("a", "bb"), "bbbb");
			ETC_TEST_EQ(str("pif paf pouf").replace("paf ", ""), "pif pouf");
		}

	}

}
