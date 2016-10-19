#include <string>
#include <fstream>
#include <istream>
#include <iostream>

#include <cpprest/http_client.h>
#include <cpprest/filestream.h>

using namespace utility;                    // Common utilities like string conversions
using namespace web;                        // Common features like URIs.
using namespace web::http;                  // Common HTTP functionality
using namespace web::http::client;          // HTTP client features
using namespace concurrency::streams;       // Asynchronous streams

class GeocodeGrabber {
private:
	const std::string api_key = "AIzaSyD-NPqot8WGQyK0GtcrkMasHPIzKHB-HTo";

	bool GetLongLatFromAddress(std::string address) {
		// Create http_client to send the request.
		http_client client(U("https://maps.googleapis.com/maps/api/geocode/json"));

		// Build request URI and start the request.
		uri_builder builder(U(""));
		builder.append_query(U("address"), U("something"));
		client
			.request(methods::GET, builder.to_string())
			// continue when the response is available
			.then([](http_response response) -> pplx::task <json::value>
		{
			// if the status is OK extract the body of the response into a JSON value
			// works only when the content type is application\json
			if (response.status_code() == status_codes::OK)
			{
				return response.extract_json();
			}

			// return an empty JSON value
			return pplx::task_from_result(json::value());
		})
			// continue when the JSON value is available
			.then([](pplx::task<json::value> previousTask)
		{
			// get the JSON value from the task and display content from it
			try
			{
				json::value const & v = previousTask.get();
				// do something with extracted value
			}
			catch (http_exception const & e)
			{
				std::cout << e.what() << std::endl;
			}
		})
			.wait();
	}

public:

};