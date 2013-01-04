#ifndef  CUBE_GL_CAMERA_HPP
# define CUBE_GL_CAMERA_HPP

# include "vector.hpp"

namespace cube { namespace gl { namespace camera {

	struct Camera
	{
		vector::Vector3f position;
		vector::Vector3f front;
		vector::Vector3f up;

		Camera(vector::Vector3f const& position,
		       vector::Vector3f const& front,
		       vector::Vector3f const& up)
			: position{position}
			, front{front}
			, up{up}
		{}

		inline
		vector::Vector3f right() const
		{
			// XXX This is left !
			return vector::cross(this->front, this->up);
		}
	};

}}}

#endif

