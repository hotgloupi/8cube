#ifndef  CUBEAPP_PYTHON_INTERPRETER_HPP
# define CUBEAPP_PYTHON_INTERPRETER_HPP

# include <wrappers/boost/filesystem.hpp>


# include <string>

namespace cubeapp { namespace python {

	class Interpreter
	{
	private:
		struct Impl;
		friend struct Impl;
		Impl* _impl;

	public:
		bool exec(std::string const& script);
		void setglobal(std::string const& key, std::string const& value);

	private:
		Interpreter();
		~Interpreter();

	public:
		static Interpreter& instance(boost::filesystem::path lib_dir);
	};

}} // !app::python

#endif
