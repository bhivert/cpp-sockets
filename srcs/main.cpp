
#include <cstdlib>

int	main(int ac, char *av[]) try {
	static_cast<void>(ac);
	static_cast<void>(av);
	return EXIT_SUCCESS;
} catch (...) {
	return EXIT_FAILURE;
}
