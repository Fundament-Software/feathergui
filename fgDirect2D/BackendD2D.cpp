// Copyright (c)2021 Fundament Software
// For conditions of distribution and use, see copyright notice in "Backend.h"

#include "BackendD2D.h"
#include "Window.h"
#include "util.h"
#include <stdint.h>
#include <wincodec.h>
#include <dwrite_1.h>
#include <malloc.h>
#include <signal.h>
#include <codecvt>
#include <filesystem>
#include <dwmapi.h>
#include <shellscalingapi.h>
#include "RoundRect.h"
#include "Circle.h"
#include "Arc.h"
#include "Triangle.h"
#include "Modulation.h"

using namespace D2D;
namespace fs = std::filesystem;

typedef HRESULT(STDAPICALLTYPE* DWMCOMPENABLE)(BOOL*);
typedef HRESULT(STDAPICALLTYPE* DWMBLURBEHIND)(HWND, const DWM_BLURBEHIND*);
typedef HRESULT(STDAPICALLTYPE* GETDPIFORMONITOR)(HMONITOR, int, UINT*, UINT*);
typedef HRESULT(STDAPICALLTYPE* GETSCALEFACTORFORMONITOR)(HMONITOR, int*);

static std::unique_ptr<struct HINSTANCE__, void (*)(struct HINSTANCE__*)> shcoreD2D(LoadLibraryW(L"Shcore.dll"),
                                                                                    [](HMODULE h) { FreeLibrary(h); });

FG_Err Backend::DrawD2D(FG_Backend* self, FG_Window* window, FG_Command* commandlist, unsigned int n_commands,
                        FG_BlendState* blend)
{
  if(!self || !window)
    return -1;

  auto backend = static_cast<Backend*>(self);
  auto context = static_cast<Window*>(window);

  for(unsigned int i = 0; i < n_commands; ++i)
  {
    auto& c = commandlist[i];
    switch(c.category)
    {
    case FG_Category_ARC:
      context->DrawArc(*c.shape.area, c.shape.arc.angles, c.shape.fillColor, c.shape.border, c.shape.borderColor,
                       c.shape.blur, c.shape.arc.innerRadius, c.shape.asset, c.shape.z);
      break;
    case FG_Category_CIRCLE:
      context->DrawCircle(*c.shape.area, c.shape.fillColor, c.shape.border, c.shape.borderColor, c.shape.blur,
                          c.shape.circle.innerRadius, c.shape.circle.innerBorder, c.shape.asset, c.shape.z);
      break;
    case FG_Category_RECT:
      context->DrawRect(*c.shape.area, *c.shape.rect.corners, c.shape.fillColor, c.shape.border, c.shape.borderColor,
                        c.shape.blur, c.shape.asset, c.shape.rect.rotate, c.shape.z);
      break;
    case FG_Category_TRIANGLE:
      context->DrawTriangle(*c.shape.area, *c.shape.triangle.corners, c.shape.fillColor, c.shape.border,
                            c.shape.borderColor, c.shape.blur, c.shape.asset, c.shape.triangle.rotate, c.shape.z);
      break;
    case FG_Category_TEXT:
      context->DrawTextD2D(c.text.font, c.text.layout, c.text.area, c.text.color, c.text.blur, c.text.rotate, c.text.z);
      break;
    case FG_Category_ASSET:
      context->DrawAsset(c.asset.asset, *c.asset.area, c.asset.source, c.asset.color, c.asset.time, c.asset.rotate,
                         c.asset.z);
      break;
    case FG_Category_LINES: context->DrawLines(c.lines.points, c.lines.count, c.lines.color); break;
    case FG_Category_CURVE:
      context->DrawCurve(c.curve.points, c.curve.count, c.curve.fillColor, c.curve.stroke, c.curve.strokeColor);
      break;
    case FG_Category_SHADER:
      context->DrawShader(c.shader.shader, c.shader.vertices, c.shader.indices, c.shader.values);
      break;
    default: return -20;
    }
  }
  return 0;
}

bool Backend::Clear(FG_Backend* self, FG_Window* window, FG_Color color)
{
  auto context = static_cast<Window*>(window);
  context->Clear(color);
  return true;
}

FG_Err Backend::PushLayer(FG_Backend* self, FG_Window* window, FG_Asset* asset, float* transform, float opacity,
                          FG_BlendState* blend)
{
  if(!window || !transform)
    return -1;
  auto context = static_cast<Window*>(window);
  auto layer   = reinterpret_cast<ID2D1Layer*>(asset->data.data);
  auto size    = layer->GetSize();

  // TODO: Properly project 3D transform into a transposed 2D transform
  context->PushTransform(
    D2D1::Matrix3x2F(transform[0], transform[4], transform[1], transform[5], transform[12], transform[13]));

  // We only need a proper layer if we are doing opacity, otherwise the transform is sufficient
  if(opacity != 1.0)
  {
    context->layers.push(asset);

    D2D1_LAYER_PARAMETERS params = {
      D2D1::RectF(0, 0, size.width, size.height),  0, D2D1_ANTIALIAS_MODE_ALIASED, D2D1::IdentityMatrix(), opacity, 0,
      D2D1_LAYER_OPTIONS_INITIALIZE_FOR_CLEARTYPE,
    };

    context->target->PushLayer(params, layer);
  }
  else
    context->layers.push(nullptr);

  context->PushClip(FG_Rect{ 0, 0, size.width, size.height });
  return 0;
}

FG_Err Backend::PopLayer(FG_Backend* self, FG_Window* window)
{
  if(!window)
    return -1;
  auto context = static_cast<Window*>(window);
  auto p       = context->layers.top();
  context->PopClip();
  context->layers.pop();
  if(p)
    context->target->PopLayer();
  context->PopTransform();

  return 0;
}

