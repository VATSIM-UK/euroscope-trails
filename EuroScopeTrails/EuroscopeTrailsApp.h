#pragma once
#include "stdafx.h"
#include "EuroScopeTrailsPlugin.h"

/*
    DLL Entry Point For MFC
*/
class EuroScopeTrailsApp : public CWinApp
{
    public:
        EuroScopeTrailsPlugin thePlugin;
};
