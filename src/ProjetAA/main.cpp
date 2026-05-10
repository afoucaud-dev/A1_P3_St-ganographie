//C:\Users\fouca\source\repos\ProjetImage\montagne_3232.bmp
#include "pch.h"
HWND hEdit;
// Identificateur du bouton
#define ID_BTN_HELLO 1001
HBITMAP g_hBitmap = NULL;
HBITMAP g_hBitmap2 = NULL;

// Prototype de la procédure de fenêtre
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
FILE* image;
BYTE* bufferImage = new BYTE[1000000000]; // 10 MB pour être large
BYTE* bufferResult = new BYTE[1000000000]; // 10 MB pour être large
std::wstring g_textFinal = L"";
std::wstring mdp;

std::wstring Decoder(BYTE* buffer, const std::wstring& mdp)
{
    // Ouvrir le fichier BMP choisi
    FILE* f = nullptr;
    if (_wfopen_s(&f, L"resultat.bmp", L"rb") != 0 || f == nullptr)
    {
        MessageBox(nullptr, L"Impossible d'ouvrir l'image choisie.", L"Erreur", MB_OK | MB_ICONERROR);
        return 0;
    }

    // Lire headers BMP
    BITMAPFILEHEADER fileHeader;
    BITMAPINFOHEADER infoHeader;

    fread(&fileHeader, sizeof(fileHeader), 1, f);
    fread(&infoHeader, sizeof(infoHeader), 1, f);

    int width = infoHeader.biWidth;
    int height = infoHeader.biHeight;

    // Lire pixels
    fseek(f, fileHeader.bfOffBits, SEEK_SET);
    fread(buffer, 1, fileHeader.bfSize - fileHeader.bfOffBits, f);

    fclose(f);

    BYTE* pixelData = buffer;
    int padding = (4 - ((width * 3) % 4)) % 4;

    int nbrchar = 0;
    BYTE charmdp;
    std::wstring secret;
    for (int y = 0; y < height; y++)
    {
        BYTE* row = pixelData + y * (width * 3 + padding);

        for (int x = 0; x < width && nbrchar < mdp.size(); x++)
        {
            int idx = x * 3;
            BYTE B = row[idx];
            BYTE G = row[idx + 1];
            BYTE R = row[idx + 2];

            if (x % 2 == 0)
            {
                charmdp = 0;
                R <<= 6;
                R >>= 6;
                charmdp |= R;
                charmdp <<= 1;

                G <<= 7;
                G >>= 7;
                charmdp |= G;
                charmdp <<= 1;

                B <<= 7;
                B >>= 7;
                charmdp |= B;
                charmdp <<= 2;
            }

            else
            {
                R <<= 6;
                R >>= 6;
                charmdp |= R;
                charmdp <<= 1;

                G <<= 7;
                G >>= 7;
                charmdp |= G;
                charmdp <<= 1;

                B <<= 7;
                B >>= 7;
                charmdp |= B;
                //C:\Users\amollaret\Pictures\MainBefore.bmp
                
                secret.push_back((char)charmdp);
                

               
                nbrchar++;
               
                
            }
           
            
        }

    }
   
    return secret;
}

