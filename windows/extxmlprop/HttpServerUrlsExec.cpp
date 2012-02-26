#include "common.h"

static HRESULT ExecHttpServerUrlChange(
    __in MSIHANDLE hInstall,
    __in WCA_TODO action
    );
static HRESULT Add(
    __in_z LPWSTR wzUrl,
    __in_z LPWSTR wzSddl
    );
static HRESULT Remove(
    __in_z LPWSTR wzUrl,
    __in_z LPWSTR wzSddl
    );


extern "C" UINT WINAPI WriteHttpServerUrls(__in MSIHANDLE hInstall)
{
    //AssertSz(FALSE, "debug WriteHttpServerUrls");
    HRESULT hr = S_OK;

    hr = WcaInitialize(hInstall, "WriteHttpServerUrls");
    ExitOnFailure(hr, "Failed to initialize WriteHttpServerUrls.");

    hr = ExecHttpServerUrlChange(hInstall, WCA_TODO_INSTALL);
    ExitOnFailure(hr, "Failed to write HTTP Server URLs.");

LExit:
    DWORD er = SUCCEEDED(hr) ? ERROR_SUCCESS : ERROR_INSTALL_FAILURE;
    return WcaFinalize(er);
}


extern "C" UINT WINAPI RemoveHttpServerUrls(MSIHANDLE hInstall)
{
    //AssertSz(FALSE, "debug RemoveHttpServerUrls");
    HRESULT hr = S_OK;

    hr = WcaInitialize(hInstall, "RemoveHttpServerUrls");
    ExitOnFailure(hr, "Failed to initialize RemoveHttpServerUrls.");

    hr = ExecHttpServerUrlChange(hInstall, WCA_TODO_UNINSTALL);
    ExitOnFailure(hr, "Failed to remove HTTP Server URLs.");

LExit:
    DWORD er = SUCCEEDED(hr) ? ERROR_SUCCESS : ERROR_INSTALL_FAILURE;
    return WcaFinalize(er);
}


static HRESULT ExecHttpServerUrlChange(
    __in MSIHANDLE hInstall,
    __in WCA_TODO action
    )
{
    HRESULT hr = S_OK;
    DWORD er;
    BOOL fHttpInitialized = FALSE;
    HTTPAPI_VERSION haVersion = HTTPAPI_VERSION_1;
    HANDLE hHttp = NULL;
    LPWSTR sczCaData = NULL;
    LPWSTR pwz;
    LPWSTR sczId = NULL;
    LPWSTR sczUrl = NULL;
    LPWSTR sczSddl = NULL;

    er = ::HttpInitialize(haVersion, HTTP_INITIALIZE_CONFIG, NULL);
    hr = HRESULT_FROM_WIN32(er);
    ExitOnFailure(hr, "Failed to initialize HTTP Server configuration.");

    fHttpInitialized = TRUE;

    hr = WcaGetProperty(L"CustomActionData", &sczCaData);
    ExitOnFailure(hr, "Failed to get CustomActionData");

    WcaLog(LOGMSG_TRACEONLY, "CustomActionData: %S", sczCaData);

    pwz = sczCaData;
    while (S_OK == (hr = WcaReadStringFromCaData(&pwz, &sczId)))
    {
        hr = WcaReadStringFromCaData(&pwz, &sczUrl);
        ExitOnFailure(hr, "Failed to parse CustomActionData, url.");

        hr = WcaReadStringFromCaData(&pwz, &sczSddl);
        ExitOnFailure(hr, "Failed to parse CustomActionData, sddl.");

        if (WCA_TODO_INSTALL == action)
        {
            hr = Add(sczUrl, sczSddl);
            ExitOnFailure1(hr, "Failed to add HTTP Server URL: %S", sczId);
        }
        else
        {
            hr = Remove(sczUrl, sczSddl);
            ExitOnFailure1(hr, "Failed to remove HTTP Server URL: %S", sczId);
        }
    }

    if (E_NOMOREITEMS == hr) // If there are no more items, all is well
    {
        hr = S_OK;
    }

LExit:
    ReleaseStr(sczSddl);
    ReleaseStr(sczUrl);
    ReleaseStr(sczId);
    ReleaseStr(sczCaData);

    if (fHttpInitialized)
    {
        ::HttpTerminate(HTTP_INITIALIZE_SERVER | HTTP_INITIALIZE_CONFIG, NULL);
    }

    return hr;
}


static HRESULT Add(
    __in_z LPWSTR wzUrl,
    __in_z LPWSTR wzSddl
    )
{
    HRESULT hr = S_OK;
    DWORD er;

    HTTP_SERVICE_CONFIG_URLACL_SET set;
    set.KeyDesc.pUrlPrefix = wzUrl;
    set.ParamDesc.pStringSecurityDescriptor = wzSddl;

    er = ::HttpSetServiceConfiguration(NULL, HttpServiceConfigUrlAclInfo, &set, sizeof(set), NULL);
    hr = HRESULT_FROM_WIN32(er);
    if (HRESULT_FROM_WIN32(ERROR_ALREADY_EXISTS) == hr)
    {
        hr = S_FALSE;
    }
    ExitOnFailure2(hr, "Failed to set ACL on HTTP Server URL: %S, ACL: %S", wzUrl, wzSddl);

LExit:
    return hr;
}


static HRESULT Remove(
    __in_z LPWSTR wzUrl,
    __in_z LPWSTR wzSddl
    )
{
    HRESULT hr = S_OK;
    DWORD er;

    HTTP_SERVICE_CONFIG_URLACL_SET set;
    set.KeyDesc.pUrlPrefix = wzUrl;
    set.ParamDesc.pStringSecurityDescriptor = wzSddl;

    er = ::HttpDeleteServiceConfiguration(NULL, HttpServiceConfigUrlAclInfo, &set, sizeof(set), NULL);
    hr = HRESULT_FROM_WIN32(er);
    if (HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND) == hr)
    {
        hr = S_FALSE;
    }
    ExitOnFailure2(hr, "Failed to remove ACL on HTTP Server URL: %S, ACL: %S", wzUrl, wzSddl);

LExit:
    return hr;
}