FG_Err Backend::SetRenderTarget(FG_Backend* self, FG_Window* window, FG_Asset* target) { return -1; }

FG_Err Backend::PushClip(FG_Backend* self, FG_Window* window, FG_Rect* area)
{
  if(!window || !area)
    return -1;
  static_cast<Window*>(window)->PushClip(*area);
  return 0;
}

FG_Err Backend::PopClip(FG_Backend* self, FG_Window* window)
{
  if(!window)
    return -1;
  static_cast<Window*>(window)->PopClip();
  return 0;
}

FG_Err Backend::DirtyRect(FG_Backend* self, FG_Window* window, FG_Rect* area)
{
  static_cast<Window*>(window)->InvalidateHWND(area);
  return 0;
}

void* Backend::CreateSystemControl(FG_Backend* self, FG_Window* window, const char* id, FG_Rect* area, ...) { return 0; }
FG_Err Backend::SetSystemControl(FG_Backend* self, FG_Window* window, void* control, FG_Rect* area, ...) { return -1; }
FG_Err Backend::DestroySystemControl(FG_Backend* self, FG_Window* window, void* control) { return -1; }

FG_Font* Backend::CreateFontD2D(FG_Backend* self, const char* family, unsigned short weight, bool italic, unsigned int pt,
                                FG_Vec dpi, FG_AntiAliasing aa)
{
  auto instance = static_cast<Backend*>(self);
  size_t len    = UTF8toUTF16(family, -1, 0, 0);
  auto wtext    = (wchar_t*)ALLOCA(sizeof(wchar_t) * len);
  UTF8toUTF16(family, -1, wtext, len);

  IDWriteTextFormat* format = 0;
  wchar_t wlocale[LOCALE_NAME_MAX_LENGTH];
  GetSystemDefaultLocaleName(wlocale, LOCALE_NAME_MAX_LENGTH);
  LOGFAILURERET(instance->_writefactory->CreateTextFormat(wtext, 0, DWRITE_FONT_WEIGHT(weight),
                                                          italic ? DWRITE_FONT_STYLE_ITALIC : DWRITE_FONT_STYLE_NORMAL,
                                                          DWRITE_FONT_STRETCH_NORMAL, pt * (dpi.x / 72.0f), wlocale,
                                                          &format),
                0, "CreateTextFormat failed with error code %li", hr);

  if(!format)
    return 0;
  TCHAR name[64];
  UINT32 findex;
  BOOL exists;
  IDWriteFontCollection* collection;
  format->GetFontFamilyName(name, 64);
  format->GetFontCollection(&collection);
  collection->FindFamilyName(name, &findex, &exists);
  if(!exists) // CreateTextFormat always succeeds even for invalid font names so we have to check to see if we actually
              // loaded a real font
  {
    format->Release();
    collection->Release();
    return 0;
  }

  IDWriteFontFamily* ffamily;
  collection->GetFontFamily(findex, &ffamily);
  IDWriteFont* font;
  ffamily->GetFirstMatchingFont(format->GetFontWeight(), format->GetFontStretch(), format->GetFontStyle(), &font);

  DWRITE_FONT_METRICS metrics;
  font->GetMetrics(&metrics);
  float ratio       = format->GetFontSize() / static_cast<float>(metrics.designUnitsPerEm);
  FLOAT linespacing = (metrics.ascent + metrics.descent + metrics.lineGap) * ratio;
  FLOAT baseline    = metrics.ascent * ratio;
  ffamily->Release();
  font->Release();
  format->SetLineSpacing(DWRITE_LINE_SPACING_METHOD_DEFAULT, linespacing, baseline);
  collection->Release();

  return new FG_Font{ format, dpi, baseline, linespacing, pt, aa };
}

FG_Err Backend::DestroyFont(FG_Backend* self, FG_Font* font)
{
  if(!reinterpret_cast<IDWriteTextFormat*>(font->data.data)->Release())
    delete font;
  else
    return -1;
  return 0;
}
FG_Err Backend::DestroyLayout(FG_Backend* self, void* layout)
{
  if(!layout)
    return -1;
  reinterpret_cast<IDWriteTextLayout*>(layout)->Release();
  return 0;
}

