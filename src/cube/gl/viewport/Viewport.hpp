#ifndef  CUBE_GL_VIEWPORT_VIEWPORT_HPP
# define CUBE_GL_VIEWPORT_VIEWPORT_HPP

# include <cube/api.hpp>
# include <cube/gl/rectangle.hpp>

namespace cube { namespace gl { namespace viewport {

	struct CUBE_API Viewport
		: public rectangle::Rectanglef
	{
		inline
		Viewport(float x, float y, float w, float h) ETC_NOEXCEPT
			: rectangle::Rectanglef{x, y, w, h}
		{}

		inline
		Viewport(Viewport const& other) ETC_NOEXCEPT
			: rectangle::Rectanglef{other}
		{}

		inline
		Viewport& operator =(Viewport const& other) ETC_NOEXCEPT
		{
			this->rectangle::Rectanglef::operator =(other);
			return *this;
		}
	};

	CUBE_API
	std::ostream& operator <<(std::ostream& out, Viewport const& vp);

}}}

#endif /* ! VIEWPORT_HPP */


