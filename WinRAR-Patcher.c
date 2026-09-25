#include <windows.h>
#include <shellapi.h>
#include <stdio.h>

static const char KEY_CONTENT[] =
    "RAR registration data\n"
    "WinRAR\n"
    "Unlimited Company License\n"
    "UID=4b914fb772c8376bf571\n"
    "6412212250f5711ad072cf351cfa39e2851192daf8a362681bbb1d\n"
    "cd48da1d14d995f0bbf960fce6cb5ffde62890079861be57638717\n"
    "7131ced835ed65cc743d9777f2ea71a8e32c7e593cf66794343565\n"
    "b41bcf56929486b8bcdac33d50ecf773996052598f1f556defffbd\n"
    "982fbe71e93df6b6346c37a3890f3c7edc65d7f5455470d13d1190\n"
    "6e6fb824bcf25f155547b5fc41901ad58c0992f570be1cf5608ba9\n"
    "aef69d48c864bcd72d15163897773d314187f6a9af350808719796";

int IsElevated(void) {
    HANDLE token = NULL;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token))
        return 0;
    TOKEN_ELEVATION elev;
    DWORD size = sizeof(elev);
    int elevated = 0;
    if (GetTokenInformation(token, TokenElevation, &elev, sizeof(elev), &size))
        elevated = elev.TokenIsElevated ? 1 : 0;
    CloseHandle(token);
    return elevated;
}

void RelaunchAsAdmin(void) {
    char path[MAX_PATH];
    if (!GetModuleFileNameA(NULL, path, MAX_PATH))
        return;
    SHELLEXECUTEINFOA sei;
    ZeroMemory(&sei, sizeof(sei));
    sei.cbSize = sizeof(sei);
    sei.lpVerb = "runas";
    sei.lpFile = path;
    sei.nShow = SW_SHOWNORMAL;
    if (ShellExecuteExA(&sei))
        ExitProcess(0);
}

int main(void) {
    const char *dirPath = "C:\\Program Files\\WinRAR";
    const char *keyPath = "C:\\Program Files\\WinRAR\\rarreg.key";

    printf("[*] WinRAR Patcher\n");

    if (!IsElevated()) {
        printf("[*] Requesting administrator privileges...\n");
        RelaunchAsAdmin();
        printf("[-] Elevation declined or failed.\n");
        printf("[-] Right-click the EXE and choose 'Run as administrator'.\n");
        printf("Press any key to exit...\n");
        getchar();
        return 1;
    }

    printf("[*] Writing key file to: %s\n", keyPath);

    CreateDirectoryA(dirPath, NULL);

    FILE *f = fopen(keyPath, "wb");
    if (f) {
        fwrite(KEY_CONTENT, 1, sizeof(KEY_CONTENT) - 1, f);
        fclose(f);
        printf("[+] Patching complete!\n");
    } else {
        DWORD err = GetLastError();
        printf("[-] Failed to write key file (error %lu).\n", (unsigned long)err);
        printf("[-] Try running as Administrator.\n");
        printf("Press any key to exit...\n");
        getchar();
        return 1;
    }

    printf("Press any key to exit...\n");
    getchar();
    return 0;
}
