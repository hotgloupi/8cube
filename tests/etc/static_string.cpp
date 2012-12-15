
#include <etc/static_string.hpp>

#include <iostream>

template<unsigned int size>
void print(etc::StaticString<size> const& s)
{
	std::cout << s << std::endl;
}

int main()
{
	etc::StaticString<4> s1{"pif"};
	etc::StaticString<4> s2 = "paf";
	char const tab[] = "salut les coupains";
	print(etc::static_string(tab));
	std::cout << s1 << ' ' << s2 << std::endl;
}
