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
