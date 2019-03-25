#include "stdafx.h"
#include "EuroScopeTrailsPlugin.h"
#include "RadarScreen.h"

EuroScopeTrailsPlugin::EuroScopeTrailsPlugin()
    : EuroScopePlugIn::CPlugIn(
        EuroScopePlugIn::COMPATIBILITY_CODE,
        "EuroScope History Trails",
        "1.0.0",
        "VATSIM UK",
        "VATSIM United Kingdom Division"
    )
{
    this->DisplayUserMessage(
        "message",
        "ESTrails",
        "EuroScope History Trails Loaded",
        true,
        true,
        false,
        false,
        false
    );
}

/*
    Called by EuroScope when flightplan data is updated.
*/
void EuroScopeTrailsPlugin::OnFlightPlanFlightPlanDataUpdate(EuroScopePlugIn::CFlightPlan flightPlan)
{
    if (this->historyTrails.count(flightPlan.GetCallsign())) {
        return;
    }

    this->historyTrails[flightPlan.GetCallsign()] = std::make_unique<AircraftHistoryTrail>(flightPlan.GetCallsign());
}

/*
    Called by Euroscope when an aircraft disconnects from the network. If the aircraft
    is NOFP, then the only really useful information we get from the argument is the callsign.
*/
void EuroScopeTrailsPlugin::OnFlightPlanDisconnect(EuroScopePlugIn::CFlightPlan flightPlan)
{
    this->historyTrails.erase(flightPlan.GetCallsign());
}

/*
    Returns a pointer a radar screen instance
*/
inline EuroScopePlugIn::CRadarScreen * EuroScopeTrailsPlugin::OnRadarScreenCreated(
    const char * sDisplayName,
    bool NeedRadarContent,
    bool GeoReferenced,
    bool CanBeSaved,
    bool CanBeCreated
) {
    return new RadarScreen(this->historyTrails);
}

/*
    Called by EuroScope every time a radar target has a position update.
*/
void EuroScopeTrailsPlugin::OnRadarTargetPositionUpdate(EuroScopePlugIn::CRadarTarget radarTarget)
{
    if (!radarTarget.IsValid()) {
        return;
    }

    EuroScopePlugIn::CFlightPlan flightPlan = radarTarget.GetCorrelatedFlightPlan();
    if (!flightPlan.IsValid() || !this->historyTrails.count(flightPlan.GetCallsign())) {
        return;
    }

    this->historyTrails.at(flightPlan.GetCallsign())->AddItem(radarTarget.GetPosition().GetPosition());
}
