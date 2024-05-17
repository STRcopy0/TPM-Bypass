#include <Windows.h>
#include <Tpm.h>

#pragma comment(lib, "Tpm.lib")

bool IsRunningAsAdmin()
{
    BOOL fIsElevated = FALSE;
    HANDLE hToken = NULL;
    TOKEN_ELEVATION elevation;
    DWORD dwSize;

    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
    {
        std::cerr << "Failed to get Process Token: " << GetLastError() << std::endl;
        return false;  // if fail then falsae
    }

    if (!GetTokenInformation(hToken, TokenElevation, &elevation, sizeof(elevation), &dwSize))
    {
        std::cerr << "Failed to get Token Information: " << GetLastError() << std::endl;
        CloseHandle(hToken);
        return false;  //if fail then false
    }

    fIsElevated = elevation.TokenIsElevated;

    CloseHandle(hToken);
    return fIsElevated;
}

int main()
{
    if (!IsRunningAsAdmin())
    {
        std::cerr << "This program must be run as an administrator." << std::endl;
        return 1;  // exit with error code
    }

    // fetch tpm API
    TPMS_API_VERSION tpmApiVersion = { 2, 0 };
    HRESULT hr = TpmInitialize(&tpmApiVersion);
    if (FAILED(hr))
    {
        printf("Failed to initialize TPM API: 0x%x\n", hr);
        return 1;
    }

    // fetch TPM device
    HANDLE hTpm = INVALID_HANDLE_VALUE;
    hr = TpmGetDevice(0, &hTpm);
    if (FAILED(hr))
    {
        printf("Failed to get TPM device: 0x%x\n", hr);
        TpmShutdown();
        return 1;
    }

    // disable auto provision
    TPMAUTO_PROVISIONING_INFO provisioningInfo;
    provisioningInfo.dwSize = sizeof(TPMAUTO_PROVISIONING_INFO);
    provisioningInfo.bEnableAutoProvisioning = FALSE;
    hr = TpmSetAutoProvisioning(hTpm, &provisioningInfo);
    if (FAILED(hr))
    {
        printf("Failed to disable auto-provisioning: 0x%x\n", hr);
        TpmCloseDevice(hTpm);
        TpmShutdown();
        return 1;
    }

    // dont leave no mess cuh
    TpmCloseDevice(hTpm);
    TpmShutdown();

    printf("Auto-provisioning disabled successfully.\n");
    return 0;
}
