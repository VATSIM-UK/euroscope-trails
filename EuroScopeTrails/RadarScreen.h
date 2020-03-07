#pragma once
#include "EuroScopePlugIn.h"
#include "AircraftHistoryTrail.h"

/*
    Class for displaying to the default EuroScope Radar Screen.

    This class handles the rendering of the various modules and screen objects.
    It also contains the logic for what to do when objects on the screen are
    clicked or moved - which involves saving data to the loaded ASR file. In the vast majority of
    cases, this is delegated to other classes.
*/
class RadarScreen : public EuroScopePlugIn::CRadarScreen
{
public:
    RadarScreen(const std::map<std::string, std::unique_ptr<AircraftHistoryTrail>>& trails);
    ~RadarScreen(void);
    void OnAsrContentToBeClosed(void);
    void OnAsrContentToBeSaved(void);
    void OnAsrContentLoaded(bool loaded);
    bool OnCompileCommand(const char* sCommandLine);
    void OnRefresh(HDC hdc, int phase);
    bool PositionOffScreen(EuroScopePlugIn::CPosition pos);

private:

    void DrawDot(
        Gdiplus::Graphics& graphics,
        Gdiplus::Pen& pen,
        const Gdiplus::RectF& area
    );

    bool HasAsrKey(const char* key);

    // Is the ASR Loaded
    bool asrLoaded = false;

    // The colour to draw the trails with (or just the first colour, if fading)
    Gdiplus::Color startColour;

    // The pen with which to draw the trails
    Gdiplus::Pen pen;

    const std::map<std::string, std::unique_ptr<AircraftHistoryTrail>>& trails;

    // Visibility ASR settings
    const char* visibleUserSettingKey = "HistoryTrailDisplay";

    const char* visibleUserSettingDescription = "Display History Trails";

    // Trail type ASR settings
    const char* trailTypeUserSettingKey = "HistoryTrailType";

    const char* trailTypeUserSettingDescription = "History Trail Type";

    // Dot size ASR settings
    const char* dotSizeUserSettingKey = "HistoryTrailDotSize";

    const char* dotSizeUserSettingDescription = "History Trail Dot Size";

    // Degrading ASR settings
    const char* degradingUserSettingKey = "HistoryTrailDegrade";

    const char* degradingUserSettingDescription = "History Trail Degrade";

    // Fading ASR settings
    const char* fadingUserSettingKey = "HistoryTrailFade";

    const char* fadingUserSettingDescription = "History Trail Fade";

    // Filled ASR settings
    const char* filledUserSettingKey = "HistoryTrailDotFilled";

    const char* filledUserSettingDescription = "History Trail Filled";

    // AA ASR settings
    const char* antialiasUserSettingKey = "HistoryTrailAntiAlias";

    const char* antialiasUserSettingDescription = "History Trail Antialias";

    // Trail length ASR settings
    const char* trailLengthUserSettingKey = "HistoryTrailLength";

    const char* trailLengthUserSettingDescription = "History Trail Length";

    // Trail colour ASR settings
    const char* trailColourUserSettingKey = "HistoryTrailColour";

    const char* trailColourUserSettingDescription = "History Trail Colour";

    // Min altitude filter settings
    const char* minAltitudeFilterUserSettingKey = "HistoryTrailMinAltitudeFilter";
    const char* minAltitudeFilterUserSettingDescription = "Minimum History Trail Altitude";

    // Max altitude filter settings
    const char* maxAltitudeFilterUserSettingKey = "HistoryTrailMaxAltitudeFilter";
    const char* maxAltitudeFilterUserSettingDescription = "Maximum History Trail Altitude";

    // The module menu text
    const char* menuItemDescription = "Configure History Trails";

    // The minimum groundspeed to display history trails for.
    const int minimumSpeed = 50;

    // Diamond trail type
    const int trailTypeDiamond = 0;

    // Square trail type
    const int trailTypeSquare = 1;

    // Circle trail type
    const int trailTypeCircle = 2;

    // Default minimum altitude for altitude filter
    const int defaultMinAltitude = 0;

    // Default max altitude for altitude filter
    const int defaultMaxAltitude = 99999;

    // Whether or not we should render the trails.
    bool visible;

    // The type of history trail.
    int historyTrailType;

    // History trail dotsize as a float
    float historyTrailDotSizeFloat;

    // The size of each history trail dot
    int historyTrailDotSize;

    // Whether trails should degrade over time
    bool degradingTrails;

    // Whether or not trails should fade over time
    bool fadingTrails;

    // Whether or not trails should be filled with the color
    bool filledTrails;

    // Whether or not trails should use antialiasing
    bool antialiasedTrails;

	// The length of trail to render
    int historyTrailLength;

    // The amount of alpha to reduce per dot
    int alphaPerDot;

    // The altitude at and below which not to display
    int minimumDisplayAltitude;

    // The altitude at and above which not to display
    int maximumDisplayAltitude;
};
