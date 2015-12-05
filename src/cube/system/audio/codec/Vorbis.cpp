#define STB_VORBIS_HEADER_ONLY
#include "detail/stb_vorbis.c"

#include "Vorbis.hpp"
#include <cube/system/audio/Exception.hpp>

#include <etc/memory.hpp>
#include <etc/log.hpp>

namespace cube { namespace system { namespace audio { namespace codec {

	ETC_LOG_COMPONENT("cube.system.audio.codec.Vorbis");

	namespace {
		struct Vorbis;
		std::ostream& operator <<(std::ostream& out, Vorbis const&);

		struct Vorbis : public Codec
		{
			::stb_vorbis* vorbis;
			::stb_vorbis_info info;

			Vorbis(boost::filesystem::path const& file)
				: vorbis(nullptr)
			{
				int err;
				this->vorbis = ::stb_vorbis_open_filename(file.c_str(), &err, nullptr);
				if (this->vorbis == nullptr)
					throw Exception("Couldn't create a Vorbis codec for '" +
									file.string() + "'");
				this->info = ::stb_vorbis_get_info(this->vorbis);
				ETC_TRACE_CTOR("for", file);
			}

			~Vorbis()
			{
				ETC_TRACE_DTOR();
				::stb_vorbis_close(this->vorbis);
			}

			unsigned sample_rate() const final { return this->info.sample_rate; }
			unsigned channels() const final { return this->info.channels; }
			unsigned max_frames() const final { return this->info.max_frame_size; }

			size_t read(std::vector<float*>& samples) final
			{
				int channels;
				float** output;
				int sample_size = ::stb_vorbis_get_frame_float(this->vorbis,
				                                               &channels,
				                                               &output);
				if (sample_size == 0)
					return 0;
				samples.resize(channels);
				for (int i = 0; i < channels; ++i)
					samples[i] = output[i];

				return sample_size;
			}
		};

		std::ostream& operator <<(std::ostream& out, Vorbis const& codec)
		{
			return out << "<VorbisCodec(rate=" << codec.info.sample_rate
			           << ", channels=" << codec.info.channels
			           << ") at " << &codec << ">";
		}
	}


	CodecPtr make_vorbis_codec(boost::filesystem::path const& file)
	{
		return etc::make_unique<Vorbis>(file);
	}

}}}}

#undef STB_VORBIS_HEADER_ONLY
#include "detail/stb_vorbis.c"
