#include "stdafx.h"
#include "RadarScreen.h"
#include "HistoryTrailData.h"
#include "HistoryTrailDialog.h"

RadarScreen::RadarScreen(
    const std::map<std::string, std::unique_ptr<AircraftHistoryTrail>> & trails
) : trails(trails), startColour(255, 255, 130, 20), pen(this->startColour)
{
}

RadarScreen::~RadarScreen(void)
{
    this->OnAsrContentToBeSaved();
}

/*
    Has to be defined, because EuroScope. Have to call delete on current object
    in order to trigger last minute ASR content saving.
*/
void RadarScreen::OnAsrContentToBeClosed(void)
{
    delete this;
}

/*
    ASR closing, save data.
*/
void RadarScreen::OnAsrContentToBeSaved(void)
{
    this->SaveDataToAsr(
        this->visibleUserSettingKey,
        this->visibleUserSettingDescription,
        this->visible ? "1" : "0"
    );
    this->SaveDataToAsr(
        this->trailTypeUserSettingKey,
        this->trailTypeUserSettingDescription,
        std::to_string(this->historyTrailType).c_str()
    );
    this->SaveDataToAsr(
        this->dotSizeUserSettingKey,
        this->dotSizeUserSettingDescription,
        std::to_string(this->historyTrailDotSize).c_str()
    );
    this->SaveDataToAsr(
        this->degradingUserSettingKey,
        this->degradingUserSettingDescription,
        this->degradingTrails ? "1" : "0"
    );
    this->SaveDataToAsr(
        this->fadingUserSettingKey,
        this->fadingUserSettingDescription,
        this->fadingTrails ? "1" : "0"
    );
    this->SaveDataToAsr(
        this->trailLengthUserSettingKey,
        this->trailLengthUserSettingDescription,
        std::to_string(this->historyTrailLength).c_str()
    );

    COLORREF colour = this->startColour.ToCOLORREF();
    std::string colourString = std::to_string(GetRValue(colour)) + "," +
        std::to_string(GetGValue(colour)) + "," + std::to_string(GetBValue(colour));

    this->SaveDataToAsr(
        this->trailColourUserSettingKey,
        this->trailColourUserSettingDescription,
        colourString.c_str()
    );
    this->SaveDataToAsr(
        this->antialiasUserSettingKey,
        this->antialiasUserSettingDescription,
        this->antialiasedTrails ? "1" : "0"
    );
    this->SaveDataToAsr(
        this->minAltitudeFilterUserSettingKey,
        this->minAltitudeFilterUserSettingDescription,
        std::to_string(this->minimumDisplayAltitude).c_str()
    );
    this->SaveDataToAsr(
        this->maxAltitudeFilterUserSettingKey,
        this->maxAltitudeFilterUserSettingDescription,
        std::to_string(this->maximumDisplayAltitude).c_str()
    );
}

/*
    Called by Euroscope for each RadarScreen instance once the data from
    the ASR file has been successfully loaded.
*/
void RadarScreen::OnAsrContentLoaded(bool loaded)
{
    this->visible = this->HasAsrKey(this->visibleUserSettingKey) 
        ? strcmp(this->GetDataFromAsr(this->visibleUserSettingKey), "1") == 0
        : true;

    this->historyTrailType = this->HasAsrKey(this->trailTypeUserSettingKey)
        ? std::stoi(this->GetDataFromAsr(this->trailTypeUserSettingKey))
        : this->trailTypeDiamond;

    this->historyTrailDotSize = this->HasAsrKey(this->dotSizeUserSettingKey)
        ? std::stoi(this->GetDataFromAsr(this->dotSizeUserSettingKey))
        : 4;
    this->historyTrailDotSizeFloat = static_cast<float>(this->historyTrailDotSize);

    this->degradingTrails = this->HasAsrKey(this->degradingUserSettingKey)
        ? strcmp(this->GetDataFromAsr(this->degradingUserSettingKey), "1") == 0
        : true;

    this->fadingTrails = this->HasAsrKey(this->fadingUserSettingKey)
        ? strcmp(this->GetDataFromAsr(this->fadingUserSettingKey), "1") == 0
        : true;

    this->historyTrailLength = this->HasAsrKey(this->trailLengthUserSettingKey)
        ? std::stoi(this->GetDataFromAsr(this->trailLengthUserSettingKey))
        : 15;

    this->antialiasedTrails = this->HasAsrKey(this->antialiasUserSettingKey)
        ? strcmp(this->GetDataFromAsr(this->antialiasUserSettingKey), "1") == 0
        : true;

    std::string colourString = this->HasAsrKey(this->trailColourUserSettingKey)
        ? this->GetDataFromAsr(this->trailColourUserSettingKey)
        : "255,130,20";

    std::istringstream ss(colourString);
    std::vector<std::string> tokens;
    std::string token;

    while (std::getline(ss, token, ',')) {
        if (token.compare("") == 0) {
            continue;
        }

        tokens.push_back(token);
    }

    COLORREF settingColour RGB(std::stoi(tokens[0]), std::stoi(tokens[1]), std::stoi(tokens[2]));

    this->startColour.SetFromCOLORREF(settingColour);
    this->alphaPerDot = 255 / this->historyTrailLength;

    this->minimumDisplayAltitude = this->HasAsrKey(this->minAltitudeFilterUserSettingKey)
        ? std::stoi(this->GetDataFromAsr(this->minAltitudeFilterUserSettingKey))
        : this->defaultMinAltitude;

    this->maximumDisplayAltitude = this->HasAsrKey(this->maxAltitudeFilterUserSettingKey)
        ? std::stoi(this->GetDataFromAsr(this->maxAltitudeFilterUserSettingKey))
        : this->defaultMaxAltitude;

    this->asrLoaded = true;
}