void* Backend::FontLayout(FG_Backend* self, FG_Font* font, const char* text, FG_Rect* area, float lineHeight,
                          float letterSpacing, FG_BreakStyle breakStyle, void* prev)
{
  auto instance = static_cast<Backend*>(self);
  fgassert(font);
  IDWriteTextLayout* layout = reinterpret_cast<IDWriteTextLayout*>(prev);
  if(layout)
    layout->Release();
  if(!area)
    return 0;

  std::wstring utf;
  utf.resize(MultiByteToWideChar(CP_UTF8, 0, text, -1, 0, 0));
  utf.resize(MultiByteToWideChar(CP_UTF8, 0, text, -1, utf.data(), static_cast<int>(utf.capacity())));

  if(!text)
    return 0;
  float x = area->right - area->left;
  float y = area->bottom - area->top;
  LOGFAILURE(instance->_writefactory->CreateTextLayout(utf.c_str(), (UINT32)utf.size(),
                                                       reinterpret_cast<IDWriteTextFormat*>(font->data.data),
                                                       (x <= 0.0f ? INFINITY : x), (y <= 0.0f ? INFINITY : y), &layout),
             "CreateTextLayout failed with error code %li", hr);

  if(!layout)
  {
    area->right  = area->left;
    area->bottom = area->top;
    return 0;
  }
  FLOAT linespacing;
  FLOAT baseline;
  DWRITE_LINE_SPACING_METHOD method;
  layout->GetLineSpacing(&method, &linespacing, &baseline);
  if(lineHeight > 0.0f)
    layout->SetLineSpacing(DWRITE_LINE_SPACING_METHOD_UNIFORM, lineHeight, baseline * (lineHeight / linespacing));
  /*layout->SetWordWrapping((flags & (FGTEXT_CHARWRAP | FGTEXT_WORDWRAP)) ? DWRITE_WORD_WRAPPING_WRAP :
  DWRITE_WORD_WRAPPING_NO_WRAP); layout->SetReadingDirection((flags & FGTEXT_RTL) ? DWRITE_READING_DIRECTION_RIGHT_TO_LEFT :
  DWRITE_READING_DIRECTION_LEFT_TO_RIGHT); if(flags & FGTEXT_RIGHTALIGN)
    layout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_FAR);
  if(flags & FGTEXT_CENTER)
    layout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);*/
  if(letterSpacing > 0.0f)
  {
    DWRITE_TEXT_RANGE range;
    FLOAT leading, trailing, minimum;
    IDWriteTextLayout1* layout1 = 0;
    layout->QueryInterface<IDWriteTextLayout1>(&layout1);
    layout1->GetCharacterSpacing(0, &leading, &trailing, &minimum, &range);
    layout1->SetCharacterSpacing(leading, trailing + letterSpacing, minimum, range);
    layout1->Release();
  }

  DWRITE_TEXT_METRICS metrics;
  layout->GetMetrics(&metrics);
  if(area->right <= area->left)
    area->right = area->left + metrics.width;
  if(area->bottom <= area->top)
    area->bottom = area->top + metrics.height;
  layout->SetMaxWidth(area->right - area->left);
  layout->SetMaxHeight(area->bottom - area->top);

  switch(breakStyle)
  {
  case FG_BreakStyle_NONE: layout->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP); break;
  case FG_BreakStyle_WORD: layout->SetWordWrapping(DWRITE_WORD_WRAPPING_EMERGENCY_BREAK); break;
  case FG_BreakStyle_CHARACTER: layout->SetWordWrapping(DWRITE_WORD_WRAPPING_CHARACTER); break;
  }

  return layout;
}

uint32_t Backend::FontIndex(FG_Backend* self, FG_Font* font, void* fontlayout, FG_Rect* area, FG_Vec pos, FG_Vec* cursor)
{
  fgassert(font != 0);
  IDWriteTextLayout* layout = (IDWriteTextLayout*)fontlayout;
  if(!layout)
    return 0;

  BOOL trailing;
  BOOL inside;
  DWRITE_HIT_TEST_METRICS hit;
  layout->HitTestPoint(pos.x - area->left, pos.y - area->top, &trailing, &inside, &hit);

  cursor->x = hit.left;
  if(trailing)
    cursor->x += hit.width;
  cursor->y = hit.top;
  return hit.textPosition + trailing;
}

FG_Vec Backend::FontPos(FG_Backend* self, FG_Font* font, void* fontlayout, FG_Rect* area, uint32_t index)
{
  IDWriteTextLayout* layout = (IDWriteTextLayout*)fontlayout;
  if(!layout)
    return FG_Vec{ 0, 0 };

  FLOAT x, y;
  DWRITE_HIT_TEST_METRICS hit;
  layout->HitTestTextPosition(index, false, &x, &y, &hit);
  return FG_Vec{ x + area->left, y + area->top };
}

inline Asset* Backend::LoadAsset(const char* data, size_t count)
{
  IWICBitmapDecoder* decoder    = nullptr;
  IWICBitmapFrameDecode* source = nullptr;
  IWICFormatConverter* conv     = nullptr;
  IWICStream* stream            = nullptr;
  HRESULT hr                    = 0;

  if(!count && data)
  {
    fs::path p(data);
    hr = _wicfactory->CreateDecoderFromFilename(p.wstring().c_str(), nullptr, GENERIC_READ, WICDecodeMetadataCacheOnDemand,
                                                &decoder);
  }
  else
  {
    hr = _wicfactory->CreateStream(&stream);
    if(SUCCEEDED(hr))
      stream->InitializeFromMemory((BYTE*)data, (DWORD)count);
    if(SUCCEEDED(hr)) // WICDecodeMetadataCacheOnDemand
      hr = _wicfactory->CreateDecoderFromStream(stream, NULL, WICDecodeMetadataCacheOnLoad, &decoder);
  }
  if(SUCCEEDED(hr))
    hr = decoder->GetFrame(0, &source);
  if(SUCCEEDED(hr))
    hr = _wicfactory->CreateFormatConverter(&conv);
  if(SUCCEEDED(hr)) // Convert the image format to 32bppPBGRA (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
    hr = conv->Initialize(source, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, nullptr, 0.0f,
                          WICBitmapPaletteTypeCustom);
  if(FAILED(hr))
  {
    (*_log)(_root, FG_Level_ERROR, "fgCreateAssetD2D failed with error code %li", hr);
    return 0;
  }

  Asset* asset     = new Asset();
  asset->data.data = conv;
  asset->format    = FG_Format_UNKNOWN;

  GUID format;
  decoder->GetContainerFormat(&format);

  if(format == GUID_ContainerFormatBmp)
    asset->format = FG_Format_BMP;
  else if(format == GUID_ContainerFormatPng)
    asset->format = FG_Format_PNG;
  else if(format == GUID_ContainerFormatIco)
    asset->format = FG_Format_ICO;
  else if(format == GUID_ContainerFormatJpeg)
    asset->format = FG_Format_JPG;
  else if(format == GUID_ContainerFormatTiff)
    asset->format = FG_Format_TIFF;
  else if(format == GUID_ContainerFormatGif)
    asset->format = FG_Format_GIF;
  else if(format == GUID_ContainerFormatWebp)
    asset->format = FG_Format_WEBP;

  if(stream)
    stream->Release();
  if(decoder)
    decoder->Release();
  if(source)
    source->Release();

  D2D1_SIZE_U sz = { 0 };
  double dpix, dpiy;

  conv->GetSize(&sz.width, &sz.height);
  conv->GetResolution(&dpix, &dpiy);

  asset->dpi  = { static_cast<float>(dpix), static_cast<float>(dpiy) };
  asset->size = { static_cast<int>(sz.width), static_cast<int>(sz.height) };
  return asset;

  // if(data[0] == 0xFF && data[1] == 0xD8) // JPEG SOI header
  // else if(data[0] == 'B' && data[1] == 'M') // BMP header
  // else if(data[0] == 137 && data[1] == 80 && data[2] == 78 && data[3] == 71) // PNG file signature
  // else if(data[0] == 'G' && data[1] == 'I' && data[2] == 'F') // GIF header
  // else if((data[0] == 'I' && data[1] == 'I' && data[2] == '*' && data[3] == 0) || (data[0] == 'M' && data[1] == 'M' &&
  // data[2] == 0 && data[3] == '*')) // TIFF header
}

