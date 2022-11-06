
#include "imgUtil.h"

bool ImgUtil::getBinaryDataByImgHandle(HBITMAP& in_hbitmap, byte** out_data, int* out_length, int* out_width, int* out_height) {

    HDC hdcScreen = GetDC(NULL);

    BITMAPINFO bitmapInfo = { 0 };
    bitmapInfo.bmiHeader.biSize = sizeof(bitmapInfo.bmiHeader);
    GetDIBits(hdcScreen, in_hbitmap, 0, 0, NULL, &bitmapInfo, DIB_RGB_COLORS);

    BITMAPINFOHEADER& bmiHeader = bitmapInfo.bmiHeader;
    bmiHeader.biCompression = BI_RGB;

    byte* lpbitmap = new byte[bmiHeader.biSizeImage];
    memset(lpbitmap, 0, bmiHeader.biSizeImage);

    GetDIBits(hdcScreen, in_hbitmap, 0, (UINT)bmiHeader.biHeight, lpbitmap, (BITMAPINFO*)&bmiHeader, DIB_RGB_COLORS);

    // Add the size of the headers to the size of the bitmap to get the total file size.
    DWORD dwSizeofDIB = bmiHeader.biSizeImage + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    // Offset to where the actual bitmap bits start.
    BITMAPFILEHEADER   bmfHeader;
    bmfHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER);

    // Size of the file.
    bmfHeader.bfSize = dwSizeofDIB;

    // bfType must always be BM for Bitmaps.
    bmfHeader.bfType = 0x4D42; // BM.

    if (lpbitmap == NULL)
        return false;

    byte* imgdata = new byte[dwSizeofDIB]();
    byte* index = imgdata;
    memcpy(index, &bmfHeader, sizeof(BITMAPFILEHEADER)); index += sizeof(BITMAPFILEHEADER);
    memcpy(index, &bmiHeader, sizeof(BITMAPINFOHEADER)); index += sizeof(BITMAPINFOHEADER);
    memcpy(index, lpbitmap, bmiHeader.biSizeImage);

    *out_data = imgdata;
    *out_length = dwSizeofDIB;
    *out_width = bmiHeader.biWidth;
    *out_height = bmiHeader.biHeight;

    delete[] lpbitmap;
    ReleaseDC(NULL, hdcScreen);

    return true;

}
 

bool ImgUtil::getBinaryDataByIconHandle(HICON& in_hIcon, byte** out_data, int* out_length, int* out_width, int* out_height) {
    if (in_hIcon == NULL) {
        return false;
    }

    // Get the icon info
    ICONINFO iconInfo = { 0 };
    GetIconInfo(in_hIcon, &iconInfo);

    // Get the screen DC
    HDC hdcScreen = GetDC(NULL);

    // Get icon size info
    BITMAP bm = { 0 };
    GetObject(iconInfo.hbmColor, sizeof(BITMAP), &bm);
    //glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    // Set up BITMAPINFO
    BITMAPINFO bmi = { 0 };
    BITMAPINFOHEADER& bmiHeader = bmi.bmiHeader;
    bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmiHeader.biWidth = bm.bmWidth;
    bmiHeader.biHeight = -bm.bmHeight;
    bmiHeader.biPlanes = 1;
    bmiHeader.biBitCount = 32;
    bmiHeader.biCompression = BI_RGB;

    // Extract the color bitmap
    int nBits = bm.bmWidth * bm.bmHeight;
    int32_t* lpbitmap = new int32_t[nBits];
    GetDIBits(hdcScreen, iconInfo.hbmColor, 0, bm.bmHeight, lpbitmap, &bmi, DIB_RGB_COLORS);

    // Add the size of the headers to the size of the bitmap to get the total file size.
    DWORD dwSizeofDIB = bmi.bmiHeader.biSizeImage + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    // Offset to where the actual bitmap bits start.
    BITMAPFILEHEADER   bmfHeader;
    bmfHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER);

    // Size of the file.
    bmfHeader.bfSize = dwSizeofDIB;

    // bfType must always be BM for Bitmaps.
    bmfHeader.bfType = 0x4D42; // BM.

    if (lpbitmap == NULL)
        return false;

    byte* imgdata = new byte[dwSizeofDIB]();
    byte* index = imgdata;
    memcpy(index, &bmfHeader, sizeof(BITMAPFILEHEADER)); index += sizeof(BITMAPFILEHEADER);
    memcpy(index, &bmiHeader, sizeof(BITMAPINFOHEADER)); index += sizeof(BITMAPINFOHEADER);
    memcpy(index, lpbitmap, bmiHeader.biSizeImage);

    *out_data = imgdata;
    *out_length = dwSizeofDIB;
    *out_width = bmiHeader.biWidth;
    *out_height = bmiHeader.biHeight;

    delete[] lpbitmap;
    ReleaseDC(NULL, hdcScreen);
}

