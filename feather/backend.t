local C = terralib.includecstring [[
  #include <string.h>
]]

local B = {}
local F = require 'feather.shared'
local Flags = require 'feather.flags'
local Enum = require 'feather.enum'
local OS = require 'feather.os'

B.Features = Flags{
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

B.Formats = Enum{
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

B.Formats.UNKNOWN = `0xff

B.AntiAliasing = Enum{
  "NO_AA",
  "AA",
  "LCD",
  "LCD_V"}

B.Clipboard = Enum{
  "NONE",
  "TEXT",
  "WAVE",
  "BITMAP",
  "FILE",
  "ELEMENT",
  "CUSTOM",
  "ALL"}

B.Cursor = Enum{
  "NONE",
  "ARROW",
  "IBEAM",
  "CROSS",
  "WAIT",
  "HAND",
  "RESIZENS",
  "RESIZEWE",
  "RESIZENWSE",
  "RESIZENESW",
  "RESIZEALL",
  "NO",
  "HELP",
  "DRAG",
  "CUSTOM"}

struct B.Data {
  data : &opaque
}

struct B.Font {
  data : B.Data
  dpi : F.Vec
  baseline : float
  lineheight : float
  pt : uint
}

struct B.Asset {
  data : B.Data
  size : F.Veci
  dpi : F.Vec
  format : B.Formats
}

struct B.Display {
  size : F.Veci
  dpi : F.Vec
  handle : uint64
  primary : bool
}

struct Element {}

struct DynamicBackend {
  drawFont : {&DynamicBackend, &opaque, &B.Font, &opaque, &F.Rect, F.Color, float, float, float, B.AntiAliasing} -> F.Err
  drawAsset : {&DynamicBackend, &opaque, &B.Asset, &F.Rect, &F.Rect, F.Color, float} -> F.Err
  drawRect : {&DynamicBackend, &opaque, &F.Rect, &F.Rect, F.Color, float, F.Color, float, &B.Asset} -> F.Err
  drawCircle : {&DynamicBackend, &opaque, &F.Rect, &F.Rect, F.Color, float, F.Color, float, &B.Asset} -> F.Err
  drawTriangle : {&DynamicBackend, &opaque, &F.Rect, &F.Rect, F.Color, float, F.Color, float, &B.Asset} -> F.Err
  drawLines : {&DynamicBackend, &opaque, &F.Vec, uint, F.Color} -> F.Err
  drawCurve : {&DynamicBackend, &opaque, &F.Vec, uint, F.Color, float, F.Color} -> F.Err
  --drawShader : {&DynamicBackend, &opaque} -> F.Err
  pushLayer : {&DynamicBackend, &opaque, F.Rect, &float, float} -> F.Err
  popLayer : {&DynamicBackend, &opaque} -> F.Err
  pushClip : {&DynamicBackend, &opaque, F.Rect} -> F.Err
  popClip : {&DynamicBackend, &opaque} -> F.Err
  dirtyRect : {&DynamicBackend, &opaque, F.Rect} -> F.Err

  createFont : {&DynamicBackend, rawstring, uint16, bool, uint32, F.Vec} -> &B.Font
  destroyFont : {&DynamicBackend, &B.Font} -> F.Err
  fontLayout : {&DynamicBackend, &B.Font, rawstring, &F.Rect, float, float, &opaque, F.Vec} -> &opaque
  fontIndex : {&DynamicBackend, &B.Font, &opaque, &F.Rect, float, float, F.Vec, &F.Vec, F.Vec} -> uint
  fontPos : {&DynamicBackend, &B.Font, &opaque, &F.Rect, float, float, uint, F.Vec} -> F.Vec

  createAsset : {&DynamicBackend, rawstring, uint, B.Formats} -> &B.Asset
  destroyAsset : {&DynamicBackend, &B.Asset} -> F.Err

  putClipboard : {&DynamicBackend, B.Clipboard, rawstring, uint} -> F.Err
  getClipboard : {&DynamicBackend, B.Clipboard, &opaque, uint} -> uint
  checkClipboard : {&DynamicBackend, B.Clipboard} -> bool
  clearClipboard : {&DynamicBackend, B.Clipboard} -> F.Err

  processMessages : {&opaque, &DynamicBackend} -> F.Err
  setCursor : {&DynamicBackend, &opaque, B.Cursor} -> F.Err
  requestAnimationFrame : {&DynamicBackend, &opaque, uint64} -> F.Err
  destroy : {&DynamicBackend} -> {}
  getMonitorIndex : {&DynamicBackend, uint, &B.Display} -> {F.Err}
  getMonitor : {&DynamicBackend, uint64, &B.Display} -> {F.Err}
  spawnWindow : {&Element, &F.Rect, rawstring} -> {&opaque}
  despawnWindow : {&opaque} -> F.Err

  features : B.Features
  formats : uint
  dpi : F.Vec
  scale : float
  cursorblink : uint64
  tooltipdelay : uint64
}

-- It's not clear if we should be generating member functions from the function pointers or vice-versa
--for _, entry in ipairs(DynamicBackend:getentries()) do
--  if entry.type:ispointertofunction() then 
--    DynamicBackend.methods[string.upper(entry.field:sub(1,1)) .. entry.field:sub(2, -1)]
--  end
--end

terra DynamicBackend:DrawFont(data : &opaque, font : &B.Font, layout : &opaque, area : &F.Rect, color : F.Color, lineHeight : float, letterSpacing : float, blur : float, aa : B.AntiAliasing) : F.Err
  return self.drawFont(self, data, font, layout, area, color, lineHeight, letterSpacing, blur, aa)
end
terra DynamicBackend:DrawAsset(data : &opaque, asset : &B.Asset, area : &F.Rect, source : &F.Rect, color : F.Color, time : float) : F.Err
  return self.drawAsset(self, data, asset, area, source, color, time)
end
terra DynamicBackend:DrawRect(data : &opaque, area : &F.Rect, corners : &F.Rect, fillColor : F.Color, border : float, borderColor : F.Color, blur : float, asset : &B.Asset) : F.Err
  return self.drawRect(self, data, area, corners, fillColor, border, borderColor, blur, asset)
end
terra DynamicBackend:DrawCircle(data : &opaque, area : &F.Rect, arcs : &F.Rect, fillColor : F.Color, border : float, borderColor : F.Color, blur : float, asset : &B.Asset) : F.Err
  return self.drawCircle(self, data, area, arcs, fillColor, border, borderColor, blur, asset)
end
terra DynamicBackend:DrawTriangle(data : &opaque, area : &F.Rect, corners : &F.Rect, fillColor : F.Color, border : float, borderColor : F.Color, blur : float, asset : &B.Asset) : F.Err
  return self.drawTriangle(self, data, area, corners, fillColor, border, borderColor, blur, asset)
end
terra DynamicBackend:DrawLines(data : &opaque, points : &F.Vec, count : uint, color : F.Color) : F.Err
  return self.drawLines(self, data, points, count, color)
end
terra DynamicBackend:DrawCurve(data : &opaque, anchors : &F.Vec, count : uint, fillColor : F.Color, stroke : float, strokeColor : F.Color) : F.Err
  return self.drawCurve(self, data, anchors, count, fillColor, stroke, strokeColor)
end
terra DynamicBackend:PushLayer(data : &opaque, area : F.Rect, transform : &float, opacity : float) : F.Err
  return self.pushLayer(self, data, area, transform, opacity)
end
terra DynamicBackend:PopLayer(data : &opaque) : F.Err
  return self.popLayer(self, data)
end
terra DynamicBackend:PushClip(data : &opaque, area : F.Rect) : F.Err
  return self.pushClip(self, data, area)
end
terra DynamicBackend:PopClip(data : &opaque) : F.Err
  return self.popClip(self, data)
end
terra DynamicBackend:DirtyRect(data : &opaque, area : F.Rect) : F.Err
  return self.dirtyRect(self, data, area)
end

terra DynamicBackend:CreateFont(family : rawstring, weight : uint16, italic : bool, pt : uint32, dpi : F.Vec) : &B.Font
  return self.createFont(self, family, weight, italic, pt, dpi)
end 
terra DynamicBackend:DestroyFont(font : &B.Font) : F.Err
  return self.destroyFont(self, font)
end
terra DynamicBackend:FontLayout(font : &B.Font, text : rawstring, area : &F.Rect, lineHeight : float, letterSpacing : float, previous : &opaque, dpi : F.Vec) : &opaque
  return self.fontLayout(self, font, text, area, lineHeight, letterSpacing, previous, dpi)
end
terra DynamicBackend:FontIndex(font : &B.Font, layout : &opaque, area : &F.Rect, lineHeight : float, letterSpacing : float, pos : F.Vec, cursor : &F.Vec, dpi : F.Vec) : uint
  return self.fontIndex(self, font, layout, area, lineHeight, letterSpacing, pos, cursor, dpi)
end
terra DynamicBackend:FontPos(font : &B.Font, layout : &opaque, area :&F.Rect, lineHeight : float, letterSpacing : float, index : uint, dpi : F.Vec) : F.Vec
  return self.fontPos(self, font, layout, area, lineHeight, letterSpacing, index, dpi)
end

terra DynamicBackend:CreateAsset(data : rawstring, count : uint, format : B.Formats) : &B.Asset
  return self.createAsset(self, data, count, format)
end
terra DynamicBackend:DestroyAsset(asset : &B.Asset) : F.Err
  return self.destroyAsset(self, asset)
end

terra DynamicBackend:PutClipboard(kind : B.Clipboard, data : rawstring, count : uint) : F.Err
  return self.putClipboard(self, kind, data, count)
end
terra DynamicBackend:GetClipboard(kind : B.Clipboard, target : &opaque, count : uint) : uint
  return self.getClipboard(self, kind, target, count)
end
terra DynamicBackend:CheckClipboard(kind : B.Clipboard) : bool
  return self.checkClipboard(self, kind)
end
terra DynamicBackend:ClearClipboard(kind : B.Clipboard) : F.Err
  return self.clearClipboard(self, kind)
end

terra DynamicBackend:ProcessMessages(data : &opaque) : F.Err
  return self.processMessages(data, self)
end
terra DynamicBackend:SetCursor(data : &opaque, cursor : B.Cursor) : F.Err
  return self.setCursor(self, data, cursor)
end
terra DynamicBackend:RequestAnimationFrame(data : &opaque, delay : uint64) : F.Err
  return self.requestAnimationFrame(self, data, delay)
end

terra DynamicBackend:free(library : &opaque) : {}
  self.destroy(self)
  if library ~= nil then 
    OS.FreeLibrary(library)
  end
end

B.InitBackend = {&opaque} -> &DynamicBackend

-- In cases where the backend is not known at compile time, we must load it via a shared library at runtime 
function DynamicBackend:new(root, path, name)
  return quote
    var l : &opaque = OS.LoadLibrary(path)

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

    var f : B.InitBackend = [B.InitBackend](OS.LoadFunction(l, name))

    if str ~= nil then
      C.free(str)
    end

    if f ~= nil then
      var b : &DynamicBackend = f(root)
      if b ~= nil then
        return b, l
      end
    end

    OS.FreeLibrary(l)
    return nil, nil
  end
end

return B