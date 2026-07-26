#include "auth.h"
#include <cstdint>
#include <windows.h>
#include <winhttp.h>
#include <iphlpapi.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>

#pragma comment(lib, "winhttp.lib")
#pragma comment(lib, "iphlpapi.lib")

typedef unsigned int uint32;
typedef unsigned char uint8;

uint32 rotr(uint32 x, uint32 n) { return (x >> n) | (x << (32 - n)); }

std::string sha256(const std::string& input) {
    uint32 h[8] = {
        0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
        0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
    };

    static const uint32 k[64] = {
        0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
        0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
        0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
        0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
        0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
        0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
        0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
        0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
    };

    std::vector<uint8> data(input.begin(), input.end());
    uint64_t bit_len = data.size() * 8;

    data.push_back(0x80);
    while ((data.size() % 64) != 56) data.push_back(0x00);

    for (int i = 7; i >= 0; i--)
        data.push_back((bit_len >> (i * 8)) & 0xff);

    for (size_t chunk = 0; chunk < data.size(); chunk += 64) {
        uint32 w[64];
        for (int i = 0; i < 16; i++) {
            w[i] = (data[chunk + i * 4] << 24) |
                (data[chunk + i * 4 + 1] << 16) |
                (data[chunk + i * 4 + 2] << 8) |
                (data[chunk + i * 4 + 3]);
        }
        for (int i = 16; i < 64; i++) {
            uint32 s0 = rotr(w[i - 15], 7) ^ rotr(w[i - 15], 18) ^ (w[i - 15] >> 3);
            uint32 s1 = rotr(w[i - 2], 17) ^ rotr(w[i - 2], 19) ^ (w[i - 2] >> 10);
            w[i] = w[i - 16] + s0 + w[i - 7] + s1;
        }

        uint32 a = h[0], b = h[1], c = h[2], d = h[3];
        uint32 e = h[4], f = h[5], g = h[6], hh = h[7];

        for (int i = 0; i < 64; i++) {
            uint32 S1 = rotr(e, 6) ^ rotr(e, 11) ^ rotr(e, 25);
            uint32 ch = (e & f) ^ ((~e) & g);
            uint32 temp1 = hh + S1 + ch + k[i] + w[i];
            uint32 S0 = rotr(a, 2) ^ rotr(a, 13) ^ rotr(a, 22);
            uint32 maj = (a & b) ^ (a & c) ^ (b & c);
            uint32 temp2 = S0 + maj;

            hh = g;
            g = f;
            f = e;
            e = d + temp1;
            d = c;
            c = b;
            b = a;
            a = temp1 + temp2;
        }

        h[0] += a; h[1] += b; h[2] += c; h[3] += d;
        h[4] += e; h[5] += f; h[6] += g; h[7] += hh;
    }

    std::ostringstream out;
    for (int i = 0; i < 8; i++)
        out << std::hex << std::setw(8) << std::setfill('0') << h[i];

    return out.str();
}

std::string getHWID() {
    IP_ADAPTER_INFO info[16];
    DWORD len = sizeof(info);

    if (GetAdaptersInfo(info, &len) != ERROR_SUCCESS)
        return "unknown";

    auto& a = info[0];
    std::ostringstream mac;

    for (UINT i = 0; i < a.AddressLength; i++) {
        if (i) mac << ":";
        mac << std::hex << std::setw(2) << std::setfill('0')
            << (int)a.Address[i];
    }

    return sha256(mac.str());
}

bool validateLicense(const std::string& key) {
    std::string hwid = getHWID();

    HINTERNET hSession = WinHttpOpen(L"AuthClient/1.0",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        NULL, NULL, 0);

    if (!hSession) return false;

    HINTERNET hConnect = WinHttpConnect(
        hSession,
        L"www.coreauth.cc",
        INTERNET_DEFAULT_HTTPS_PORT,
        0
    );

    if (!hConnect) return false;

    HINTERNET hRequest = WinHttpOpenRequest(
        hConnect,
        L"POST",
        L"/api/v1/license/validate",
        NULL,
        WINHTTP_NO_REFERER,
        WINHTTP_DEFAULT_ACCEPT_TYPES,
        WINHTTP_FLAG_SECURE
    );

    if (!hRequest) return false;

    std::string body =
        "{\"license_key\":\"" + key +
        "\",\"hwid\":\"" + hwid + "\"}";

    std::wstring headers =
        L"Content-Type: application/json\r\n"
        L"X-App-Key: coreauth-app-lCguRy15By2L\r\n"
        L"X-Api-Secret: coreauth-sec-s3Y4K1XlnXg8iXhb\r\n";

    BOOL sent = WinHttpSendRequest(
        hRequest,
        headers.c_str(),
        headers.size(),
        (LPVOID)body.c_str(),
        body.size(),
        body.size(),
        0
    );

    if (!sent) return false;

    WinHttpReceiveResponse(hRequest, NULL);

    std::string response;

    DWORD avail = 0;

    do {
        if (!WinHttpQueryDataAvailable(hRequest, &avail))
            break;

        if (avail == 0)
            break;

        std::vector<char> buf(avail);
        DWORD read = 0;

        if (!WinHttpReadData(hRequest, buf.data(), avail, &read))
            break;

        response.append(buf.data(), read);

    } while (avail > 0);

    bool ok = response.find("\"success\":true") != std::string::npos;

    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);

    return ok;

}