/*
    Called when EuroScope takes a dot command.

    Handle show/hide and opening a dialog to display options
*/
bool RadarScreen::OnCompileCommand(const char * sCommandLine)
{
    // Required so we can hit the dialog resource.
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    // Show hide
    if (strcmp(sCommandLine, ".estrails show") == 0) {
        this->visible = true;
        return true;
    } else if (strcmp(sCommandLine, ".estrails hide") == 0) {
        this->visible = false;
        return true;
    }

    // If this doesn't match
    if (strcmp(sCommandLine, ".estrails opt") != 0) {
        return false;
    }

    // Open settings menu
    COLORREF newColour = RGB(
        this->startColour.GetRed(),
        this->startColour.GetGreen(),
        this->startColour.GetBlue()
    );

    HistoryTrailData data;
    data.fade = &this->fadingTrails;
    data.degrade = &this->degradingTrails;
    data.antiAlias = &this->antialiasedTrails;
    data.type = &this->historyTrailType;
    data.length = &this->historyTrailLength;
    data.colour = &newColour;
    data.dotSize = &this->historyTrailDotSize;
    data.maxAltitude = &this->maximumDisplayAltitude;
    data.minAltitude = &this->minimumDisplayAltitude;

    HistoryTrailDialog dialog(NULL, &data);
    dialog.DoModal();

    this->historyTrailDotSizeFloat = static_cast<float>(this->historyTrailDotSize);
    this->startColour.SetFromCOLORREF(newColour);
    this->alphaPerDot = 255 / this->historyTrailLength;

    return true;
}

