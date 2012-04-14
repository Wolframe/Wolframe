#include "common.h"

LPCWSTR vcsRemoveFoldersExQuery = L"SELECT `RemoveFoldersEx`, `Component_`, `DirProperty`, `InstallMode` FROM `RemoveFoldersEx`";
enum eRemoveFoldersExQuery { rfqId = 1, rfqComponent, rfqProperty, feqMode };

static HRESULT RecursePath(
    __in_z LPCWSTR wzPath,
    __in_z LPCWSTR wzId,
    __in_z LPCWSTR wzComponent,
    __in_z LPCWSTR wzProperty,
    __in int iMode,
    __inout DWORD* pdwCounter,
    __inout MSIHANDLE* phTable,
    __inout MSIHANDLE* phColumns
    );


extern "C" UINT WINAPI RemoveFoldersEx(
    __in MSIHANDLE hInstall
    )
{
    //AssertSz(FALSE, "debug RemoveFoldersEx");

    HRESULT hr = S_OK;
    PMSIHANDLE hView;
    PMSIHANDLE hRec;
    LPWSTR sczId = NULL;
    LPWSTR sczComponent = NULL;
    LPWSTR sczProperty = NULL;
    LPWSTR sczPath = NULL;
    int iMode = 0;
    DWORD dwCounter = 0;
    MSIHANDLE hTable = NULL;
    MSIHANDLE hColumns = NULL;

    hr = WcaInitialize(hInstall, "RemoveFoldersEx");
    ExitOnFailure(hr, "Failed to initialize RemoveFoldersEx.");

    // anything to do?
    if (S_OK != WcaTableExists(L"RemoveFoldersEx"))
    {
        WcaLog(LOGMSG_STANDARD, "RemoveFoldersEx table doesn't exist, so there are no folders to remove.");
        ExitFunction();
    }

    // query and loop through all the remove folders exceptions
    hr = WcaOpenExecuteView(vcsRemoveFoldersExQuery, &hView);
    ExitOnFailure(hr, "Failed to open view on RemoveFoldersEx table");

    while (S_OK == (hr = WcaFetchRecord(hView, &hRec)))
    {
        hr = WcaGetRecordString(hRec, rfqId, &sczId);
        ExitOnFailure(hr, "Failed to get remove folder identity.");

        hr = WcaGetRecordString(hRec, rfqComponent, &sczComponent);
        ExitOnFailure(hr, "Failed to get remove folder component.");

        hr = WcaGetRecordString(hRec, rfqProperty, &sczProperty);
        ExitOnFailure(hr, "Failed to get remove folder property.");

        hr = WcaGetRecordInteger(hRec, feqMode, &iMode);
        ExitOnFailure(hr, "Failed to get remove folder mode");

        hr = WcaGetProperty(sczProperty, &sczPath);
        ExitOnFailure2(hr, "Failed to resolve remove folder property: %S for row: %S", sczProperty, sczId);

        hr = RecursePath(sczPath, sczId, sczComponent, sczProperty, iMode, &dwCounter, &hTable, &hColumns);
        ExitOnFailure2(hr, "Failed while navigating path: %S for row: %S", sczPath, sczId);
    }

    // reaching the end of the list is actually a good thing, not an error
    if (E_NOMOREITEMS == hr)
    {
        hr = S_OK;
    }
    ExitOnFailure(hr, "Failure occured while processing RemoveFoldersEx table");

LExit:
    if (hColumns)
    {
        ::MsiCloseHandle(hColumns);
    }

    if (hTable)
    {
        ::MsiCloseHandle(hTable);
    }

    ReleaseStr(sczPath);
    ReleaseStr(sczProperty);
    ReleaseStr(sczComponent);
    ReleaseStr(sczId);

    DWORD er = SUCCEEDED(hr) ? ERROR_SUCCESS : ERROR_INSTALL_FAILURE;
    return WcaFinalize(er);
}