FG_Asset* Backend::CreateAsset(FG_Backend* self, const char* data, uint32_t count, FG_Format format, int flags)
{
  return static_cast<Backend*>(self)->LoadAsset(data, count);
}
FG_Asset* Backend::CreateBuffer(FG_Backend* self, void* data, uint32_t bytes, uint8_t primitive,
                                FG_ShaderParameter* parameters, uint32_t n_parameters)
{
  return 0;
}

FG_Asset* Backend::CreateLayer(FG_Backend* self, FG_Window* window, FG_Vec* size, int flags)
{
  if(!self || !window)
    return nullptr;

  auto context = static_cast<Window*>(window);
  ID2D1Layer* layer;
  if(FAILED(context->target->CreateLayer(!size ? context->target->GetSize() : D2D1::SizeF(size->x, size->y), &layer)))
    return nullptr;

  auto psize   = context->target->GetPixelSize();
  auto p       = new FG_Asset();
  p->data.data = layer;
  p->size      = FG_Veci{ (int)psize.width, (int)psize.height };
  p->format    = FG_Format_WEAK_LAYER;
  context->target->GetDpi(&p->dpi.x, &p->dpi.y);
  return p;
}

FG_Err Backend::DestroyAsset(FG_Backend* self, FG_Asset* fgasset)
{
  if(fgasset->format == FG_Format_WEAK_LAYER || fgasset->format == FG_Format_LAYER)
  {
    FG_Err e = reinterpret_cast<ID2D1Layer*>(fgasset->data.data)->Release();
    delete fgasset;
    return e;
  }

  auto asset = static_cast<Asset*>(fgasset);
  for(auto& i : asset->instances)
    i->DiscardAsset(asset);

  FG_Err e = reinterpret_cast<IUnknown*>(asset->data.data)->Release();
  delete asset;
  return e;
}

FG_Shader* Backend::CreateShader(FG_Backend* self, const char* ps, const char* vs, const char* gs, const char* cs,
                                 const char* ds, const char* hs, FG_ShaderParameter* parameters, uint32_t n_parameters)
{
  return 0;
}
FG_Err Backend::DestroyShader(FG_Backend* self, FG_Shader* shader) { return -1; }
FG_Err Backend::GetProjection(FG_Backend* self, FG_Window* window, FG_Asset* layer, float* proj4x4) { return -1; }
FG_Err Backend::PutClipboard(FG_Backend* self, FG_Window* window, FG_Clipboard kind, const char* data, uint32_t count)
{
  if(!OpenClipboard(reinterpret_cast<HWND>(window->handle)))
    return -1;
  if(data != 0 && count > 0 && EmptyClipboard())
  {
    if(kind == FG_Clipboard_TEXT)
    {
      size_t unilen  = UTF8toUTF16(data, count, 0, 0);
      HGLOBAL unimem = GlobalAlloc(GMEM_MOVEABLE, unilen * sizeof(wchar_t));
      if(unimem)
      {
        wchar_t* uni = reinterpret_cast<wchar_t*>(GlobalLock(unimem));
        size_t sz    = UTF8toUTF16(data, count, uni, unilen);
        if(sz < unilen) // ensure we have a null terminator
          uni[sz] = 0;
        GlobalUnlock(unimem);
        SetClipboardData(CF_UNICODETEXT, unimem);
      }
      HGLOBAL gmem = GlobalAlloc(GMEM_MOVEABLE, count + 1);
      if(gmem)
      {
        char* mem = reinterpret_cast<char*>(GlobalLock(gmem));
        MEMCPY(mem, count + 1, data, count);
        mem[count] = 0;
        GlobalUnlock(gmem);
        SetClipboardData(CF_TEXT, gmem);
      }
    }
    else
    {
      HGLOBAL gmem = GlobalAlloc(GMEM_MOVEABLE, count);
      if(gmem)
      {
        void* mem = GlobalLock(gmem);
        MEMCPY(mem, count, data, count);
        GlobalUnlock(gmem);
        UINT format = CF_PRIVATEFIRST;
        switch(kind)
        {
        case FG_Clipboard_WAVE: format = CF_WAVE; break;
        case FG_Clipboard_BITMAP: format = CF_BITMAP; break;
        }
        SetClipboardData(format, gmem);
      }
    }
  }
  CloseClipboard();
  return 0;
}

