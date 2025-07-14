#include <network/HttpClientAdapter.h>

void HttpClientAdapter::sendHeartbeatTask(int port)
{
	if (_client == nullptr)
	{
		_client = std::make_unique<httplib::Client>("127.0.0.1", port);
	}
	 _client->Get("/health");
}

HttpClientAdapter::HttpClientAdapter()
{

}