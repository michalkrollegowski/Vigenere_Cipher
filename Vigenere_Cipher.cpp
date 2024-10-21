#include "framework.h"
#include "Vigenere_Cipher.h"
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cctype>

#define MAX_LOADSTRING 1005

// Global Variables
HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];
std::wstring key; // Global variable for the key

// Function Declarations
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

// Control Identifiers
#define IDC_ENCRYPT_TEXT 1001 // Identifier for encryption text field
#define IDC_DECRYPT_TEXT 1002 // Identifier for decryption text field
#define IDC_ENCRYPT_BUTTON 1003 // Identifier for encrypt button
#define IDC_DECRYPT_BUTTON 1004 // Identifier for decrypt button
#define IDC_KEY_TEXT 1006 // Identifier for key text field
#define IDC_INSERT_KEY_BUTTON 1007 // Identifier for insert key button

// Vigenère Cipher functions
std::wstring VigenereEncrypt(const std::wstring& text, const std::wstring& key);
std::wstring VigenereDecrypt(const std::wstring& text, const std::wstring& key);
bool IsTextValid(const std::wstring& text, const std::vector<std::wstring>& alphabet);

// The alphabet used for the cipher
static std::vector<std::wstring> alphabet = {
    L"A", L"Ą", L"B", L"C", L"Ć", L"D", L"E", L"Ę", L"F", L"G",
    L"H", L"I", L"J", L"K", L"L", L"Ł", L"M", L"N", L"Ń", L"O",
    L"Ó", L"P", L"Q", L"R", L"S", L"Ś", L"T", L"U", L"V", L"W", L"X",
    L"Y", L"Z", L"Ź", L"Ż"
};

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_VIGENERECIPHER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance(hInstance, nCmdShow)) {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_VIGENERECIPHER));

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance) {
    WNDCLASSEXW wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_VIGENERECIPHER));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_VIGENERECIPHER);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
    hInst = hInstance;

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        300, 100, 800, 600, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd) {
        return FALSE;
    }

    // Font Creation
    HFONT hFont = CreateFont(24, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_QUALITY, OUT_DEFAULT_PRECIS);

    // Encrypting Label
    CreateWindowEx(0, L"STATIC", L"Encrypting Field:", WS_CHILD | WS_VISIBLE | SS_LEFT,
        300, 10, 200, 50, hWnd, nullptr, hInstance, nullptr);

    // Encrypting Text Field
    HWND hEncryptingText = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"",
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL,
        250, 40, 300, 100, hWnd, (HMENU)IDC_ENCRYPT_TEXT, hInstance, nullptr);

    // Encrypt Button
    CreateWindowEx(0, L"BUTTON", L"Encrypt", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        350, 150, 100, 30, hWnd, (HMENU)IDC_ENCRYPT_BUTTON, hInstance, nullptr);

    // Decrypting Label
    CreateWindowEx(0, L"STATIC", L"Decrypting Field:", WS_CHILD | WS_VISIBLE | SS_LEFT,
        300, 310, 200, 50, hWnd, nullptr, hInstance, nullptr);

    // Decrypting Text Field
    HWND hDecryptingText = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"",
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL,
        250, 340, 300, 100, hWnd, (HMENU)IDC_DECRYPT_TEXT, hInstance, nullptr);

    // Decrypt Button
    CreateWindowEx(0, L"BUTTON", L"Decrypt", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        350, 450, 100, 30, hWnd, (HMENU)IDC_DECRYPT_BUTTON, hInstance, nullptr);

    // KEY Label
    CreateWindowEx(0, L"STATIC", L"KEY:", WS_CHILD | WS_VISIBLE | SS_LEFT,
        50, 180, 100, 50, hWnd, nullptr, hInstance, nullptr);

    // KEY Text Field
    HWND hKEY = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"",
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE,
        50, 200, 100, 100, hWnd, (HMENU)IDC_KEY_TEXT, hInstance, nullptr);

    // KEY Button
    CreateWindowEx(0, L"BUTTON", L"INSERT KEY", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        50, 300, 100, 30, hWnd, (HMENU)IDC_INSERT_KEY_BUTTON, hInstance, nullptr);

    SendMessage(hEncryptingText, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessage(hDecryptingText, WM_SETFONT, (WPARAM)hFont, TRUE);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

// Vigenère encryption
std::wstring VigenereEncrypt(const std::wstring& text, const std::wstring& key) {
    std::wstring result;
    int keyLength = key.length();
    size_t j = 0;

    for (size_t i = 0; i < text.length(); ++i) {
        wchar_t currentChar = text[i];
        std::wstring currentCharStr(1, toupper(currentChar)); // Convert to uppercase

        // Check if character belongs to the alphabet
        auto it = std::find(alphabet.begin(), alphabet.end(), currentCharStr);
        if (it != alphabet.end()) {
            int index = std::distance(alphabet.begin(), it);
            int shift = std::distance(alphabet.begin(), std::find(alphabet.begin(), alphabet.end(), std::wstring(1, toupper(key[j % keyLength]))));
            int encryptedIndex = (index + shift) % alphabet.size();
            result += alphabet[encryptedIndex];
            j++;
        }
        else {
            result += currentChar; // Keep non-alphabet characters as is
        }
    }

    return result;
}

// Vigenère decryption
std::wstring VigenereDecrypt(const std::wstring& text, const std::wstring& key) {
    std::wstring result;
    int keyLength = key.length();
    size_t j = 0;

    for (size_t i = 0; i < text.length(); ++i) {
        wchar_t currentChar = text[i];
        std::wstring currentCharStr(1, toupper(currentChar)); // Convert to uppercase

        // Check if character belongs to the alphabet
        auto it = std::find(alphabet.begin(), alphabet.end(), currentCharStr);
        if (it != alphabet.end()) {
            int index = std::distance(alphabet.begin(), it);
            int shift = std::distance(alphabet.begin(), std::find(alphabet.begin(), alphabet.end(), std::wstring(1, toupper(key[j % keyLength]))));
            int decryptedIndex = (index - shift + alphabet.size()) % alphabet.size();
            result += alphabet[decryptedIndex];
            j++;
        }
        else {
            result += currentChar; // Keep non-alphabet characters as is
        }
    }

    return result;
}

// Check if all characters in the text are valid (belong to the alphabet)
bool IsTextValid(const std::wstring& text, const std::vector<std::wstring>& alphabet) {
    for (const wchar_t& c : text) {
        std::wstring charStr(1, toupper(c));
        if (std::find(alphabet.begin(), alphabet.end(), charStr) == alphabet.end()) {
            return false;
        }
    }
    return true;
}

// Main message handler
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_COMMAND: {
        int wmId = LOWORD(wParam);

        switch (wmId) {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        case IDC_INSERT_KEY_BUTTON: {
            WCHAR buffer[1024];
            GetWindowText(GetDlgItem(hWnd, IDC_KEY_TEXT), buffer, 1024);
            key = buffer;
            // Validate the key
            if (!IsTextValid(key, alphabet)) {
                MessageBox(hWnd, L"Key contains invalid characters. Please enter a valid key.", L"Invalid Key", MB_OK | MB_ICONERROR);
                key.clear(); // Clear the invalid key
            }
        }
                                  break;
        case IDC_ENCRYPT_BUTTON: {
            WCHAR buffer[1024];
            GetWindowText(GetDlgItem(hWnd, IDC_ENCRYPT_TEXT), buffer, 1024);
            std::wstring text = buffer;
            if (key.empty()) {
                MessageBox(hWnd, L"Please insert a key before decrypting.", L"Error", MB_OK | MB_ICONERROR);
                break;
            }
            if (!IsTextValid(text, alphabet)) {
                MessageBox(hWnd, L"Text contains invalid characters.", L"Error", MB_OK | MB_ICONERROR);
                break;
            }
            std::wstring encryptedText = VigenereEncrypt(text, key);
            SetWindowText(GetDlgItem(hWnd, IDC_DECRYPT_TEXT), encryptedText.c_str());
        }
                               break;
        case IDC_DECRYPT_BUTTON: {
            WCHAR buffer[1024];
            GetWindowText(GetDlgItem(hWnd, IDC_DECRYPT_TEXT), buffer, 1024);
            std::wstring text = buffer;
            if (key.empty()) {
                MessageBox(hWnd, L"Please insert a key before decrypting.", L"Error", MB_OK | MB_ICONERROR);
                break;
            }
            if (!IsTextValid(text, alphabet)) {
                MessageBox(hWnd, L"Text contains invalid characters.", L"Error", MB_OK | MB_ICONERROR);
                break;
            }
            std::wstring decryptedText = VigenereDecrypt(text, key);
            SetWindowText(GetDlgItem(hWnd, IDC_ENCRYPT_TEXT), decryptedText.c_str());
        }
                               break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
                   break;
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
    }
                 break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    UNREFERENCED_PARAMETER(lParam);
    switch (message) {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}