void NewPicture(const std::wstring& fileName, BYTE* buffer, const std::wstring& mdp)
{
    // Ouvrir le fichier BMP choisi
    FILE* f = nullptr;
    if (_wfopen_s(&f, fileName.c_str(), L"rb") != 0 || f == nullptr)
    {
        MessageBox(nullptr, L"Impossible d'ouvrir l'image choisie.", L"Erreur", MB_OK | MB_ICONERROR);
        return;
    }

    fread(buffer, sizeof(BYTE), 1000000000, f);
    fclose(f);

    // Lire headers BMP
    BITMAPFILEHEADER* fileHeader = (BITMAPFILEHEADER*)buffer;
    BITMAPINFOHEADER* infoHeader = (BITMAPINFOHEADER*)(buffer + sizeof(BITMAPFILEHEADER));

    int width = infoHeader->biWidth;
    int height = infoHeader->biHeight;

    fread(&fileHeader, sizeof(fileHeader), 1, f);
    fread(&infoHeader, sizeof(infoHeader), 1, f);

    BYTE* pixelData = buffer + fileHeader->bfOffBits;
    int padding = (4 - ((width * 3) % 4)) % 4;

    
    //fseek(f, fileHeader.bfOffBits, SEEK_SET);
    
    int nbrchar = 0;

    for (int y = 0; y < height; y++)
    {
        BYTE* row = pixelData + y * (width * 3 + padding);

        for (int x = 0; x < width; x++)
        {
            int idx = x * 3;
            BYTE B = row[idx];
            BYTE G = row[idx + 1];
            BYTE R = row[idx + 2];


            if (nbrchar < mdp.size())
            {
                if (x % 2 == 0)
                {
                    BYTE bitR = mdp[nbrchar] >> 6;
                    BYTE bitG = mdp[nbrchar] << 2;
                    bitG >>= 7;
                    BYTE bitB = mdp[nbrchar] << 3;
                    bitB >>= 7;

                    R = ((R >> 2) << 2) | bitR;
                    R = 255;
                    G = (G >> 1) << 1 | bitG;
                    B = (B >> 1) << 1 | bitB;
                }
                else
                {
                    BYTE bitR = (mdp[nbrchar] << 4);
                    bitR >>= 6;
                    BYTE bitG = (mdp[nbrchar] << 6);
                    bitG >>= 7;
                    BYTE bitB = (mdp[nbrchar] << 7);
                    bitB >>= 7;

                    R = (R >> 2) << 2 | bitR;
                    G = (G >> 1) << 1 | bitG;
                    B = (B >> 1) << 1 | bitB;

                    nbrchar++;
                }
            }

            row[idx] = B;
            row[idx + 1] = G;
            row[idx + 2] = R;


        }
    }


    FILE* output;
    if (_wfopen_s(&output, L"resultat.bmp", L"wb") == 0)
    {
        fwrite(buffer, 1, fileHeader->bfSize, output);
        fclose(output);
    }
    else
    {
        MessageBox(nullptr, L"Impossible de créer resultat.bmp", L"Erreur", MB_OK | MB_ICONERROR);
        return;
    }

    /*fwrite(&fileHeader, sizeof(fileHeader), 1, out);
    fwrite(&infoHeader, sizeof(infoHeader), 1, out);
    fwrite(buffer, 1, fileHeader.bfSize - fileHeader.bfOffBits, out);

    fclose(out);*/

    delete[] buffer;
}

// Point d'entrée pour une application Windows (pas de console)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    
    (void)hPrevInstance; (void)lpCmdLine;

    // 1) Déclarer et enregistrer la classe de fenêtre
    const wchar_t CLASS_NAME[] = L"MaClasseFenetre";

    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = CLASS_NAME;
    wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);

    if (!RegisterClassEx(&wc)) {
        MessageBox(nullptr, L"Impossible d'enregistrer la classe de fenêtre", L"Erreur", MB_OK | MB_ICONERROR);
        return 0;
    }

    // 2) Créer la fenêtre
    HWND hwnd = CreateWindowEx(
        0,                              // styles étendus
        CLASS_NAME,                     // nom de la classe
        L"fenetreee", // titre de la fenêtre
        WS_OVERLAPPEDWINDOW,            // style de la fenêtre
        CW_USEDEFAULT, CW_USEDEFAULT,   // position
        800, 600,                       // taille
        nullptr,                        // fenêtre parente
        nullptr,                        // menu
        hInstance,                      // instance
        nullptr                         // paramètres création
    );
   

    if (!hwnd) {
        MessageBox(nullptr, L"Échec de CreateWindowEx", L"Erreur", MB_OK | MB_ICONERROR);
        return 0;
    }

    ShowWindow(hwnd, nShowCmd);
    UpdateWindow(hwnd);

    // 3) Boucle de messages
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return static_cast<int>(msg.wParam);
}

