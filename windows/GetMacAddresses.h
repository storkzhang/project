#include "stdafx.h"
#include <Winsock2.h>
#include <Iphlpapi.h>
#include <string>
#include <vector>
#pragma comment(lib,"Iphlpapi")
#pragma warning( disable : 4996 )
#define MACSESION 6
namespace {
	typedef std::vector< std::string > MACAddresses;
	typedef MACAddresses::iterator vsIt;
}
class GetMacAddresses{
public:
	static MACAddresses GetINETMacAddresses();
};