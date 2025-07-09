#include <network/HttpClientAdapter.h>

void HttpClientAdapter::sendHeartbeatTask(int port)
{
	httplib::Client client("127.0.0.1", port);
	client.Get("/health");
}

HttpClientAdapter::HttpClientAdapter()
{

}