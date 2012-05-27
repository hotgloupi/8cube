#ifndef  APP_PYTHON_INTERPRETER_HPP
# define APP_PYTHON_INTERPRETER_HPP

# include <string>

namespace app { namespace python {

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
		static Interpreter& instance();
	};

}} // !app::python

#endif