uint32_t Backend::GetClipboard(FG_Backend* self, FG_Window* window, FG_Clipboard kind, void* target, uint32_t count)
{
  if(!OpenClipboard(reinterpret_cast<HWND>(window->handle)))
    return 0;
  UINT format = CF_PRIVATEFIRST;
  switch(kind)
  {
  case FG_Clipboard_TEXT:
    if(IsClipboardFormatAvailable(CF_UNICODETEXT))
    {
      HANDLE gdata = GetClipboardData(CF_UNICODETEXT);
      SIZE_T len   = 0;
      if(gdata)
      {
        SIZE_T size        = GlobalSize(gdata) / 2;
        const wchar_t* str = (const wchar_t*)GlobalLock(gdata);
        if(str)
        {
          len = UTF16toUTF8(str, size, 0, 0);

          if(target && count >= len)
            len = UTF16toUTF8(str, size, (char*)target, count);

          GlobalUnlock(gdata);
        }
      }
      CloseClipboard();
      return (uint32_t)len;
    }
    format = CF_TEXT;
    break;
  case FG_Clipboard_WAVE: format = CF_WAVE; break;
  case FG_Clipboard_BITMAP: format = CF_BITMAP; break;
  }
  HANDLE gdata = GetClipboardData(format);
  SIZE_T size  = 0;
  if(gdata)
  {
    size = GlobalSize(gdata);
    if(target && count >= size)
    {
      void* data = GlobalLock(gdata);
      if(data)
      {
        MEMCPY(target, count, data, size);
        GlobalUnlock(gdata);
      }
      else
        size = 0;
    }
  }
  CloseClipboard();
  return (uint32_t)size;
}

bool Backend::CheckClipboard(FG_Backend* self, FG_Window* window, FG_Clipboard kind)
{
  switch(kind)
  {
  case FG_Clipboard_TEXT: return IsClipboardFormatAvailable(CF_TEXT) | IsClipboardFormatAvailable(CF_UNICODETEXT);
  case FG_Clipboard_WAVE: return IsClipboardFormatAvailable(CF_WAVE);
  case FG_Clipboard_BITMAP: return IsClipboardFormatAvailable(CF_BITMAP);
  case FG_Clipboard_CUSTOM: return IsClipboardFormatAvailable(CF_PRIVATEFIRST);
  case FG_Clipboard_ALL:
    return IsClipboardFormatAvailable(CF_TEXT) | IsClipboardFormatAvailable(CF_UNICODETEXT) |
           IsClipboardFormatAvailable(CF_WAVE) | IsClipboardFormatAvailable(CF_BITMAP) |
           IsClipboardFormatAvailable(CF_PRIVATEFIRST);
  }
  return false;
}

FG_Err Backend::ClearClipboard(FG_Backend* self, FG_Window* window, FG_Clipboard kind)
{
  if(!OpenClipboard(reinterpret_cast<HWND>(window->handle)))
    return -1;
  if(!EmptyClipboard())
    return -1;
  CloseClipboard();
  return 0;
}

