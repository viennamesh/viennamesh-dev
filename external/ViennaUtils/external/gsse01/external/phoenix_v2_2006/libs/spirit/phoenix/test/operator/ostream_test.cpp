#include <iostream>
#include <boost/spirit/phoenix/core.hpp>
#include <boost/spirit/phoenix/operator.hpp>
#include <boost/fusion/sequence.hpp>

int main()
{
	using namespace boost::phoenix;
	int i = 4;
	(std::cout << arg1 * 2 << std::endl) (i);
	return 0;
}
