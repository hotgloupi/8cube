#include "pretty_type.hpp"
#include "Node.hpp"

#include <etc/log.hpp>
#include <etc/assert.hpp>

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/fusion/adapted/struct.hpp>
#include <boost/algorithm/string.hpp>

#include <string>

namespace qi = boost::spirit::qi;

namespace {

	ETC_LOG_COMPONENT("cube.scene.node");

	struct cxxtype
	{
		std::string name;
		typedef std::vector<cxxtype> args_type;
		args_type args;
	};

	std::ostream& operator << (std::ostream& out, cxxtype const& v)
	{
		out << v.name;
		if (v.args.empty())
			return out;
		out << '{';
		auto it = v.args.begin(),
			 end = v.args.end();
		out << *it++;
		for (; it != end; ++it)
			out << ", " << *it;
		return out << '}';
	}
}

	BOOST_FUSION_ADAPT_STRUCT(
		cxxtype,
		(std::string, name)(cxxtype::args_type, args)
	);

namespace {

	template <typename Iterator>
	struct type_parser
		: qi::grammar<Iterator, cxxtype(), qi::ascii::space_type>
	{
		qi::rule<Iterator, cxxtype(), qi::ascii::space_type> type_rule;
		qi::rule<Iterator, std::string(), qi::ascii::space_type> name_part;
		qi::rule<Iterator, std::string(), qi::ascii::space_type> name;
		qi::rule<Iterator, std::string(), qi::ascii::space_type> namespace_sep;

		type_parser()
			: type_parser::base_type(type_rule)
		{
			using boost::phoenix::at_c;
			using boost::phoenix::push_back;

			this->name_part = qi::lexeme[+(qi::char_ - ':' - '<' - ',' - '>')[ qi::_val += qi::_1 ]];
			this->namespace_sep = qi::string("::") [qi::_val += qi::_1 ];
			this->name = name_part >> *(namespace_sep >> name_part);
			this->type_rule =
				this->name [at_c<0>(qi::_val) = qi::_1]
				>> -(
					'<' >> *(type_rule % ',') [at_c<1>(qi::_val) = qi::_1] >> '>'
				)
			;
		}
	};

	std::string clean_type(cxxtype const& v)
	{
		if (v.name == "std::shared_ptr")
		{
			ETC_ASSERT_EQ(v.args.size(), 1u);
			return clean_type(v.args[0]);
		}

		std::string res = v.name;
		auto p = res.find_last_of("::");
		if (p != std::string::npos)
			res = res.substr(p + 1, std::string::npos);
		boost::algorithm::erase_all(res, " const");
		boost::algorithm::erase_all(res, "*");
		boost::algorithm::erase_all(res, "&");
		if (!v.args.empty())
		{
			res.push_back('<');
			auto it = v.args.begin(),
			     end = v.args.end();
			res.append(clean_type(*it++));
			for (; it != end; ++it)
			{
				res.append(", ");
				res.append(clean_type(*it));
			}
			res.push_back('>');
		}
		return res;
	}

}

namespace cube { namespace scene { namespace node {

	std::string pretty_type(std::string const& input)
	{
		type_parser<std::string::const_iterator> parser;
		cxxtype type_ast;
		bool res = qi::phrase_parse(
			input.begin(),
			input.end(),
			parser,
			qi::ascii::space,
			type_ast
		);
		if (!res)
		{
			ETC_LOG.warn("Couldn't parse", input);
			return input;
		}
		return clean_type(type_ast);
	}

	CUBE_API std::string pretty_type(Node const& node)
	{ return pretty_type(ETC_TYPE_STRING(node)); }

}}}