FG_Err Backend::ProcessMessages(FG_Backend* self)
{
  MSG msg;
  while(PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
  {
    LRESULT r = DispatchMessageW(&msg);

    switch(msg.message)
    {
    case WM_SYSKEYUP:
    case WM_SYSKEYDOWN:
    case WM_KEYUP:
    case WM_KEYDOWN:
      if(!r) // if the return value is zero, we already processed the keydown message successfully, so DON'T turn it into a
             // character.
        break;
    default: TranslateMessage(&msg); break;
    case WM_QUIT: return 0;
    }
  }

  return 1;
}

FG_Err Backend::SetCursorD2D(FG_Backend* self, FG_Window* window, FG_Cursor cursor)
{
  static HCURSOR hArrow    = LoadCursor(NULL, IDC_ARROW);
  static HCURSOR hIBeam    = LoadCursor(NULL, IDC_IBEAM);
  static HCURSOR hCross    = LoadCursor(NULL, IDC_CROSS);
  static HCURSOR hWait     = LoadCursor(NULL, IDC_WAIT);
  static HCURSOR hHand     = LoadCursor(NULL, IDC_HAND);
  static HCURSOR hSizeNS   = LoadCursor(NULL, IDC_SIZENS);
  static HCURSOR hSizeWE   = LoadCursor(NULL, IDC_SIZEWE);
  static HCURSOR hSizeNWSE = LoadCursor(NULL, IDC_SIZENWSE);
  static HCURSOR hSizeNESW = LoadCursor(NULL, IDC_SIZENESW);
  static HCURSOR hSizeAll  = LoadCursor(NULL, IDC_SIZEALL);
  static HCURSOR hNo       = LoadCursor(NULL, IDC_NO);
  static HCURSOR hHelp     = LoadCursor(NULL, IDC_HELP);
  static HCURSOR hDrag     = hSizeAll;

  switch(cursor)
  {
  case FG_Cursor_ARROW: SetCursor(hArrow); break;
  case FG_Cursor_IBEAM: SetCursor(hIBeam); break;
  case FG_Cursor_CROSS: SetCursor(hCross); break;
  case FG_Cursor_WAIT: SetCursor(hWait); break;
  case FG_Cursor_HAND: SetCursor(hHand); break;
  case FG_Cursor_RESIZENS: SetCursor(hSizeNS); break;
  case FG_Cursor_RESIZEWE: SetCursor(hSizeWE); break;
  case FG_Cursor_RESIZENWSE: SetCursor(hSizeNWSE); break;
  case FG_Cursor_RESIZENESW: SetCursor(hSizeNESW); break;
  case FG_Cursor_RESIZEALL: SetCursor(hSizeAll); break;
  case FG_Cursor_NO: SetCursor(hNo); break;
  case FG_Cursor_HELP: SetCursor(hHelp); break;
  case FG_Cursor_DRAG: SetCursor(hDrag); break;
  default: return -1;
  }

  return 0;
}

FG_Err Backend::GetDisplayIndex(FG_Backend* self, unsigned int index, FG_Display* out)
{
  auto instance = static_cast<Backend*>(self);
  if(index >= instance->_displays.Length())
    return -1;
  *out = instance->_displays[index];
  return 0;
}

FG_Err Backend::GetDisplay(FG_Backend* self, void* handle, FG_Display* out)
{
  for(auto& i : static_cast<Backend*>(self)->_displays)
    if(i.handle == handle)
    {
      *out = i;
      return 0;
    }

  return -1;
}

FG_Err Backend::GetDisplayWindow(FG_Backend* self, FG_Window* window, FG_Display* out)
{
  HMONITOR monitor = MonitorFromWindow(reinterpret_cast<HWND>(window->handle), MONITOR_DEFAULTTONEAREST);
  for(auto& i : static_cast<Backend*>(self)->_displays)
    if(i.handle == monitor)
    {
      *out = i;
      return 0;
    }

  return -1;
}

FG_Window* Backend::CreateWindowD2D(FG_Backend* self, FG_MsgReceiver* element, void* display, FG_Vec* pos, FG_Vec* dim,
                               const char* caption, uint64_t flags)
{
  // TODO: If a display other than the primary monitor is specified AND pos == NULL, then we should recreate Windows' DWM
  // new window logic by incrementing _lastwindowpos on both the x and y axis by the height of a standard window titlebar
  // and shifting it into that monitor's rectangle.
  auto window = new Window(static_cast<Backend*>(self), element, pos, dim, flags, caption);
  return window;
}

FG_Err Backend::SetWindowD2D(FG_Backend* self, FG_Window* window, FG_MsgReceiver* element, void* display, FG_Vec* pos,
                             FG_Vec* dim, const char* caption, uint64_t flags)
{
  Window* ptr = static_cast<Window*>(window);
  if(!ptr)
    return -1;
  ptr->element = element;
  if(pos || dim)
    ptr->SetArea(pos, dim);
  if(caption)
    ptr->SetCaption(caption);
  if(flags != ~0ULL)
    ptr->SetFlags(flags);
  return 0;
}

FG_Err Backend::DestroyWindow(FG_Backend* self, FG_Window* window)
{
  Window* ptr = static_cast<Window*>(window);
  if(!ptr)
    return -1;
  delete ptr;
  return 0;
}
FG_Err Backend::BeginDraw(FG_Backend* self, FG_Window* window, FG_Rect* area)
{
  Window* ptr = static_cast<Window*>(window);
  if(!ptr)
    return -1;
  ptr->BeginDraw(*area);
  return 0;
}
FG_Err Backend::EndDraw(FG_Backend* self, FG_Window* window)
{
  Window* ptr = static_cast<Window*>(window);
  if(!ptr)
    return -1;
  ptr->EndDraw();
  return 0;
}

void DestroyD2D(FG_Backend* self)
{
  PostQuitMessage(0);
  auto d2d = static_cast<Backend*>(self);
  if(!d2d)
    return;

  d2d->~Backend();
  CoUninitialize();
  free(d2d);
}

long Backend::CreateHWNDTarget(const D2D1_RENDER_TARGET_PROPERTIES& rtprop, const D2D1_HWND_RENDER_TARGET_PROPERTIES& hprop,
                               ID2D1HwndRenderTarget** target)
{
  return _factory->CreateHwndRenderTarget(rtprop, hprop, target);
}

Backend::~Backend()
{
  if(_factory)
    _factory->Release();
  if(_wicfactory)
    _wicfactory->Release();
  if(_writefactory)
    _writefactory->Release();
}

inline FG_Vec operator-(const FG_Vec& l, const FG_Vec& r) { return FG_Vec{ l.x - r.x, l.y - r.y }; }

BOOL __stdcall Backend::EnumerateMonitorsProc(HMONITOR monitor, HDC hdc, LPRECT, LPARAM lparam)
{
  MONITORINFO info = { sizeof(MONITORINFO), 0 };
  if(GetMonitorInfoW(monitor, &info) != 0)
  {
    Backend* instance = reinterpret_cast<Backend*>(lparam);
    instance->_displays.Add(
      FG_Display{ { info.rcMonitor.right - info.rcMonitor.left, info.rcMonitor.bottom - info.rcMonitor.top },
                  { info.rcMonitor.left, info.rcMonitor.top },
                  instance->dpi,
                  1.0f,
                  monitor,
                  (info.dwFlags & MONITORINFOF_PRIMARY) != 0 });

    if(instance->getDpiForMonitor)
    {
      UINT x, y;
      if((*instance->getDpiForMonitor)(monitor, 0, &x, &y) == S_OK)
        instance->_displays.Back().dpi = { static_cast<float>(x), static_cast<float>(y) };
    }

    if(instance->getScaleFactorForMonitor)
    {
      static_assert(sizeof(DEVICE_SCALE_FACTOR) == sizeof(int));
      DEVICE_SCALE_FACTOR factor;
      (*instance->getScaleFactorForMonitor)(monitor, (int*)&factor);
      float scale = 1.0f;
      switch(factor)
      {
      case DEVICE_SCALE_FACTOR_INVALID:
      case SCALE_100_PERCENT: scale = 1.0f; break;
      case SCALE_120_PERCENT: scale = 1.2f; break;
      case SCALE_125_PERCENT: scale = 1.25f; break;
      case SCALE_140_PERCENT: scale = 1.4f; break;
      case SCALE_150_PERCENT: scale = 1.5f; break;
      case SCALE_160_PERCENT: scale = 1.6f; break;
      case SCALE_175_PERCENT: scale = 1.75f; break;
      case SCALE_180_PERCENT: scale = 1.8f; break;
      case SCALE_200_PERCENT: scale = 2.0f; break;
      case SCALE_225_PERCENT: scale = 2.25f; break;
      case SCALE_250_PERCENT: scale = 2.5f; break;
      case SCALE_300_PERCENT: scale = 3.0f; break;
      case SCALE_350_PERCENT: scale = 3.5f; break;
      case SCALE_400_PERCENT: scale = 4.0f; break;
      case SCALE_450_PERCENT: scale = 4.5f; break;
      case SCALE_500_PERCENT: scale = 5.0f; break;
      }

      instance->_displays.Back().scale = scale;
    }
  }

  return TRUE;
}

void Backend::RefreshMonitors()
{
  _displays.Clear();
  EnumDisplayMonitors(0, 0, EnumerateMonitorsProc, (LPARAM)this);
}

FG_Result Backend::Behavior(Window* w, const FG_Msg& msg)
{
  return (*_behavior)(w->element, w, _root, const_cast<FG_Msg*>(&msg));
}

uint16_t Backend::GetTouchIndex(DWORD index, bool up)
{
  uint16_t n = _touchid.Length();

  for(uint16_t i = 0; i < _touchid.Length(); ++i)
  {
    if(_touchid[i] == index)
    {
      n = i;
      break;
    }
    if(_touchid[i] == ~0ULL && i < n)
      n = i;
  }

  if(n < _touchid.Length())
  {
    if(!up)
      _touchid[n] = index;
    else
      _touchid[n] = ~0ULL;
  }
  else if(!up)
    _touchid.Add(index);

  return n;
}

int64_t GetRegistryValueW(HKEY__* hKeyRoot, const wchar_t* szKey, const wchar_t* szValue, unsigned char* data,
                          unsigned long sz)
{
  HKEY__* hKey;
  LRESULT e = RegOpenKeyExW(hKeyRoot, szKey, 0, KEY_READ, &hKey);
  if(!hKey)
    return -2;
  LSTATUS r = RegQueryValueExW(hKey, szValue, 0, 0, data, &sz);
  RegCloseKey(hKey);
  if(r == ERROR_SUCCESS)
    return sz;
  return (r == ERROR_MORE_DATA) ? sz : -1;
}

extern "C" FG_COMPILER_DLLEXPORT FG_Backend* fgDirect2D(void* root, FG_Log log, FG_Behavior behavior)
{
  static_assert(std::is_same<FG_InitBackend, decltype(&(fgDirect2D))>::value,
                "fgDirect2D must match InitBackend function pointer");
  typedef BOOL(WINAPI * tGetPolicy)(LPDWORD lpFlags);
  typedef BOOL(WINAPI * tSetPolicy)(DWORD dwFlags);
  const DWORD EXCEPTION_SWALLOWING = 0x1;
  DWORD dwFlags;

  HMODULE kernel32 = LoadLibraryA("kernel32.dll");
  fgassert(kernel32 != 0);
  tGetPolicy pGetPolicy = (tGetPolicy)GetProcAddress(kernel32, "GetProcessUserModeExceptionPolicy");
  tSetPolicy pSetPolicy = (tSetPolicy)GetProcAddress(kernel32, "SetProcessUserModeExceptionPolicy");
  if(pGetPolicy && pSetPolicy && pGetPolicy(&dwFlags))
    pSetPolicy(dwFlags & ~EXCEPTION_SWALLOWING); // Turn off the filter

  HRESULT hr = CoInitialize(NULL); // If this fails for some reason we can't even log an error
  if(FAILED(hr))
    return 0;

  if(FAILED(hr = CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_PKT_PRIVACY, RPC_C_IMP_LEVEL_IMPERSONATE,
                                      NULL, EOAC_DYNAMIC_CLOAKING, NULL)))
    return 0;

  IGlobalOptions* pGlobalOptions;
  hr = CoCreateInstance(CLSID_GlobalOptions, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pGlobalOptions));
  if(SUCCEEDED(hr))
  {
    hr = pGlobalOptions->Set(COMGLB_EXCEPTION_HANDLING, COMGLB_EXCEPTION_DONOT_HANDLE);
    pGlobalOptions->Release();
  }

  ID2D1Factory1* factory         = 0;
  IWICImagingFactory* wicfactory = 0;
  IDWriteFactory1* writefactory  = 0;
  hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (LPVOID*)&wicfactory);
  if(SUCCEEDED(hr))
    hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory1),
                             reinterpret_cast<IUnknown**>(&writefactory));
  else
    (*log)(root, FG_Level_ERROR, "CoCreateInstance() failed with error: %li", hr);

  D2D1_FACTORY_OPTIONS d2dopt = { D2D1_DEBUG_LEVEL_NONE };
