#include <cpprest/http_client.h>
#include <cpprest/filestream.h>

using namespace utility;                    // Common utilities like string conversions
using namespace web;                        // Common features like URIs.
using namespace web::http;                  // Common HTTP functionality
using namespace web::http::client;          // HTTP client features
using namespace concurrency::streams;       // Asynchronous streams

class GeocodeGrabber {
private:
	bool GetLongLatFromAddress(std::string address) {
		// Create http_client to send the request.
		http_client client(U("https://maps.googleapis.com/maps/api/geocode/json"));

		// Build request URI and start the request.
		uri_builder builder(U(""));
		builder.append_query(U("address"), address);
		client.request(methods::GET, builder.to_string());
	}

public:

};