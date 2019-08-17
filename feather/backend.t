local C = terralib.includecstring [[
  #include <string.h>
]]

local Flags = require 'feather.flags'
local Enum = require 'feather.enum'
local Color = require 'feather.color'
require 'feather.os'

local BACKEND_FEATURES = Flags{
  "TEXT_ANTIALIAS", 
  "TEXT_SUBPIXEL",
  "TEXT_BLUR",
  "TEXT_ALPHA",
  "RECT_CORNERS",
  "RECT_BORDER",
  "RECT_BLUR",
  "RECT_ALPHA",
  "CIRCLE_ARCS",
  "CIRCLE_BORDER",
  "CIRCLE_BLUR",
  "CIRCLE_ALPHA",
  "TRIANGLE_CORNERS",
  "TRIANGLE_BORDER",
  "TRIANGLE_BLUR",
  "TRIANGLE_ALPHA",
  "LINES_ALPHA",
  "CURVE_STROKE", -- If both stroke and fill are false, it doesn't support curves at all
  "CURVE_FILL",
  "LAYER_TRANSFORM",
  "LAYER_OPACITY",
  "SHADER_GLSL2",
  "SHADER_GLSL4",
  "SHADER_HLSL2",
  "BACKGROUND_OPACITY",
  "IMMEDIATE_MODE"}

local BACKEND_FORMATS = Enum{
  "GRADIENT",
  "BMP",
  "JPG",
  "PNG",
  "ICO",
  "GIF",
  "TIFF",
  "TGA",
  "WEBP",
  "DDS",
  "WIC",
  "SVG",
  "AVI",
  "MP4",
  "MKV",
  "WEBM"}