#ifdef FG_DEBUG
  d2dopt.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif

  if(SUCCEEDED(hr))
    hr = D2D1CreateFactory<ID2D1Factory1>(D2D1_FACTORY_TYPE_SINGLE_THREADED, d2dopt, &factory);
  else
    (*log)(root, FG_Level_ERROR, "DWriteCreateFactory() failed with error: %li", hr);

  if(FAILED(hr))
  {
    (*log)(root, FG_Level_ERROR, "D2D1CreateFactory() failed with error: %li", hr);
    return 0;
  }

  return new Backend(root, log, behavior, factory, wicfactory, writefactory);
}

Backend::Backend(void* root, FG_Log log, FG_Behavior behavior, ID2D1Factory1* factory, IWICImagingFactory* wicfactory,
                 IDWriteFactory1* writefactory) :
  _root(root), _log(log), _behavior(behavior), _factory(factory), _wicfactory(wicfactory), _writefactory(writefactory)
{
  draw                 = &DrawD2D;
  clear                = &Clear;
  pushLayer            = &PushLayer;
  popLayer             = &PopLayer;
  pushClip             = &PushClip;
  popClip              = &PopClip;
  dirtyRect            = &DirtyRect;
  beginDraw            = &BeginDraw;
  endDraw              = &EndDraw;
  createShader         = &CreateShader;
  destroyShader        = &DestroyShader;
  createFont           = &CreateFontD2D;
  destroyFont          = &DestroyFont;
  fontLayout           = &FontLayout;
  destroyLayout        = &DestroyLayout;
  fontIndex            = &FontIndex;
  fontPos              = &FontPos;
  createAsset          = &CreateAsset;
  createBuffer         = &CreateBuffer;
  createLayer          = &CreateLayer;
  destroyAsset         = &DestroyAsset;
  getProjection        = &GetProjection;
  putClipboard         = &PutClipboard;
  getClipboard         = &GetClipboard;
  checkClipboard       = &CheckClipboard;
  clearClipboard       = &ClearClipboard;
  processMessages      = &ProcessMessages;
  setCursor            = &SetCursorD2D;
  getDisplayIndex      = &GetDisplayIndex;
  getDisplay           = &GetDisplay;
  getDisplayWindow     = &GetDisplayWindow;
  createWindow         = &CreateWindowD2D;
  setWindow            = &SetWindowD2D;
  destroyWindow        = &DestroyWindow;
  destroy              = &DestroyD2D;
  createSystemControl  = &CreateSystemControl;
  setSystemControl     = &SetSystemControl;
  destroySystemControl = &DestroySystemControl;

  HDC hdc = GetDC(NULL);
  dpi     = { static_cast<float>(GetDeviceCaps(hdc, LOGPIXELSX)), static_cast<float>(GetDeviceCaps(hdc, LOGPIXELSY)) };
  ReleaseDC(NULL, hdc);

  (*_log)(_root, FG_Level_NONE, "Initializing fgDirect2D...");

#ifdef FG_DEBUG
  HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);
