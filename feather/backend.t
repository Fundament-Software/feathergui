local C = terralib.includecstring [[
  #include <string.h>
]]

local B = {}
local F = require 'feather.shared'
local Flags = require 'feather.flags'
local Enum = require 'feather.enum'
local OS = require 'feather.os'
local Alloc = require 'std.alloc'

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

struct DynamicBackend {
  destroy : {&DynamicBackend} -> {}

  features : B.Features
  formats : uint
  dpi : F.Vec
  scale : float
  cursorblink : uint64
  tooltipdelay : uint64
}

terra DynamicBackend:DrawFont(data : &opaque, font : &B.Font, layout : &opaque, area : &F.Rect, color : F.Color, lineHeight : float, letterSpacing : float, blur : float, aa : B.AntiAliasing) : F.Err return 0 end
terra DynamicBackend:DrawAsset(data : &opaque, asset : &B.Asset, area : &F.Rect, source : &F.Rect, color : F.Color, time : float) : F.Err return 0 end
terra DynamicBackend:DrawRect(data : &opaque, area : &F.Rect, corners : &F.Rect, fillColor : F.Color, border : float, borderColor : F.Color, blur : float, asset : &B.Asset) : F.Err return 0 end
terra DynamicBackend:DrawCircle(data : &opaque, area : &F.Rect, arcs : &F.Rect, fillColor : F.Color, border : float, borderColor : F.Color, blur : float, asset : &B.Asset) : F.Err return 0 end
terra DynamicBackend:DrawTriangle(data : &opaque, area : &F.Rect, corners : &F.Rect, fillColor : F.Color, border : float, borderColor : F.Color, blur : float, asset : &B.Asset) : F.Err return 0 end
terra DynamicBackend:DrawLines(data : &opaque, points : &F.Vec, count : uint, color : F.Color) : F.Err return 0 end
terra DynamicBackend:DrawCurve(data : &opaque, anchors : &F.Vec, count : uint, fillColor : F.Color, stroke : float, strokeColor : F.Color) : F.Err return 0 end
terra DynamicBackend:PushLayer(data : &opaque, area : F.Rect, transform : &float, opacity : float) : F.Err return 0 end
terra DynamicBackend:PopLayer(data : &opaque) : F.Err return 0 end
terra DynamicBackend:PushClip(data : &opaque, area : F.Rect) : F.Err return 0 end
terra DynamicBackend:PopClip(data : &opaque) : F.Err return 0 end
terra DynamicBackend:DirtyRect(data : &opaque, area : F.Rect) : F.Err return 0 end

terra DynamicBackend:CreateFont(family : rawstring, weight : uint16, italic : bool, pt : uint32, dpi : F.Vec) : &B.Font return nil end
terra DynamicBackend:DestroyFont(font : &B.Font) : F.Err return 0 end
terra DynamicBackend:FontLayout(font : &B.Font, text : rawstring, area : &F.Rect, lineHeight : float, letterSpacing : float, previous : &opaque, dpi : F.Vec) : &opaque return nil end
terra DynamicBackend:FontIndex(font : &B.Font, layout : &opaque, area : &F.Rect, lineHeight : float, letterSpacing : float, pos : F.Vec, cursor : &F.Vec, dpi : F.Vec) : uint return 0 end
terra DynamicBackend:FontPos(font : &B.Font, layout : &opaque, area :&F.Rect, lineHeight : float, letterSpacing : float, index : uint, dpi : F.Vec) : F.Vec return F.Vec{} end

terra DynamicBackend:CreateAsset(data : rawstring, count : uint, format : B.Formats) : &B.Asset return nil end
terra DynamicBackend:DestroyAsset(asset : &B.Asset) : F.Err return 0 end

terra DynamicBackend:PutClipboard(kind : B.Clipboard, data : rawstring, count : uint) : F.Err return 0 end
terra DynamicBackend:GetClipboard(kind : B.Clipboard, target : &opaque, count : uint) : uint return 0 end
terra DynamicBackend:CheckClipboard(kind : B.Clipboard) : bool return false end
terra DynamicBackend:ClearClipboard(kind : B.Clipboard) : F.Err return 0 end

terra DynamicBackend:ProcessMessages(data : &opaque) : F.Err return 0 end
terra DynamicBackend:SetCursor(data : &opaque, cursor : B.Cursor) : F.Err return 0 end
terra DynamicBackend:RequestAnimationFrame(data : &opaque, delay : uint64) : F.Err return 0 end
terra DynamicBackend:GetMonitorIndex(index : uint, out : &B.Display) : F.Err return 0 end
terra DynamicBackend:GetMonitor(handle : uint64, out : &B.Display) : F.Err return 0 end
terra DynamicBackend:SpawnWindow(element : &opaque, area : &F.Rect, caption : rawstring) : &opaque return nil end
terra DynamicBackend:DespawnWindow(handle : &opaque) : F.Err return 0 end

-- Generate both the function pointer field and redefine the function to call the generated function pointer
do
  local map = {}
  for k, v in pairs(DynamicBackend.methods) do
    k = string.lower(k:sub(1,1)) .. k:sub(2, -1)
    DynamicBackend.entries:insert({field = k, type = terralib.types.pointer(v:gettype())})
    map[k] = v -- TODO: alphabetically sort map before putting in entries, or use a C struct sorting method on the resulting struct
  end

  for k, v in pairs(map) do
    local args = v.type.parameters:map(symbol)
    v:resetdefinition(terra([args]): v.type.returntype return [args[1]].[k]([args]) end)
  end
end

terra DynamicBackend:free(library : &opaque) : {}
  self.destroy(self)
  if library ~= nil then 
    OS.FreeLibrary(library)
  end
end

B.InitBackend = {&opaque} -> &DynamicBackend

terra LoadDynamicBackend(ui : &opaque, path : rawstring, name : rawstring) : {&DynamicBackend, &opaque}
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

    str = Alloc.default_allocator:alloc(int8, C.strlen(name) + 1)
    C.strcpy(str, name)
    name = str

    var ext : rawstring = C.strrchr(str, (".")[0])
    if ext ~= nil then
      @ext = 0
    end
  end

  var f : B.InitBackend = [B.InitBackend](OS.LoadFunction(l, name))

  if str ~= nil then
    Alloc.free(str)
  end

  if f ~= nil then
    var b : &DynamicBackend = f(ui)
    if b ~= nil then
      return b, l
    end
  end

  OS.FreeLibrary(l)
  return nil, nil
end

-- In cases where the backend is not known at compile time, we must load it via a shared library at runtime 
function DynamicBackend:new(root, path, name)
  return quote in LoadDynamicBackend(root, path, name) end
end

return B