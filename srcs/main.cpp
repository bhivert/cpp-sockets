
#include <cstdlib>
#include <iostream>

//#include "nw_addr.hpp"
//#include "nw_addrinfo.hpp"
//#include "nw_protoent.hpp"
#include "buffer/nw_ibuffer.hpp"
#include "buffer/nw_obuffer.hpp"

//#include <unistd.h>
//#include <sys/types.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <arpa/inet.h>
//
//#include <sys/socket.h>

#include "nw_socket.hpp"

int	main(int ac, char *av[]) try {
	//close

	//setsockopt

	//recv

	//sendto
	//recvfrom

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
//	connect(fd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr));
//
//	nw::buffer<30>	b(nw::buffer<30>::dir::in, [fd](void *b, nw::size_type n){
//			return read(fd, b, n);;
//		});
//
//	char c[31] = {0};
//	while (!b.eof()) {
//		std::cout << "================================" << std::endl;
//		b.sync();
////		std::cout << b << std::endl;
//		nw::size_type bc = b.in_avail();
//		b.getn(c, bc);
//		c[bc] = '\0';
//		std::cout << c;
//		std::cout << b << std::endl;
//	}
//
//	b.clear();

//	nw::ibuffer<38> i;

//	i.putn("Hello world !", 13);
//	std::cout << i << std::endl;

	nw::obuffer<38> o;
//	std::cout << (o << 0xFFFFFF00) << std::endl;

//	std::cout << nw::addrinfo<nw::sa_family::INET>("80", "google.fr", "tcp") << std::endl;
//	std::cout << nw::addrinfo<nw::sa_family::INET6V4M>("80", "google.fr", "tcp") << std::endl;
//	std::cout << nw::addrinfo<nw::sa_family::UNSPEC>("80", "google.fr", 0, nw::sock_type::UNSPEC) << std::endl;

	nw::socket<nw::sa_family::INET, nw::sock_type::STREAM> c("tcp");
	c.connect({80, "127.0.0.1"});

	o << "Hello world !" << '\n';
	std::cout << o << std::endl;
	std::cout << c << std::endl;
	std::cout << "=====" << std::endl;
	c.send(o);
	std::cout << c << std::endl;
	std::cout << "=====" << std::endl;
	c.close();
	std::cout << c << std::endl;
	std::cout << "=====" << std::endl;
	while(1) {}


//	nw::socket<nw::sa_family::INET, nw::sock_type::STREAM>	so("tcp");
//	std::cout << so << std::endl;
//	so.bind(80);
//	std::cout << so << std::endl;
//	so.listen(10);
//	std::cout << so << std::endl;
//	std::cout << "=====" << std::endl;
//	nw::socket<nw::sa_family::INET, nw::sock_type::STREAM> so0 = so.accept();
//	std::cout << so0 << std::endl;
//	std::cout << "=====" << std::endl;
//	so0.send(o);
//	so0.close();
//	std::cout << so0 << std::endl;
//	std::cout << "=====" << std::endl;
//	nw::socket<nw::sa_family::INET, nw::sock_type::STREAM> so1 = so.accept();
//	std::cout << so1 << std::endl;
//	std::cout << "=====" << std::endl;
//	nw::socket<nw::sa_family::INET6, nw::sock_type::STREAM> so2 = so.accept();
//	std::cout << so2 << std::endl;


//	nw::socket<nw::sa_family::UNSPEC, nw::sock_type::STREAM>	sc = std::move(so);
//	std::cout << so << std::endl;
//	std::cout << sc << std::endl;
//
//	nw::socket<nw::sa_family::INET6, nw::sock_type::STREAM>	sc0 = std::move(sc);
//	std::cout << sc << std::endl;
//	std::cout << sc0 << std::endl;





//	std::cout << SO_REUSEADDR << std::endl;

	return EXIT_SUCCESS;
	static_cast<void>(ac);
	static_cast<void>(av);
} catch (const std::exception &e) {
	std::cerr << "Excpetion: " << e.what() << std::endl;
	return EXIT_FAILURE;
}
