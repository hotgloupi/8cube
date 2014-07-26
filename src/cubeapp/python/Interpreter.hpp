#ifndef  CUBEAPP_PYTHON_INTERPRETER_HPP
# define CUBEAPP_PYTHON_INTERPRETER_HPP

# include <cubeapp/api.hpp>

# include <wrappers/boost/filesystem.hpp>
# include <cube/python.hpp>

# include <string>

namespace cubeapp { namespace python {

	class CUBEAPP_API Interpreter
	{
	private:
		struct Impl;
		friend struct Impl;
		Impl* _impl;

	public:
		void exec(std::string const& script);
		void setglobal(std::string const& key, std::string const& value);
		void setglobal(std::string const& key, boost::python::object const& value);
		boost::python::dict& globals();

	private:
		Interpreter();
	public:
		~Interpreter();

	public:
		static Interpreter& instance(boost::filesystem::path lib_dir);
		static void release();
	};

}} // !app::python

#endif