#endif

  HRESULT hr;
  if(FAILED(hr = RoundRect::Register(factory)))
    (*_log)(_root, FG_Level_ERROR, "Failed to register RoundRect", hr);
  if(FAILED(hr = Circle::Register(factory)))
    (*_log)(_root, FG_Level_ERROR, "Failed to register Circle", hr);
  if(FAILED(hr = Arc::Register(factory)))
    (*_log)(_root, FG_Level_ERROR, "Failed to register Arc", hr);
  if(FAILED(hr = Triangle::Register(factory)))
    (*_log)(_root, FG_Level_ERROR, "Failed to register Triangle", hr);
  if(FAILED(hr = Modulation::Register(factory)))
    (*_log)(_root, FG_Level_ERROR, "Failed to register Modulation", hr);

  // Check for desktop composition
  HMODULE dwm = LoadLibraryW(L"dwmapi.dll");
  if(dwm)
  {
    DWMCOMPENABLE dwmcomp = (DWMCOMPENABLE)GetProcAddress(dwm, "DwmIsCompositionEnabled");
    if(!dwmcomp)
    {
      FreeLibrary(dwm);
      dwm = 0;
    }
    else
    {
      BOOL res;
      (*dwmcomp)(&res);
      if(res == FALSE)
      {
        FreeLibrary(dwm);
        dwm = 0;
      } // fail
    }
    dwmblurbehind = (DWMBLURBEHIND)GetProcAddress(dwm, "DwmEnableBlurBehindWindow");

    if(!dwmblurbehind)
    {
      FreeLibrary(dwm);
      dwm = 0;
    }
  }

  Window::WndRegister(Window::WndProc, WindowClass);
  getDpiForMonitor = 0;
  getScaleFactorForMonitor = 0;

  // factory->GetDesktopDpi(&dpi.x, &dpi.y);

  /*if(shcoreD2D)
  {
    getDpiForMonitor = (GETDPIFORMONITOR)GetProcAddress(shcoreD2D.get(), "GetDpiForMonitor");
    getScaleFactorForMonitor = (GETSCALEFACTORFORMONITOR)GetProcAddress(shcoreD2D.get(), "GetScaleFactorForMonitor");
  }
  RefreshMonitors();*/

  int64_t sz = GetRegistryValueW(HKEY_CURRENT_USER, L"Control Panel\\Desktop", L"CursorBlinkRate", 0, 0);
  if(sz > 0)
  {
    std::wstring buf;
    buf.resize(sz / 2);
    sz = GetRegistryValueW(HKEY_CURRENT_USER, L"Control Panel\\Desktop", L"CursorBlinkRate",
                           reinterpret_cast<unsigned char*>(buf.data()), (unsigned long)sz);
    if(sz > 0)
      cursorblink = _wtoi(buf.data());
  }
  else
    (*_log)(_root, FG_Level_WARNING, "Couldn't get user's cursor blink rate.");

  sz = GetRegistryValueW(HKEY_CURRENT_USER, L"Control Panel\\Mouse", L"MouseHoverTime", 0, 0);
  if(sz > 0)
  {
    std::wstring buf;
    buf.resize(sz / 2);
    sz = GetRegistryValueW(HKEY_CURRENT_USER, L"Control Panel\\Mouse", L"MouseHoverTime",
                           reinterpret_cast<unsigned char*>(buf.data()), (unsigned long)sz);
    if(sz > 0)
      tooltipdelay = _wtoi(buf.data());
  }
  else
    (*_log)(_root, FG_Level_WARNING, "Couldn't get user's mouse hover time.");
}