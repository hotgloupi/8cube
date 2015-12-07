#pragma once

#include <cube/api.hpp>
#include <etc/printable.hpp>

#include <boost/filesystem/path.hpp>

#include <memory>

namespace cube { namespace system { namespace audio {

	class CUBE_API Codec
		: public etc::Printable
	{

	public:
		virtual ~Codec();

	public:
		virtual unsigned sample_rate() const = 0;
		virtual unsigned channels() const = 0;
		virtual unsigned max_frames() const = 0;

	public:
		virtual size_t read(std::vector<float*>& samples) = 0;


	public:
		void print(std::ostream& out) const ETC_NOEXCEPT override;
	};

	typedef std::unique_ptr<Codec> CodecPtr;

	CodecPtr make_codec(boost::filesystem::path const& path);

}}}