static HRESULT RecursePath(
    __in_z LPCWSTR wzPath,
    __in_z LPCWSTR wzId,
    __in_z LPCWSTR wzComponent,
    __in_z LPCWSTR wzProperty,
    __in int iMode,
    __inout DWORD* pdwCounter,
    __inout MSIHANDLE* phTable,
    __inout MSIHANDLE* phColumns
    )
{
    HRESULT hr = S_OK;
    DWORD er;
    LPWSTR sczSearch = NULL;
    LPWSTR sczDirProperty = NULL;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATAW wfd;
    LPWSTR sczNext = NULL;

    // First recurse down to all the child directories.
    hr = StrAllocFormatted(&sczSearch, L"%s*", wzPath);
    ExitOnFailure1(hr, "Failed to allocate file search string in path: %S", wzPath);

    hFind = ::FindFirstFileW(sczSearch, &wfd);
    if (INVALID_HANDLE_VALUE == hFind)
    {
        er = ::GetLastError();
        if (ERROR_PATH_NOT_FOUND == er)
        {
            ExitFunction1(hr = S_FALSE);
        }
        else
        {
            hr = HRESULT_FROM_WIN32(er);
        }
        ExitOnFailure1(hr, "Failed to find all files in path: %S", wzPath);
    }

    do
    {
        // Skip files and the dot directories.
        if (FILE_ATTRIBUTE_DIRECTORY != (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) || L'.' == wfd.cFileName[0] && (L'\0' == wfd.cFileName[1] || (L'.' == wfd.cFileName[1] && L'\0' == wfd.cFileName[2])))
        {
            continue;
        }

        hr = StrAllocFormatted(&sczNext, L"%s%s\\", wzPath, wfd.cFileName);
        ExitOnFailure2(hr, "Failed to concat filename '%S' to string: %S", wfd.cFileName, wzPath);

        hr = RecursePath(sczNext, wzId, wzComponent, wzProperty, iMode, pdwCounter, phTable, phColumns);
        ExitOnFailure1(hr, "Failed to recurse path: %S", sczNext);
    } while (::FindNextFileW(hFind, &wfd));

    er = ::GetLastError();
    if (ERROR_NO_MORE_FILES == er)
    {
        hr = S_OK;
    }
    else
    {
        hr = HRESULT_FROM_WIN32(er);
        ExitOnFailure1(hr, "Failed while looping through files in directory: %S", wzPath);
    }

    // Finally, set a property that points at our path.
    hr = StrAllocFormatted(&sczDirProperty, L"_%s_%u", wzProperty, *pdwCounter);
    ExitOnFailure1(hr, "Failed to allocate DirProperty for RemoveFiles table with property: %S.", wzProperty);

    ++(*pdwCounter);

    hr = WcaSetProperty(sczDirProperty, wzPath);
    ExitOnFailure2(hr, "Failed to set DirProperty: %S with path: %S", sczDirProperty, wzPath);

    // Add the row to remove any files and another row to remove the folder.
    hr = WcaAddTempRecord(phTable, phColumns, L"RemoveFile", NULL, 1, 5, L"RfxFiles", wzComponent, L"*.*", sczDirProperty, iMode);
    ExitOnFailure2(hr, "Failed to add row to remove all files for RemoveFolderEx row: %S under path:", wzId, wzPath);

    hr = WcaAddTempRecord(phTable, phColumns, L"RemoveFile", NULL, 1, 5, L"RfxFolder", wzComponent, NULL, sczDirProperty, iMode);
    ExitOnFailure2(hr, "Failed to add row to remove folder for RemoveFolderEx row: %S under path: %S", wzId, wzPath);

LExit:
    if (INVALID_HANDLE_VALUE != hFind)
    {
        ::FindClose(hFind);
    }

    ReleaseStr(sczNext);
    ReleaseStr(sczDirProperty);
    ReleaseStr(sczSearch);
    return hr;
}
