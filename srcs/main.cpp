
#include <cstdlib>
#include <iostream>

#include "nw_addr.hpp"
#include "nw_addrinfo.hpp"
#include "nw_protoent.hpp"
#include "nw_buffer.hpp"

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <sys/socket.h>

int	main(int ac, char *av[]) try {
	//socket

	//sendmsg
	//recvmsg

	//send
	//recv

	//writev
	//readv

	//write
	//read

//	int fd = socket(AF_INET, SOCK_STREAM, 6);
//
//	struct sockaddr_in addr = {
//		AF_INET,
//		htons(80),
//		{0},
//		{0}
//	};
//
//	inet_aton("192.168.1.9", &addr.sin_addr);
//
//	if (!bind(fd, (struct sockaddr *)&addr, sizeof(addr))) {
//		if (!listen(fd, 10)) {
//			while(1);
//		}
//	}
//
//	connect(fd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr));
//
//	nw::buffer<30>	b(nw::buffer<30>::dir::in, [fd](void *b, nw::size_type n){
//			return read(fd, b, n);;
//		});
//
//	char c;
//	while (!b.eof()) {
//		std::cout << "================================" << std::endl;
//		b.sync();
//		std::cout << b << std::endl;
//		b.getn(&c, 1);
//		std::cout << b << std::endl;
//	}
//
//	b.clear();

//	std::cout << nw::addrinfo<nw::sock_use::BIND, nw::sa_family::INET>("192.168.1.9", "80", "tcp") << std::endl;
	std::cout << nw::addrinfo<nw::sa_family::INET6V4M>("google.fr", "80", "tcp") << std::endl;

	return EXIT_SUCCESS;
	static_cast<void>(ac);
	static_cast<void>(av);
} catch (const std::exception &e) {
	std::cerr << "Excpetion: " << e.what() << std::endl;
	return EXIT_FAILURE;
}
