#include "common.h"

LPCWSTR vcsHttpServerUrlQuery = L"SELECT `HttpServerUrl`, `Component_`, `Url`, `Sddl` FROM `HttpServerUrl`";
enum eHttpServerUrlQuery { hsuqId = 1, hsuqComponent, hsuqUrl, hsuqSddl};

static HRESULT ConfigureHttpServerUrls(
    __in WCA_TODO action
    );

extern "C" UINT WINAPI InstallHttpServerUrls(__in MSIHANDLE hInstall)
{
    //AssertSz(FALSE, "debug InstallHttpServerUrls");
    HRESULT hr = S_OK;

    hr = WcaInitialize(hInstall, "InstallHttpServerUrls");
    ExitOnFailure(hr, "Failed to initialize InstallHttpServerUrls.");

    hr = ConfigureHttpServerUrls(WCA_TODO_INSTALL);

LExit:
    DWORD er = SUCCEEDED(hr) ? ERROR_SUCCESS : ERROR_INSTALL_FAILURE;
    return WcaFinalize(er);
}


extern "C" UINT WINAPI UninstallHttpServerUrls(__in MSIHANDLE hInstall)
{
    //AssertSz(FALSE, "debug UninstallHttpServerUrls");
    HRESULT hr = S_OK;

    hr = WcaInitialize(hInstall, "UninstallHttpServerUrls");
    ExitOnFailure(hr, "Failed to initialize UninstallHttpServerUrls.");

    hr = ConfigureHttpServerUrls(WCA_TODO_UNINSTALL);

LExit:
    DWORD er = SUCCEEDED(hr) ? ERROR_SUCCESS : ERROR_INSTALL_FAILURE;
    return WcaFinalize(er);
}


static HRESULT ConfigureHttpServerUrls(
    __in WCA_TODO action
    )
{
    HRESULT hr = S_OK;
    PMSIHANDLE hView;
    PMSIHANDLE hRec;
    LPWSTR sczId = NULL;
    LPWSTR sczComponent = NULL;
    LPWSTR sczUrl = NULL;
    LPWSTR sczSddl = NULL;
    LPWSTR sczCustomActionData = NULL;

    // check for the prerequsite tables
    if (S_OK != WcaTableExists(L"HttpServerUrl"))
    {
        WcaLog(LOGMSG_VERBOSE, "skipping HttpServer CustomAction, no HttpServerUrl table");
        ExitFunction1(hr = S_FALSE);
    }

    // loop through all the sql strings
    hr = WcaOpenExecuteView(vcsHttpServerUrlQuery, &hView);
    ExitOnFailure(hr, "Failed to open view on HttpServerUrl table");

    while (S_OK == (hr = WcaFetchRecord(hView, &hRec)))
    {
        INSTALLSTATE isInstalled = INSTALLSTATE_UNKNOWN;
        INSTALLSTATE isAction = INSTALLSTATE_UNKNOWN;

        hr = WcaGetRecordString(hRec, hsuqId, &sczId);
        ExitOnFailure(hr, "Failed to get Id for HttpServerUrl.");

        hr = WcaGetRecordString(hRec, hsuqComponent, &sczComponent);
        ExitOnFailure1(hr, "Failed to get Component for HttpServerUrl: %S", sczId);

        DWORD er = ::MsiGetComponentStateW(WcaGetInstallHandle(), sczComponent, &isInstalled, &isAction);
        hr = HRESULT_FROM_WIN32(er);
        ExitOnFailure1(hr, "Failed to get state for component: %S", sczComponent);

        // If we're doing install but the Component is not being installed or we're doing
        // uninstall but the Component is not being uninstalled, skip it.
        if ((WcaIsInstalling(isInstalled, isAction) && WCA_TODO_INSTALL != action) ||
            (WcaIsUninstalling(isInstalled, isAction) && WCA_TODO_UNINSTALL != action))
        {
            continue;
        }

        hr = WcaGetRecordFormattedString(hRec, hsuqUrl, &sczUrl);
        ExitOnFailure1(hr, "Failed to get URL for HttpServerUrl: %S", sczId);

        hr = WcaGetRecordFormattedString(hRec, hsuqSddl, &sczSddl);
        ExitOnFailure1(hr, "Failed to get SDDL for HttpServerUrl: %S", sczId);

        hr = WcaWriteStringToCaData(sczId, &sczCustomActionData);
        ExitOnFailure1(hr, "Failed to add Id to CustomActionData for HttpServerUrl: %S", sczId);

        hr = WcaWriteStringToCaData(sczUrl, &sczCustomActionData);
        ExitOnFailure1(hr, "Failed to add URL to CustomActionData for HttpServerUrl: %S", sczId);

        hr = WcaWriteStringToCaData(sczSddl, &sczCustomActionData);
        ExitOnFailure1(hr, "Failed to add SDDL to CustomActionData for HttpServerUrl: %S", sczId);
    }

    if (E_NOMOREITEMS == hr)
    {
        hr = S_OK;
    }
    ExitOnFailure(hr, "Failure occured while reading HttpServerUrl table");

    if (sczCustomActionData && *sczCustomActionData)
    {
        hr = WcaDoDeferredAction(WCA_TODO_INSTALL == action ? L"RollbackWriteHttpServerUrls" : L"RollbackRemoveHttpServerUrls", sczCustomActionData, 100);
        ExitOnFailure(hr, "Failed to schedule HttpServerUrl rollback.");

        hr = WcaDoDeferredAction(WCA_TODO_INSTALL == action ? L"WriteHttpServerUrls" : L"RemoveHttpServerUrls", sczCustomActionData, 100);
        ExitOnFailure(hr, "Failed to schedule HttpServerUrl rollback.");
    }

LExit:
    ReleaseStr(sczCustomActionData);
    ReleaseStr(sczSddl);
    ReleaseStr(sczUrl);
    ReleaseStr(sczComponent);
    ReleaseStr(sczId);
    return hr;
}