/*
    Called whenever a screen refresh happens. Only render the loaded modules.
*/
void RadarScreen::OnRefresh(HDC hdc, int phase)
{
    if (phase != EuroScopePlugIn::REFRESH_PHASE_BEFORE_TAGS || !this->visible || !this->asrLoaded) {
        return;
    }

    Gdiplus::Graphics graphics(hdc);

    Gdiplus::Color currentColourArgb = this->startColour;
    this->pen.SetColor(currentColourArgb);

    // Anti aliasing
    graphics.SetSmoothingMode((this->antialiasedTrails) ? Gdiplus::SmoothingModeAntiAlias : Gdiplus::SmoothingModeNone);

    // The dot we are to make.
    Gdiplus::RectF dot;

    // The amount that we'll reduce the history dot by on all sides each time.
    Gdiplus::REAL reducePerDot = (this->historyTrailDotSizeFloat / this->historyTrailLength) / 2;

    int roundNumber;
    // Loop through the history trails.
    for (
        std::map<std::string, std::unique_ptr<AircraftHistoryTrail>>::const_iterator aircraft = this->trails.cbegin();
        aircraft != this->trails.cend();
        ++aircraft
        ) {

        EuroScopePlugIn::CRadarTarget radarTarget = this->GetPlugIn()->RadarTargetSelect(
            aircraft->second->GetCallsign().c_str()
        );

        EuroScopePlugIn::CFlightPlan flightPlan = this->GetPlugIn()->FlightPlanSelect(
            aircraft->second->GetCallsign().c_str()
        );

        int altitude = radarTarget.GetPosition().GetPressureAltitude() <= this->GetPlugIn()->GetTransitionAltitude()
            ? radarTarget.GetPosition().GetPressureAltitude()
            : radarTarget.GetPosition().GetFlightLevel();

        // If they're not going fast enough or are off the screen, don't display the trail.
        if (
            !radarTarget.IsValid() ||
            radarTarget.GetGS() < this->minimumSpeed ||
            this->PositionOffScreen(radarTarget.GetPosition().GetPosition()) ||
            altitude < this->minimumDisplayAltitude ||
            altitude > this->maximumDisplayAltitude
        ) {
            continue;
        }

        // Round number used to govern fade and degrade, reset the colour
        roundNumber = 0;
        currentColourArgb = this->startColour;

        // Reset the dot height and width
        dot.Width = this->historyTrailDotSizeFloat;
        dot.Height = this->historyTrailDotSizeFloat;

        // Loop through the points and display.
        for (
            std::deque<EuroScopePlugIn::CPosition>::const_iterator position =
            aircraft->second->GetTrail().begin();
            position != aircraft->second->GetTrail().end();
            ++position
            ) {

            POINT dotCoordinates = this->ConvertCoordFromPositionToPixel(*position);
            // Adjust the dot size and position as required
            if (this->degradingTrails) {
                dot.X = dotCoordinates.x - (this->historyTrailDotSizeFloat / 2) + (roundNumber * reducePerDot);
                dot.Y = dotCoordinates.y - (this->historyTrailDotSizeFloat / 2) + (roundNumber * reducePerDot);
                dot.Width = dot.Width - reducePerDot;
                dot.Height = dot.Height - reducePerDot;
            }
            else {
                dot.X = dotCoordinates.x - (this->historyTrailDotSizeFloat / 2);
                dot.Y = dotCoordinates.y - (this->historyTrailDotSizeFloat / 2);
            }

            // Draw the dot
            this->DrawDot(
                graphics,
                this->pen,
                dot
            );

            // If the trails are set to fade, reduce the alpha value for the next run
            if (this->fadingTrails) {
                currentColourArgb = Gdiplus::Color(
                    255 - (roundNumber * this->alphaPerDot),
                    currentColourArgb.GetRed(),
                    currentColourArgb.GetGreen(),
                    currentColourArgb.GetBlue()
                );
                this->pen.SetColor(currentColourArgb);
            }

            // If we've done enough dots, we stop.
            if (roundNumber == this->historyTrailLength) {
                break;
            }

            roundNumber++;
        }
    }
}

/*
    Draws a single dot to the screen.
*/
void RadarScreen::DrawDot(
    Gdiplus::Graphics & graphics,
    Gdiplus::Pen & pen,
    const Gdiplus::RectF & area
) {
    if (this->historyTrailType == this->trailTypeDiamond) {
        // Points, starting at the centre left and going anticlockwise
        Gdiplus::PointF points[4] = {
            Gdiplus::PointF(area.X, area.Y + (area.Height / 2)),
            Gdiplus::PointF(area.X + (area.Width / 2), area.Y + area.Height),
            Gdiplus::PointF(area.X + area.Width, area.Y + (area.Height / 2)),
            Gdiplus::PointF(area.X + (area.Width / 2), area.Y)
        };
        graphics.DrawPolygon(&pen, points, 4);
    }
    else if (this->historyTrailType == this->trailTypeCircle) {
        graphics.DrawEllipse(&pen, area);
    }
    else {
        graphics.DrawRectangle(&pen, area);
    }
}

bool RadarScreen::HasAsrKey(const char * key)
{
    return this->GetDataFromAsr(key) != NULL;
}

bool RadarScreen::PositionOffScreen(EuroScopePlugIn::CPosition pos)
{
    RECT radarArea = this->GetRadarArea();
    POINT posOnScreen = this->ConvertCoordFromPositionToPixel(pos);

    return posOnScreen.x > radarArea.right || posOnScreen.x < radarArea.left ||
        posOnScreen.y > radarArea.bottom || posOnScreen.y < radarArea.top;
}

