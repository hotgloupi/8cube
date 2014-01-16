#include "path.hpp"

#include <etc/platform.hpp>
#include <etc/test.hpp>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/filesystem.hpp>

#ifdef ETC_PLATFORM_WINDOWS
# include <wrappers/windows.hpp>
# define SEPARATORS "\\/"
#else
# include <fnmatch.h>
# define SEPARATORS "/"
#endif

namespace etc { namespace path {

    namespace fs = boost::filesystem;

	std::string absolute(std::string const& path)
	{
		return fs::absolute(path).string();
	}

	std::string directory_name(std::string const& path_)
	{
		fs::path path{path_};
		if (!fs::is_directory(path))
			path.remove_filename();
		return path.string();
	}

	bool is_directory(std::string const& path)
	{
		return fs::is_directory(path);
	}

	ETC_API bool exists(std::string const& path)
	{
		return fs::exists(path);
	}

	std::string join(std::vector<std::string> const& parts)
	{
		if (parts.size() == 0)
			return "";

		auto it = parts.begin(),
		     end = parts.end();
		fs::path path{*it};
		while (++it != end)
			path /= *it;
		return path.string();
	}

	std::vector<std::string> split(std::string path)
	{
		boost::trim_right_if(path, boost::is_any_of(SEPARATORS));
		std::vector<std::string> res;
		boost::split(
			res,
			path,
			boost::is_any_of(SEPARATORS),
			boost::token_compress_on
		);
		return res;
	}

	ETC_API bool match(std::string const& pattern,
	                   std::string const& str)
	{
#ifdef _WIN32
			return (::PathMatchSpec(str.c_str(), pattern.c_str()) == TRUE);
#else
			return (::fnmatch(pattern.c_str(), str.c_str(), 0) == 0);
#endif
	}

	namespace {

		ETC_TEST_CASE(split)
		{
			{
				ETC_ENFORCE_EQ(split("a/b/c/d").size(), 4);
				ETC_ENFORCE_EQ(split("").size(), 1);
				ETC_ENFORCE_EQ(split("a").size(), 1);
				ETC_ENFORCE_EQ(split("/a").size(), 2);
				ETC_ENFORCE_EQ(split("a/").size(), 1);
				ETC_ENFORCE_EQ(split("/a/").size(), 2);
				ETC_ENFORCE_EQ(split("//a").size(), 2);
				ETC_ENFORCE_EQ(split("//a/").size(), 2);
				ETC_ENFORCE_EQ(split("//a//").size(), 2);
				ETC_ENFORCE_EQ(split("//a//b").size(), 3);
			}

			{
				auto p = split("pif/paf");
				ETC_ENFORCE_EQ(p[0], "pif");
				ETC_ENFORCE_EQ(p[1], "paf");
			}

			{
				auto p = split("../paf/pouf.lol");
				ETC_ENFORCE_EQ(p[0], "..");
				ETC_ENFORCE_EQ(p[1], "paf");
				ETC_ENFORCE_EQ(p[2], "pouf.lol");
			}

			{
				auto p = split("/");
				ETC_ENFORCE_EQ(p[0], "");
			}
		}

		ETC_TEST_CASE(match)
		{
			ETC_ENFORCE(match("*", "lol"));
			ETC_ENFORCE(match("*", ""));
		}
	}

}}
