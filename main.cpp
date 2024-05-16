#include <Windows.h>
#include <Tpm.h>

#pragma comment(lib, "Tpm.lib")

int main()
{
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
