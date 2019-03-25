#pragma once
#include "EuroScopePlugIn.h"
#include "AircraftHistoryTrail.h"

class EuroScopeTrailsPlugin : public EuroScopePlugIn::CPlugIn
{
    public:
        explicit EuroScopeTrailsPlugin();
        void OnFlightPlanFlightPlanDataUpdate(EuroScopePlugIn::CFlightPlan flightPlan);
        void OnFlightPlanDisconnect(EuroScopePlugIn::CFlightPlan flightPlan);
        inline virtual EuroScopePlugIn::CRadarScreen * OnRadarScreenCreated(
            const char * sDisplayName,
            bool NeedRadarContent,
            bool GeoReferenced,
            bool CanBeSaved,
            bool CanBeCreated
        );
        void OnRadarTargetPositionUpdate(EuroScopePlugIn::CRadarTarget radarTarget);

    private:
        std::map<std::string, std::unique_ptr<AircraftHistoryTrail>> historyTrails;
};
