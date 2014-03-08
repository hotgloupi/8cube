#ifndef  LAUNCH_CUBE_VERSION_HPP
# define LAUNCH_CUBE_VERSION_HPP

struct version
{
	int major, minor, patch;

	version(int M, int m, int p) : major(M), minor(m), patch(p) {}
	version() : major(0), minor(0), patch(0) {}
	version(version const&) = default;
	version& operator =(version const&) = default;

	bool operator ==(version const& rhs) const
	{
	    return major == rhs.major && minor == rhs.minor && patch == rhs.patch;
	}

	bool operator <(version const& rhs) const
	{
		return (
			major < rhs.major
			|| (major == rhs.major && minor < rhs.minor)
			|| (major == rhs.major && minor == rhs.minor && patch < rhs.patch)
		);
	}
};

#endif