BACKEND_FORMATS.UNKNOWN = `0xff

local TEXT_ANTIALIASING = Enum{
  "NO_AA",
  "AA",
  "LCD",
  "LCD_V"}

local CLIPBOARD_FORMATS = Enum{
  "NONE",
  "TEXT",
  "WAVE",
  "BITMAP",
  "FILE",
  "ELEMENT",
  "CUSTOM",
  "ALL"}

local FG_CURSOR = Enum{
  "FGCURSOR_NONE",
  "FGCURSOR_ARROW",
  "FGCURSOR_IBEAM",
  "FGCURSOR_CROSS",
  "FGCURSOR_WAIT",
  "FGCURSOR_HAND",
  "FGCURSOR_RESIZENS",
  "FGCURSOR_RESIZEWE",
  "FGCURSOR_RESIZENWSE",
  "FGCURSOR_RESIZENESW",
  "FGCURSOR_RESIZEALL",
  "FGCURSOR_NO",
  "FGCURSOR_HELP",
  "FGCURSOR_DRAG",
  "FGCURSOR_CUSTOM"}

struct fgData {
  data : &opaque
}

struct fgFont {
  data : fgData
  dpi : fgVec
  baseline : float
  lineheight : float
  pt : uint
}

struct fgAsset {
  data : fgData
  size : fgVeci
  dpi : fgVec
  format : BACKEND_FORMATS
}

struct DynamicBackend {
  drawFont : {&DynamicBackend, &opaque, &fgFont, &opaque, &fgRect, fgColor, float, float, float, TEXT_ANTIALIASING} -> fgError
  drawAsset : {&DynamicBackend, &opaque, &fgAsset, &fgRect, &fgRect, fgColor, float} -> fgError
  drawRect : {&DynamicBackend, &opaque, &fgRect, &fgRect, fgColor, float, fgColor, float, &fgAsset} -> fgError
  drawCircle : {&DynamicBackend, &opaque, &fgRect, &fgRect, fgColor, float, fgColor, float, &fgAsset} -> fgError
  drawTriangle : {&DynamicBackend, &opaque, &fgRect, &fgRect, fgColor, float, fgColor, float, &fgAsset} -> fgError
  drawLines : {&DynamicBackend, &opaque, &fgVec, uint, fgColor} -> fgError
  drawCurve : {&DynamicBackend, &opaque, &fgVec, uint, fgColor, float, fgColor} -> fgError
  --drawShader : {&DynamicBackend, &opaque} -> fgError
  pushLayer : {&DynamicBackend, &opaque, fgRect, &float, float} -> fgError
  popLayer : {&DynamicBackend, &opaque} -> fgError
  pushClip : {&DynamicBackend, &opaque, fgRect} -> fgError
  popClip : {&DynamicBackend, &opaque} -> fgError
  dirtyRect : {&DynamicBackend, &opaque, fgRect} -> fgError

  createFont : {&DynamicBackend, rawstring, uint16, bool, uint32, fgVec} -> &fgFont
  destroyFont : {&DynamicBackend, &fgFont} -> fgError
  fontLayout : {&DynamicBackend, &fgFont, rawstring, &fgRect, float, float, &opaque, fgVec} -> &opaque
  fontIndex : {&DynamicBackend, &fgFont, &opaque, &fgRect, float, float, fgVec, &fgVec, fgVec} -> uint
  fontPos : {&DynamicBackend, &fgFont, &opaque, &fgRect, float, float, uint, fgVec} -> fgVec

  createAsset : {&DynamicBackend, rawstring, uint, BACKEND_FORMATS} -> &fgAsset
  destroyAsset : {&DynamicBackend, &fgAsset} -> fgError

  putClipboard : {&DynamicBackend, CLIPBOARD_FORMATS, rawstring, uint} -> fgError
  getClipboard : {&DynamicBackend, CLIPBOARD_FORMATS, &opaque, uint} -> uint
  checkClipboard : {&DynamicBackend, CLIPBOARD_FORMATS} -> bool
  clearClipboard : {&DynamicBackend, CLIPBOARD_FORMATS} -> fgError

  processMessages : {&opaque, &DynamicBackend} -> fgError
  setCursor : {&DynamicBackend, &opaque, FG_CURSOR} -> fgError
  requestAnimationFrame : {&DynamicBackend, &opaque, uint64} -> fgError
  destroy : {&DynamicBackend} -> {}
  
  features : BACKEND_FEATURES
  formats : fgFlag
  dpi : fgVec
  scale : float
  displays : &opaque
  n_displays : uint32
  cursorblink : uint64
  tooltipdelay : uint64
}

-- It's not clear if we should be generating member functions from the function pointers or vice-versa
--for _, entry in ipairs(DynamicBackend:getentries()) do
--  if entry.type:ispointertofunction() then 
--    DynamicBackend.methods[string.upper(entry.field:sub(1,1)) .. entry.field:sub(2, -1)]
--  end
--end

terra DynamicBackend:DrawFont(data : &opaque, font : &fgFont, layout : &opaque, area : &fgRect, color : fgColor, lineHeight : float, letterSpacing : float, blur : float, aa : TEXT_ANTIALIASING) : fgError
  return self.drawFont(self, data, font, layout, area, color, lineHeight, letterSpacing, blur, aa)
end
terra DynamicBackend:DrawAsset(data : &opaque, asset : &fgAsset, area : &fgRect, source : &fgRect, color : fgColor, time : float) : fgError
  return self.drawAsset(self, data, asset, area, source, color, time)
end
terra DynamicBackend:DrawRect(data : &opaque, area : &fgRect, corners : &fgRect, fillColor : fgColor, border : float, borderColor : fgColor, blur : float, asset : &fgAsset) : fgError
  return self.drawRect(self, data, area, corners, fillColor, border, borderColor, blur, asset)
end
terra DynamicBackend:DrawCircle(data : &opaque, area : &fgRect, arcs : &fgRect, fillColor : fgColor, border : float, borderColor : fgColor, blur : float, asset : &fgAsset) : fgError
  return self.drawCircle(self, data, area, arcs, fillColor, border, borderColor, blur, asset)
end
terra DynamicBackend:DrawTriangle(data : &opaque, area : &fgRect, corners : &fgRect, fillColor : fgColor, border : float, borderColor : fgColor, blur : float, asset : &fgAsset) : fgError
  return self.drawTriangle(self, data, area, corners, fillColor, border, borderColor, blur, asset)
end
terra DynamicBackend:DrawLines(data : &opaque, points : &fgVec, count : uint, color : fgColor) : fgError
  return self.drawLines(self, data, points, count, color)
end
terra DynamicBackend:DrawCurve(data : &opaque, anchors : &fgVec, count : uint, fillColor : fgColor, stroke : float, strokeColor : fgColor) : fgError
  return self.drawCurve(self, data, anchors, count, fillColor, stroke, strokeColor)
end
terra DynamicBackend:PushLayer(data : &opaque, area : fgRect, transform : &float, opacity : float) : fgError
  return self.pushLayer(self, data, area, transform, opacity)
end
terra DynamicBackend:PopLayer(data : &opaque) : fgError
  return self.popLayer(self, data)
end
terra DynamicBackend:PushClip(data : &opaque, area : fgRect) : fgError
  return self.pushClip(self, data, area)
end
terra DynamicBackend:PopClip(data : &opaque) : fgError
  return self.popClip(self, data)
end
terra DynamicBackend:DirtyRect(data : &opaque, area : fgRect) : fgError
  return self.dirtyRect(self, data, area)
end

terra DynamicBackend:CreateFont(family : rawstring, weight : uint16, italic : bool, pt : uint32, dpi : fgVec) : &fgFont
  return self.createFont(self, family, weight, italic, pt, dpi)
end 
terra DynamicBackend:DestroyFont(font : &fgFont) : fgError
  return self.destroyFont(self, font)
end
terra DynamicBackend:FontLayout(font : &fgFont, text : rawstring, area : &fgRect, lineHeight : float, letterSpacing : float, previous : &opaque, dpi : fgVec) : &opaque
  return self.fontLayout(self, font, text, area, lineHeight, letterSpacing, previous, dpi)
end
terra DynamicBackend:FontIndex(font : &fgFont, layout : &opaque, area : &fgRect, lineHeight : float, letterSpacing : float, pos : fgVec, cursor : &fgVec, dpi : fgVec) : uint
  return self.fontIndex(self, font, layout, area, lineHeight, letterSpacing, pos, cursor, dpi)
end
terra DynamicBackend:FontPos(font : &fgFont, layout : &opaque, area :&fgRect, lineHeight : float, letterSpacing : float, index : uint, dpi : fgVec) : fgVec
  return self.fontPos(self, font, layout, area, lineHeight, letterSpacing, index, dpi)
end

terra DynamicBackend:CreateAsset(data : rawstring, count : uint, format : BACKEND_FORMATS) : &fgAsset
  return self.createAsset(self, data, count, format)
end
terra DynamicBackend:DestroyAsset(asset : &fgAsset) : fgError
  return self.destroyAsset(self, asset)
end

terra DynamicBackend:PutClipboard(kind : CLIPBOARD_FORMATS, data : rawstring, count : uint) : fgError
  return self.putClipboard(self, kind, data, count)
end
terra DynamicBackend:GetClipboard(kind : CLIPBOARD_FORMATS, target : &opaque, count : uint) : uint
  return self.getClipboard(self, kind, target, count)
end
terra DynamicBackend:CheckClipboard(kind : CLIPBOARD_FORMATS) : bool
  return self.checkClipboard(self, kind)
end
terra DynamicBackend:ClearClipboard(kind : CLIPBOARD_FORMATS) : fgError
  return self.clearClipboard(self, kind)
end

terra DynamicBackend:ProcessMessages(data : &opaque) : fgError
  return self.processMessages(data, self)
end
terra DynamicBackend:SetCursor(data : &opaque, cursor : FG_CURSOR) : fgError
  return self.setCursor(self, data, cursor)
end
terra DynamicBackend:RequestAnimationFrame(data : &opaque, delay : uint64) : fgError
  return self.requestAnimationFrame(self, data, delay)
end

terra DynamicBackend:free(library : &opaque) : {}
  self.destroy(self)
  if library ~= nil then 
    FreeLibrary(library)
  end
end

fgInitBackend = {&opaque} -> &DynamicBackend

-- In cases where the backend is not known at compile time, we must load it via a shared library at runtime 
function DynamicBackend:new(root, path, name)
  return quote
    var l : &opaque = LoadLibrary(path)

    if l == nil then
      return nil, nil
    end

    var str : rawstring = nil
    if name == nil then
      name = C.strrchr(path, ("\\")[0])
      if name == nil then
        name = C.strrchr(path, ("/")[0])
      end
      name = terralib.select(name == nil, path, name)

      str = [rawstring](C.malloc(strlen(name) + 1))
      C.strcpy(str, name)
      name = str

      var ext : rawstring = C.strrchr(str, (".")[0])
      if ext ~= nil then
        @ext = 0
      end
    end

    var f : fgInitBackend = [fgInitBackend](LoadFunction(l, name))

    if str ~= nil then
      C.free(str)
    end

    if f ~= nil then
      var b : &DynamicBackend = f(root)
      if b ~= nil then
        return b, l
      end
    end

    FreeLibrary(l)
    return nil, nil
  end
end