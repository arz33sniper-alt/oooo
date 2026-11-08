#include <windows.h>
#include <commdlg.h>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <bitset>
#include <string>
#include "../Machine.h"

using namespace std;

#define ID_BTN_CLEAR   101
#define ID_BTN_LOAD    102
#define ID_BTN_RUN     103
#define ID_BTN_STEP    104
#define ID_BTN_HALT    105
#define ID_BTN_HELP    106
#define ID_BTN_OPEN    107
#define ID_EDIT_CODE   201
#define ID_EDIT_ADDR   202

static Machine gMachine;
static HWND hCodeEdit = NULL, hAddrEdit = NULL;
static HFONT hTitleFont, hLabelFont, hMonoFont, hMonoBold, hCodeFont;
static string gStatusText = "READY (Idle)";

void DrawGuiCanvas(HDC hdc, int w, int h, const string& codeStr, const string& startAddrStr, bool drawEditText) {
    // Background
    HBRUSH bgBrush = CreateSolidBrush(RGB(243, 244, 246));
    RECT fullRect = {0, 0, w, h};
    FillRect(hdc, &fullRect, bgBrush);
    DeleteObject(bgBrush);

    // Top Header Banner
    HBRUSH headerBrush = CreateSolidBrush(RGB(30, 58, 138));
    RECT headerRect = {0, 0, w, 36};
    FillRect(hdc, &headerRect, headerBrush);
    DeleteObject(headerBrush);

    SetBkMode(hdc, TRANSPARENT);
    SelectObject(hdc, hTitleFont);
    SetTextColor(hdc, RGB(255, 255, 255));
    TextOutA(hdc, 16, 8, "Vole Machine Simulator GUI - CS213 (Section S39_S40) - Dr. Mohammad El-Ramly", 76);

    HPEN borderPen = CreatePen(PS_SOLID, 1, RGB(190, 195, 205));
    HBRUSH whiteBrush = CreateSolidBrush(RGB(255, 255, 255));
    HBRUSH panelHeaderBrush = CreateSolidBrush(RGB(238, 242, 255));
    HBRUSH yellowBrush = CreateSolidBrush(RGB(254, 243, 199));
    HBRUSH greenBrush = CreateSolidBrush(RGB(187, 247, 208));

    SelectObject(hdc, borderPen);
    SelectObject(hdc, whiteBrush);

    // Labels
    SelectObject(hdc, hLabelFont);
    SetTextColor(hdc, RGB(25, 30, 40));
    TextOutA(hdc, 20, 44, "Data Input Window (Vole Machine Program):", 41);
    TextOutA(hdc, 660, 44, "Start Address (Hex Even):", 25);
    TextOutA(hdc, 660, 80, "Memory-Mapped Screen Output (Address FF):", 41);
    TextOutA(hdc, 20, 224, "CPU & Registers (R0 - RF):", 26);
    TextOutA(hdc, 325, 224, "Main Memory (16 x 16 Matrix = 256 Cells):", 41);

    // If capturing offscreen (or behind child controls), draw Data Input & StartAddr boxes
    if (drawEditText) {
        Rectangle(hdc, 20, 68, 640, 214);
        SelectObject(hdc, hCodeFont);
        RECT codeRect = {26, 72, 634, 210};
        DrawTextA(hdc, codeStr.c_str(), -1, &codeRect, DT_LEFT | DT_TOP | DT_NOPREFIX);

        Rectangle(hdc, 845, 41, 910, 67);
        SelectObject(hdc, hMonoBold);
        TextOutA(hdc, 868, 46, startAddrStr.c_str(), (int)startAddrStr.length());

        HBRUSH btnBg = CreateSolidBrush(RGB(229, 231, 235));
        RECT openBtnRect = {925, 40, 1095, 68};
        FillRect(hdc, &openBtnRect, btnBg);
        FrameRect(hdc, &openBtnRect, (HBRUSH)GetStockObject(GRAY_BRUSH));
        SelectObject(hdc, hLabelFont);
        TextOutA(hdc, 942, 46, "Browse Program File...", 22);
        DeleteObject(btnBg);
    }

    // 1. Screen Output Box (660, 104, 1095, 214)
    SelectObject(hdc, whiteBrush);
    Rectangle(hdc, 660, 104, 1095, 214);

    const Screen& scr = gMachine.getScreen();
    string asciiOut = scr.getAsciiOutput().empty() ? "(empty)" : scr.getAsciiOutput();
    string hexOut = scr.getHexOutput().empty() ? "(empty)" : scr.getHexOutput();

    SelectObject(hdc, hMonoBold);
    SetTextColor(hdc, RGB(29, 78, 216));
    string l1 = "ASCII Output : " + asciiOut;
    TextOutA(hdc, 672, 118, l1.c_str(), (int)l1.length());

    SelectObject(hdc, hMonoFont);
    SetTextColor(hdc, RGB(25, 30, 40));
    string l2 = "Hex Stream   : " + hexOut;
    TextOutA(hdc, 672, 148, l2.c_str(), (int)l2.length());

    SelectObject(hdc, hMonoBold);
    string l3 = "Status       : " + gStatusText;
    TextOutA(hdc, 672, 178, l3.c_str(), (int)l3.length());

    // 2. CPU & 16 Registers Box (20, 248, 305, 658)
    Rectangle(hdc, 20, 248, 305, 658);
    RECT cpuHeadRect = {21, 249, 304, 283};
    FillRect(hdc, &cpuHeadRect, panelHeaderBrush);

    const CPU& cpu = gMachine.getCPU();
    const RegisterFile& regs = gMachine.getRegisterFile();
    const Memory& mem = gMachine.getMemory();

    stringstream pcss;
    pcss << "PC : " << uppercase << hex << setw(2) << setfill('0') << cpu.getPC()
         << "      IR : " << cpu.getIR();
    string pcIrStr = pcss.str();
    SelectObject(hdc, hMonoBold);
    SetTextColor(hdc, RGB(29, 78, 216));
    TextOutA(hdc, 34, 258, pcIrStr.c_str(), (int)pcIrStr.length());

    SetTextColor(hdc, RGB(25, 30, 40));
    TextOutA(hdc, 30, 290, "Reg   Hex   Binary      Dec", 27);
    MoveToEx(hdc, 25, 309, NULL);
    LineTo(hdc, 299, 309);

    for (int i = 0; i < 16; i++) {
        uint8_t val = regs.getRegister(i);
        int signedVal = (int)((int8_t)val);
        string hexReg = regs.getHexRegister(i);
        string binStr = bitset<8>(val).to_string();

        int ry = 314 + i * 21;
        if (val != 0) {
            RECT hiR = {23, ry - 1, 302, ry + 18};
            FillRect(hdc, &hiR, yellowBrush);
        }

        stringstream rowSS;
        rowSS << "R" << uppercase << hex << i << "    " << hexReg << "    " << binStr << "   "
              << dec << setw(4) << signedVal;
        string rowStr = rowSS.str();
        SelectObject(hdc, hMonoFont);
        SetTextColor(hdc, RGB(25, 30, 40));
        TextOutA(hdc, 30, ry, rowStr.c_str(), (int)rowStr.length());
    }

    // 3. Main Memory 16x16 Matrix Box (325, 248, 1095, 658)
    SelectObject(hdc, whiteBrush);
    Rectangle(hdc, 325, 248, 1095, 658);
    RECT memHeadRect = {326, 249, 1094, 277};
    FillRect(hdc, &memHeadRect, panelHeaderBrush);

    SelectObject(hdc, hMonoBold);
    SetTextColor(hdc, RGB(29, 78, 216));
    for (int c = 0; c < 16; c++) {
        stringstream css;
        css << uppercase << hex << c;
        string cs = css.str();
        TextOutA(hdc, 378 + c * 44, 255, cs.c_str(), (int)cs.length());
    }

    int curPC = cpu.getPC();
    for (int r = 0; r < 16; r++) {
        int ry = 282 + r * 23;
        stringstream rss;
        rss << uppercase << hex << r << " |";
        string rs = rss.str();
        SelectObject(hdc, hMonoBold);
        SetTextColor(hdc, RGB(29, 78, 216));
        TextOutA(hdc, 338, ry, rs.c_str(), (int)rs.length());

        SelectObject(hdc, hMonoFont);
        SetTextColor(hdc, RGB(25, 30, 40));
        for (int c = 0; c < 16; c++) {
            int addr = r * 16 + c;
            int cx = 374 + c * 44;
            string cellStr = mem.getHexCell(addr);

            if (addr == curPC || addr == curPC + 1) {
                RECT hiM = {cx - 3, ry - 1, cx + 25, ry + 18};
                FillRect(hdc, &hiM, greenBrush);
            } else if (mem.getCell(addr) != 0) {
                RECT hiM = {cx - 3, ry - 1, cx + 25, ry + 18};
                FillRect(hdc, &hiM, yellowBrush);
            }
            TextOutA(hdc, cx, ry, cellStr.c_str(), (int)cellStr.length());
        }
    }

    // If capturing offscreen, also draw the bottom buttons so the snapshot matches 100%
    if (drawEditText) {
        const char* btnNames[6] = {"Clear Memory", "Load Data", "Run", "Single Step", "Halt", "Help"};
        HBRUSH grayBtn = CreateSolidBrush(RGB(229, 231, 235));
        for (int b = 0; b < 6; b++) {
            int bx = 135 + b * 145;
            int by = 670;
            RECT br = {bx, by, bx + 130, by + 36};
            FillRect(hdc, &br, grayBtn);
            FrameRect(hdc, &br, (HBRUSH)GetStockObject(GRAY_BRUSH));
            SelectObject(hdc, hLabelFont);
            SetTextColor(hdc, RGB(25, 30, 40));
            DrawTextA(hdc, btnNames[b], -1, &br, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        }
        DeleteObject(grayBtn);
    }

    DeleteObject(borderPen);
    DeleteObject(whiteBrush);
    DeleteObject(panelHeaderBrush);
    DeleteObject(yellowBrush);
    DeleteObject(greenBrush);
}

bool SaveCanvasBmp(const string& filename, const string& codeStr, const string& startAddrStr) {
    int w = 1115;
    int h = 720;
    HDC screenDC = GetDC(NULL);
    HDC memDC = CreateCompatibleDC(screenDC);
    HBITMAP hBmp = CreateCompatibleBitmap(screenDC, w, h);
    HBITMAP oldBmp = (HBITMAP)SelectObject(memDC, hBmp);

    DrawGuiCanvas(memDC, w, h, codeStr, startAddrStr, true);

    BITMAP bmp;
    GetObject(hBmp, sizeof(BITMAP), &bmp);

    BITMAPFILEHEADER bmfHeader = {0};
    BITMAPINFOHEADER bi = {0};
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = bmp.bmWidth;
    bi.biHeight = bmp.bmHeight;
    bi.biPlanes = 1;
    bi.biBitCount = 24;
    bi.biCompression = BI_RGB;

    DWORD dwBmpSize = ((bmp.bmWidth * 24 + 31) / 32) * 4 * bmp.bmHeight;
    char* lpbitmap = new char[dwBmpSize];
    GetDIBits(memDC, hBmp, 0, (UINT)bmp.bmHeight, lpbitmap, (BITMAPINFO*)&bi, DIB_RGB_COLORS);

    bmfHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    bmfHeader.bfSize = dwBmpSize + bmfHeader.bfOffBits;
    bmfHeader.bfType = 0x4D42;

    ofstream out(filename, ios::binary);
    out.write((char*)&bmfHeader, sizeof(BITMAPFILEHEADER));
    out.write((char*)&bi, sizeof(BITMAPINFOHEADER));
    out.write(lpbitmap, dwBmpSize);
    out.close();

    delete[] lpbitmap;
    SelectObject(memDC, oldBmp);
    DeleteObject(hBmp);
    DeleteDC(memDC);
    ReleaseDC(NULL, screenDC);
    return true;
}

void LoadEditorIntoMachine(HWND hwnd) {
    int len = GetWindowTextLengthA(hCodeEdit);
    string text(len + 1, '\0');
    GetWindowTextA(hCodeEdit, &text[0], len + 1);
    text.resize(len);

    ofstream out("temp_vole_gui.txt");
    for (size_t i = 0; i < text.size(); i++) {
        if (text[i] != '\r') out << text[i];
    }
    out.close();

    char addrBuf[32] = {0};
    GetWindowTextA(hAddrEdit, addrBuf, 31);
    int startAddr = 0x10;
    try {
        startAddr = stoi(string(addrBuf), 0, 16);
    } catch (...) {
        MessageBoxA(hwnd, "Invalid hex start address.", "Error", MB_ICONERROR);
        return;
    }

    gMachine.reset(startAddr);
    if (!gMachine.loadProgram("temp_vole_gui.txt", startAddr)) {
        gStatusText = "ERROR (Invalid Program)";
        InvalidateRect(hwnd, NULL, FALSE);
        MessageBoxA(hwnd, "Invalid instruction or odd start address! Program rejected.", "Load Error", MB_ICONWARNING);
        return;
    }
    remove("temp_vole_gui.txt");
    gStatusText = "LOADED (Ready to Run)";
    InvalidateRect(hwnd, NULL, FALSE);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: {
            hCodeEdit = CreateWindowExA(WS_EX_CLIENTEDGE, "EDIT",
                "; Print FCAI on Screen (3 RFF)\r\n2146 ; R1 = 'F' (46h)\r\n31FF ; Store R1 to Screen (FF)\r\n2143 ; R1 = 'C' (43h)\r\n31FF ; Store R1 to Screen (FF)\r\n2141 ; R1 = 'A' (41h)\r\n31FF ; Store R1 to Screen (FF)\r\n2149 ; R1 = 'I' (49h)\r\n31FF ; Store R1 to Screen (FF)\r\nC000 ; Halt execution",
                WS_VISIBLE | WS_CHILD | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL,
                20, 68, 620, 146, hwnd, (HMENU)ID_EDIT_CODE, NULL, NULL);

            hAddrEdit = CreateWindowExA(WS_EX_CLIENTEDGE, "EDIT", "10",
                WS_VISIBLE | WS_CHILD | ES_CENTER,
                845, 41, 65, 26, hwnd, (HMENU)ID_EDIT_ADDR, NULL, NULL);

            HWND hOpenBtn = CreateWindowA("BUTTON", "Browse Program File...", WS_VISIBLE | WS_CHILD,
                925, 40, 170, 28, hwnd, (HMENU)ID_BTN_OPEN, NULL, NULL);

            HWND b1 = CreateWindowA("BUTTON", "Clear Memory", WS_VISIBLE | WS_CHILD, 135, 670, 130, 36, hwnd, (HMENU)ID_BTN_CLEAR, NULL, NULL);
            HWND b2 = CreateWindowA("BUTTON", "Load Data",    WS_VISIBLE | WS_CHILD, 280, 670, 130, 36, hwnd, (HMENU)ID_BTN_LOAD,  NULL, NULL);
            HWND b3 = CreateWindowA("BUTTON", "Run",          WS_VISIBLE | WS_CHILD, 425, 670, 130, 36, hwnd, (HMENU)ID_BTN_RUN,   NULL, NULL);
            HWND b4 = CreateWindowA("BUTTON", "Single Step",  WS_VISIBLE | WS_CHILD, 570, 670, 130, 36, hwnd, (HMENU)ID_BTN_STEP,  NULL, NULL);
            HWND b5 = CreateWindowA("BUTTON", "Halt",         WS_VISIBLE | WS_CHILD, 715, 670, 130, 36, hwnd, (HMENU)ID_BTN_HALT,  NULL, NULL);
            HWND b6 = CreateWindowA("BUTTON", "Help",         WS_VISIBLE | WS_CHILD, 860, 670, 130, 36, hwnd, (HMENU)ID_BTN_HELP,  NULL, NULL);

            SendMessage(hCodeEdit, WM_SETFONT, (WPARAM)hCodeFont, TRUE);
            SendMessage(hAddrEdit, WM_SETFONT, (WPARAM)hMonoBold, TRUE);
            SendMessage(hOpenBtn,  WM_SETFONT, (WPARAM)hLabelFont, TRUE);
            SendMessage(b1, WM_SETFONT, (WPARAM)hLabelFont, TRUE);
            SendMessage(b2, WM_SETFONT, (WPARAM)hLabelFont, TRUE);
            SendMessage(b3, WM_SETFONT, (WPARAM)hLabelFont, TRUE);
            SendMessage(b4, WM_SETFONT, (WPARAM)hLabelFont, TRUE);
            SendMessage(b5, WM_SETFONT, (WPARAM)hLabelFont, TRUE);
            SendMessage(b6, WM_SETFONT, (WPARAM)hLabelFont, TRUE);
            break;
        }
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            RECT rc;
            GetClientRect(hwnd, &rc);
            DrawGuiCanvas(hdc, rc.right, rc.bottom, "", "", false);
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_COMMAND: {
            int id = LOWORD(wParam);
            if (id == ID_BTN_CLEAR) {
                gMachine.reset(0);
                gStatusText = "READY (Memory Cleared)";
                InvalidateRect(hwnd, NULL, FALSE);
            } else if (id == ID_BTN_LOAD) {
                LoadEditorIntoMachine(hwnd);
            } else if (id == ID_BTN_RUN) {
                gMachine.runAll();
                gStatusText = gMachine.getCPU().isHalted() ? "HALTED (C000 Reached)" : "RUNNING";
                InvalidateRect(hwnd, NULL, FALSE);
            } else if (id == ID_BTN_STEP) {
                gMachine.step();
                gStatusText = gMachine.getCPU().isHalted() ? "HALTED (C000 Reached)" : "RUNNING (Step-by-Step)";
                InvalidateRect(hwnd, NULL, FALSE);
            } else if (id == ID_BTN_HALT) {
                gStatusText = "HALTED (Stopped by User)";
                InvalidateRect(hwnd, NULL, FALSE);
            } else if (id == ID_BTN_OPEN) {
                char fileName[MAX_PATH] = "";
                OPENFILENAMEA ofn = {0};
                ofn.lStructSize = sizeof(ofn);
                ofn.hwndOwner = hwnd;
                ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
                ofn.lpstrFile = fileName;
                ofn.nMaxFile = MAX_PATH;
                ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
                if (GetOpenFileNameA(&ofn)) {
                    ifstream in(fileName);
                    stringstream ss;
                    string line;
                    while (getline(in, line)) {
                        ss << line << "\r\n";
                    }
                    SetWindowTextA(hCodeEdit, ss.str().c_str());
                    LoadEditorIntoMachine(hwnd);
                }
            } else if (id == ID_BTN_HELP) {
                MessageBoxA(hwnd,
                    "Vole Machine Simulator GUI (CS213 - Section S39_S40)\n\n"
                    "1. Enter Vole instructions in the Data Input Window or click 'Browse Program File...'\n"
                    "2. Set an even hex Start Address (e.g., 10) and click 'Load Data'\n"
                    "3. Click 'Single Step' to execute one instruction or 'Run' to execute all\n"
                    "4. Address FF is memory-mapped to the Screen Output panel.",
                    "Vole Simulator Help", MB_ICONINFORMATION);
            }
            break;
        }
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcA(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR lpCmdLine, int nCmdShow) {
    hTitleFont = CreateFontA(18, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, "Segoe UI");
    hLabelFont = CreateFontA(16, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, "Segoe UI");
    hMonoFont = CreateFontA(15, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, FIXED_PITCH | FF_MODERN, "Consolas");
    hMonoBold = CreateFontA(15, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, FIXED_PITCH | FF_MODERN, "Consolas");
    hCodeFont = CreateFontA(14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, FIXED_PITCH | FF_MODERN, "Consolas");

    string cmd(lpCmdLine ? lpCmdLine : "");
    if (cmd == "--capture-snapshots") {
        string sampleCode =
            "; Print FCAI on Screen (3 RFF)\r\n"
            "2146 ; R1 = 'F' (46h)\r\n"
            "31FF ; Store R1 to Screen (FF)\r\n"
            "2143 ; R1 = 'C' (43h)\r\n"
            "31FF ; Store R1 to Screen (FF)\r\n"
            "2141 ; R1 = 'A' (41h)\r\n"
            "31FF ; Store R1 to Screen (FF)\r\n"
            "2149 ; R1 = 'I' (49h)\r\n"
            "31FF ; Store R1 to Screen (FF)\r\n"
            "C000 ; Halt execution";

        gMachine.reset(0);
        gStatusText = "READY (Idle)";
        SaveCanvasBmp("Snapshots/1_GUI_Initial_State.bmp", sampleCode, "10");

        gMachine.loadProgram("programs/prog9_print_fcai.txt", 0x10);
        gStatusText = "LOADED (Ready to Run)";
        SaveCanvasBmp("Snapshots/2_GUI_Program_Loaded.bmp", sampleCode, "10");

        for (int i = 0; i < 4; i++) gMachine.step();
        gStatusText = "RUNNING (Step-by-Step)";
        SaveCanvasBmp("Snapshots/3_GUI_Single_Step_Execution.bmp", sampleCode, "10");

        gMachine.runAll();
        gStatusText = "HALTED (C000 Reached)";
        SaveCanvasBmp("Snapshots/4_GUI_Full_Run_Output_FCAI.bmp", sampleCode, "10");
        return 0;
    }

    WNDCLASSA wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.hbrBackground = CreateSolidBrush(RGB(243, 244, 246));
    wc.lpszClassName = "VoleGuiClass";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClassA(&wc);

    HWND hwnd = CreateWindowA("VoleGuiClass",
        "Vole Machine Simulator GUI - CS213 (Section S39_S40)",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 1130, 760,
        NULL, NULL, hInst, NULL);

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}
