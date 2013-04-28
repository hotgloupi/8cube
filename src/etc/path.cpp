#include "path.hpp"

#include <boost/filesystem.hpp>

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

}}