// 4) Procédure de fenêtre
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    RECT rect;
    RECT rect2;
    RECT rc;
    GetClientRect(hwnd, &rc);
    switch (msg) {
    case WM_CREATE:
    {
        HINSTANCE hInst = ((LPCREATESTRUCT)lParam)->hInstance;
        CreateWindowEx(
            0, L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_OEMCONVERT | ES_LEFT, 
            10, 50, 200, 25,
            hwnd,
            (HMENU)100,   // <----- ID du champ de texte
            hInst, NULL
        );

        CreateWindowEx(
            0, L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_OEMCONVERT | ES_LEFT,
            10, 340, 200, 25,
            hwnd,
            (HMENU)102,   // <----- ID du champ de texte
            hInst, NULL
        );

        CreateWindow(
            L"BUTTON",
            L"Valider",
            WS_CHILD | WS_VISIBLE,
            220, 50, 100, 25,
            hwnd,
            (HMENU)101,   // <----- ID du bouton
            hInst,
            NULL
        );
        CreateWindow(
            L"BUTTON",
            L"Valider",
            WS_CHILD | WS_VISIBLE,
            220, 340, 100, 25,
            hwnd,
            (HMENU)104,   // <----- ID du bouton
            hInst,
            NULL
        );
        CreateWindow(
            L"BUTTON",
            L"DECODER !",
            WS_CHILD | WS_VISIBLE,
            500, 500, 100, 25,
            hwnd,
            (HMENU)105,   // <----- ID du bouton
            hInst,
            NULL
        );
    }
    return 0;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case 101:  // Bouton Valider cliqué
        {
            InvalidateRect(hwnd, NULL, TRUE);
            wchar_t buffer[256];

            // Récupérer le handle du champ de texte
            HWND hEdit = GetDlgItem(hwnd, 100);

            // Lire le texte
            GetWindowText(hEdit, buffer, 256);

           

            int len = GetWindowTextLength(hEdit);
            std::wstring texte(len, L'\0');

            GetWindowText(hEdit, &texte[0], len + 1);
            // Afficher le texte
            g_hBitmap = (HBITMAP)LoadImage(
                NULL,
                texte.c_str(),
                IMAGE_BITMAP,
                200, 200,               // taille originale = 0, 0
                LR_LOADFROMFILE     
            );
            
            if (!g_hBitmap) {
                MessageBox(hwnd, L"Impossible de charger le BMP.", L"Erreur", MB_OK | MB_ICONERROR);
            }
            break;
           
          
        }
        case 104:  // Bouton Valider cliqué
        {
           
            InvalidateRect(hwnd, NULL, TRUE);
            wchar_t fileName[256];
            wchar_t buffer[256];
            GetWindowText(GetDlgItem(hwnd, 100), fileName, 256);
            // Récupérer le handle du champ de texte
            HWND hEdit = GetDlgItem(hwnd, 102);

            // Lire le texte
            GetWindowText(hEdit, buffer, 256);



            int len = GetWindowTextLength(hEdit);
            
            mdp = (len, L'\0');
          
            NewPicture(fileName, bufferImage, mdp);
            InvalidateRect(hwnd, NULL, TRUE);
            g_hBitmap2 = (HBITMAP)LoadImage(
                NULL,
                L"resultat.bmp",
                IMAGE_BITMAP,
                200, 200,               // taille originale = 0, 0
                LR_LOADFROMFILE
            );
            break;


        }
        case 105:  // Bouton Decoder cliqué
        {

            g_textFinal = Decoder(bufferResult, mdp);   
            
            MessageBox(hwnd, g_textFinal.c_str(), L"r", MB_OK | MB_ICONERROR);
            InvalidateRect(hwnd, NULL, TRUE);
            break;


        }
        }
  
        return 0;

    case WM_PAINT:
    {

        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
       

        const wchar_t* texte = L"STEGANOGRAPHIE";
        TextOut(hdc, rc.right/2 - rc.right/20,10, texte, (int)wcslen(texte));  
        const wchar_t* texte2 = L"ENTRE TON CODE SECRET :";
        TextOut(hdc,10, 315, texte2, (int)wcslen(texte2));
        const wchar_t* texte3 = L"CHOISI TON IMAGE : (chemin d'acces)";
        TextOut(hdc, 10, 30, texte3, (int)wcslen(texte3));
        
        if (!g_textFinal.empty())
        {
            TextOut(hdc, 200, 500, g_textFinal.c_str(), (int)g_textFinal.length());
        }

        SetRect(&rect, 9, 89, 211, 291);
        SetRect(&rect2, 9, 389, 211, 591);
        Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
        Rectangle(hdc, rect2.left, rect2.top, rect2.right, rect2.bottom);
        if (g_hBitmap)
        {
            HDC hdcMem = CreateCompatibleDC(hdc);
            SelectObject(hdcMem, g_hBitmap);

            BITMAP bmp;
            GetObject(g_hBitmap, sizeof(bmp), &bmp);

            BitBlt(
                hdc,
                10, 90,                 // position dans ta fenêtre
                bmp.bmWidth,
                bmp.bmHeight,
                hdcMem,
                0, 0,
                SRCCOPY
            );
            
            DeleteDC(hdcMem);
        }
      
        if (g_hBitmap2)
        {
            HDC hdcMem = CreateCompatibleDC(hdc);
            SelectObject(hdcMem, g_hBitmap2);

            BITMAP bmp;
            GetObject(g_hBitmap2, sizeof(bmp), &bmp);

            BitBlt(
                hdc,
                10, 390,                 // position dans ta fenêtre
                bmp.bmWidth,
                bmp.bmHeight,
                hdcMem,
                0, 0,
                SRCCOPY
            );

            DeleteDC(hdcMem);
        }
        EndPaint(hwnd, &ps);
    }
    return 0;

    case WM_DESTROY:
        DeleteFile(L"C:/Users/amollaret/source/repos/lyo-1-c64-s1p4-07/ide/ProjetAA/resultat.bmp");
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}
