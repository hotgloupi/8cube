#ifndef ETC_TEMP_FILE_HPP
# define ETC_TEMP_FILE_HPP

# include "default_directory.hpp"

# include <etc/log.hpp>

# include <wrappers/boost/filesystem.hpp>

# include <iosfwd>

namespace etc { namespace temp {

	template<typename Stream>
	struct File
	{
		ETC_LOG_COMPONENT("etc.temp.File");
	public:
		typedef boost::filesystem::path path_t;
		typedef Stream stream_t;

	private:
		path_t       _dir;
		std::string  _pattern;
		path_t       _path;
		stream_t     _stream;


	public:
		File(path_t const& dir,
		     std::string const& pattern,
		     std::ios_base::openmode openmode)
			: _dir{dir}
			, _pattern{pattern}
			, _path(_gen_unique_path())
			, _stream{_path.string(), openmode}
		{}

		File(std::string const& pattern,
		     std::ios_base::openmode openmode)
			: File{default_directory(), pattern, openmode}
		{}

		File()
			: File{default_directory(), std::string{}}
		{}

		~File()
		{
			_stream.close();
			ETC_TRACE.debug("Closing: ", _path);
			boost::system::error_code ec;
			boost::filesystem::remove(_path, ec);
			if (ec)
				ETC_TRACE.error("Couldn't remove temporary file ", _path);
		}
	public:
		path_t const& path() const { return _path; }
		stream_t& stream() { return _stream; }

	private:
		path_t _gen_unique_path()
		{
			boost::filesystem::path p;
			do {
				if (_pattern.empty())
					p = _dir / boost::filesystem::unique_path();
				else
					p = _dir / boost::filesystem::unique_path(_pattern);
			} while (boost::filesystem::exists(p));
			return p;
		}
	};

}}

#endif
