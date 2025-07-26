#ifndef KINETOXIN_LOCATION_TRACKER_H
#define KINETOXIN_LOCATION_TRACKER_H

#include <windows.h>
#include <locationapi.h>
#include <sensors.h>
#include <string>
#include <vector>

namespace KineToxin {
namespace Core {
namespace Surveillance {

/**
 * @brief Structure to hold location information
 */
struct LocationData {
    double latitude;
    double longitude;
    double altitude;
    double accuracy;
    SYSTEMTIME timestamp;
    std::string source; // "GPS", "WiFi", "Cell", etc.
};

/**
 * @brief Location tracking and geolocation capabilities
 * 
 * This class provides various methods to obtain device location
 * using GPS, Wi-Fi, and other available positioning methods.
 */
class LocationTracker {
public:
    /**
     * @brief Initialize location tracking subsystem
     * @return TRUE if initialization successful, FALSE otherwise
     */
    static BOOL Initialize();
    
    /**
     * @brief Cleanup location tracking resources
     */
    static void Cleanup();
    
    /**
     * @brief Get current location using Windows Location API
     * @param location Output location data
     * @return TRUE if location obtained successfully, FALSE otherwise
     */
    static BOOL GetCurrentLocation(LocationData& location);
    
    /**
     * @brief Get location using Wi-Fi positioning
     * @param location Output location data
     * @return TRUE if location obtained successfully, FALSE otherwise
     */
    static BOOL GetLocationByWiFi(LocationData& location);
    
    /**
     * @brief Get location history from system
     * @param locations Output vector of historical locations
     * @param maxCount Maximum number of locations to retrieve
     * @return TRUE if locations retrieved successfully, FALSE otherwise
     */
    static BOOL GetLocationHistory(std::vector<LocationData>& locations, 
                                  DWORD maxCount = 100);
    
    /**
     * @brief Check if location services are enabled
     * @return TRUE if location services available, FALSE otherwise
     */
    static BOOL IsLocationServiceEnabled();
    
    /**
     * @brief Get network-based approximate location
     * @param location Output location data
     * @return TRUE if location obtained successfully, FALSE otherwise
     */
    static BOOL GetNetworkLocation(LocationData& location);
    
    /**
     * @brief Start continuous location monitoring
     * @param callback Callback function for location updates
     * @param intervalMs Update interval in milliseconds
     * @return TRUE if monitoring started successfully, FALSE otherwise
     */
    static BOOL StartLocationMonitoring(void(*callback)(const LocationData&), 
                                       DWORD intervalMs = 30000);
    
    /**
     * @brief Stop continuous location monitoring
     */
    static void StopLocationMonitoring();

private:
    /**
     * @brief Convert Windows location data to our format
     * @param pLocationReport Windows location report
     * @param location Output location data
     * @return TRUE if conversion successful, FALSE otherwise
     */
    static BOOL ConvertLocationReport(ILocationReport* pLocationReport, 
                                     LocationData& location);
    
    /**
     * @brief Get nearby Wi-Fi access points for positioning
     * @param accessPoints Output list of access points
     * @return TRUE if scan successful, FALSE otherwise
     */
    static BOOL ScanWiFiAccessPoints(std::vector<std::string>& accessPoints);
    
    static BOOL bInitialized;
    static ILocation* pLocation;
    static HANDLE hLocationThread;
    static BOOL bMonitoring;
    static void(*pLocationCallback)(const LocationData&);
    static DWORD dwUpdateInterval;
    
    /**
     * @brief Location monitoring thread function
     * @param lpParam Thread parameter (unused)
     * @return Thread exit code
     */
    static DWORD WINAPI LocationMonitoringThread(LPVOID lpParam);
};

} // namespace Surveillance
} // namespace Core
} // namespace KineToxin

#endif // KINETOXIN_LOCATION_TRACKER_H
