#include <etc/print.hpp>

int main()
{
	etc::print("Salut", etc::iomanip::nosep(), 12, etc::iomanip::nonewline());
	etc::print(" _>", '+');
	etc::printf("Salut les %f copains\n", 12);
}
