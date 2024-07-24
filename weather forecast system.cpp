#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Options.hpp>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

// Location class
class Location {
public:
	string name;
	double latitude;
	double longitude;

	Location(const string& n, double lat, double lon) : name(n), latitude(lat), longitude(lon) {}
};

// WeatherForecastingSystem class
class WeatherForecastingSystem {
private:
	string apiKey; // Your OpenWeatherMap API key

public:
	WeatherForecastingSystem(const string& api_key) : apiKey(api_key) {}

	void fetchWeatherForecast(const Location& location) {
		string apiUrl = "https://api.openweathermap.org/data/2.5/onecall?";
		apiUrl += "lat=" + to_string(location.latitude);
		apiUrl += "&lon=" + to_string(location.longitude);
		apiUrl += "&exclude=minutely,hourly,daily,alerts"; // Exclude unnecessary parts
		apiUrl += "&appid=" + apiKey;

		try {
			curlpp::Cleanup cleanup;
			curlpp::Easy request;
			request.setOpt(new curlpp::options::Url(apiUrl));

			ostringstream responseStream;
			curlpp::options::WriteStream writeStream(&responseStream);
			request.setOpt(writeStream);

			cout << "Fetching weather forecast for: " << location.name << endl;
			request.perform();

			// Parse JSON response
			json jsonResponse = json::parse(responseStream.str());

			// Print out specific weather details (example: current temperature)
			double temperature = jsonResponse["current"]["temp"];
			cout << "Temperature: " << temperature << " K" << endl; // Temperature is in Kelvin by default
		}
		catch (curlpp::RuntimeError& e) {
			cerr << "Runtime error: " << e.what() << endl;
		}
		catch (curlpp::LogicError& e) {
			cerr << "Logic error: " << e.what() << endl;
		}
		catch (json::parse_error& e) {
			cerr << "JSON parse error: " << e.what() << endl;
		}
	}
};

// HistoricalWeatherSystem class
class HistoricalWeatherSystem {
private:
	string apiKey; // Your OpenWeatherMap API key

public:
	HistoricalWeatherSystem(const string& api_key) : apiKey(api_key) {}

	void fetchHistoricalWeather(const Location& location, const string& startDt, const string& endDt) {
		string apiUrl = "https://api.openweathermap.org/data/2.5/onecall/timemachine?";
		apiUrl += "lat=" + to_string(location.latitude);
		apiUrl += "&lon=" + to_string(location.longitude);
		apiUrl += "&start=" + startDt; // Start date in UNIX timestamp or "YYYY-MM-DDTHH:MM:SSZ" format
		apiUrl += "&end=" + endDt;     // End date in UNIX timestamp or "YYYY-MM-DDTHH:MM:SSZ" format
		apiUrl += "&appid=" + apiKey;

		try {
			curlpp::Cleanup cleanup;
			curlpp::Easy request;
			request.setOpt(new curlpp::options::Url(apiUrl));

			ostringstream responseStream;
			curlpp::options::WriteStream writeStream(&responseStream);
			request.setOpt(writeStream);

			cout << "Fetching historical weather data for: " << location.name << endl;
			request.perform();

			// Parse JSON response
			json jsonResponse = json::parse(responseStream.str());

			// Display historical weather data
			for (const auto& entry : jsonResponse["hourly"]) {
				string timestamp = entry["dt"];
				double temperature = entry["temp"];
				cout << "Timestamp: " << timestamp << ", Temperature: " << temperature << " K" << endl;
			}
		}
		catch (curlpp::RuntimeError& e) {
			cerr << "Runtime error: " << e.what() << endl;
		}
		catch (curlpp::LogicError& e) {
			cerr << "Logic error: " << e.what() << endl;
		}
		catch (json::parse_error& e) {
			cerr << "JSON parse error: " << e.what() << endl;
		}
	}
};

// LocationManager class
class LocationManager {
private:
	vector<Location> locations;

public:
	void addLocation(const string& name, double latitude, double longitude) {
		locations.push_back(Location(name, latitude, longitude));
	}

	void removeLocation(const string& name) {
		locations.erase(remove_if(locations.begin(), locations.end(),
			[&](const Location& loc) { return loc.name == name; }), locations.end());
	}

	void listLocations() const {
		for (const auto& loc : locations) {
			cout << "Name: " << loc.name << ", Latitude: " << loc.latitude << ", Longitude: " << loc.longitude << endl;
		}
	}

	const vector<Location>& getLocations() const {
		return locations;
	}
};

int main() {
	// Initialize LocationManager
	LocationManager locationManager;
	locationManager.addLocation("New York", 40.7128, -74.0060);
	locationManager.addLocation("Los Angeles", 34.0522, -118.2437);

	// Initialize WeatherForecastingSystem with your OpenWeatherMap API key
	string apiKey = "b8397a36a0d76746dccbb4ee7c3c2e8b";
	WeatherForecastingSystem forecastingSystem(apiKey);

	// Initialize HistoricalWeatherSystem with your OpenWeatherMap API key
	HistoricalWeatherSystem historicalSystem(apiKey);

	int choice;
	string startDt, endDt;

	do {
		// Display menu
		cout << "\nMenu:\n";
		cout << "1. List Locations\n";
		cout << "2. Fetch Current Weather Forecast\n";
		cout << "3. Fetch Historical Weather Data\n";
		cout << "4. Exit\n";
		cout << "Enter your choice: ";
		cin >> choice;

		switch (choice) {
		case 1:
			cout << "List of Locations:" << endl;
			locationManager.listLocations();
			break;
		case 2:
			for (const auto& location : locationManager.getLocations()) {
				forecastingSystem.fetchWeatherForecast(location);
			}
			break;
		case 3:
			cout << "Enter start date (YYYY-MM-DDTHH:MM:SSZ): ";
			cin >> startDt;
			cout << "Enter end date (YYYY-MM-DDTHH:MM:SSZ): ";
			cin >> endDt;
			for (const auto& location : locationManager.getLocations()) {
				historicalSystem.fetchHistoricalWeather(location, startDt, endDt);
			}
			break;
		case 4:
			cout << "Exiting program.\n";
			break;
		default:
			cout << "Invalid choice. Please enter a valid option.\n";
			break;
		}

	} while (choice != 4);

	return 0;
}
