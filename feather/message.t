local F = require 'feather.shared'
local Enum = require 'feather.enum'

local M = {}
M.Idx = uint16

struct M.Msg {
  type : M.Idx
  subtype : M.Idx
  union {}
}

struct M.Result {
  union {}
}

-- fgBehaviorFunction = {fgRoot&, fgElement&, fgMessage&} -> fgMessageResult

struct M.Reciever {}

terra M.Reciever:Construct(target : &opaque) : intptr return 0 end
terra M.Reciever:Destroy() : F.Err return 0 end
terra M.Reciever:Draw(data : &opaque, area : F.Rect) : F.Err return 0 end
terra M.Reciever:MouseDown(x : float, y : float, button : uint8, all : uint8) : F.Err return 0 end
terra M.Reciever:MouseDblClick(x : float, y : float, button : uint8, all : uint8) : F.Err return 0 end
terra M.Reciever:MouseUp(x : float, y : float, button : uint8, all : uint8) : F.Err return 0 end
terra M.Reciever:MouseOn(x : float, y : float, button : uint8, all : uint8) : F.Err return 0 end
terra M.Reciever:MouseOff(x : float, y : float, button : uint8, all : uint8) : F.Err return 0 end
terra M.Reciever:MouseMove(x : float, y : float, button : uint8, all : uint8) : F.Err return 0 end
terra M.Reciever:MouseScroll(x : float, y : float, delta : int16, hdelta : int16) : F.Err return 0 end
terra M.Reciever:TouchBegin(x : float, y : float, index : int16) : F.Err return 0 end
terra M.Reciever:TouchMove(x : float, y : float, index : int16) : F.Err return 0 end
terra M.Reciever:TouchEnd(x : float, y : float, index : int16) : F.Err return 0 end
terra M.Reciever:KeyUp(code : uint8, sigkeys : uint8) : F.Err return 0 end
terra M.Reciever:KeyDown(code : uint8, sigkeys : uint8) : F.Err return 0 end
terra M.Reciever:KeyChar(unicode : int32, sigkeys : uint8) : F.Err return 0 end
terra M.Reciever:JoyButtonDown(button : uint16) : F.Err return 0 end
terra M.Reciever:JoyButtonUp(button : uint16) : F.Err return 0 end
terra M.Reciever:JoyAxis(value : float, axis : uint16) : F.Err return 0 end
terra M.Reciever:GotFocus() : F.Err return 0 end
terra M.Reciever:LostFocus() : F.Err return 0 end
terra M.Reciever:DragDrop() : F.Err return 0 end

local msgenum = {}

-- Go through all our message functions and generate the parameter and result unions (TODO: sort these)
for k, v in pairs(M.Reciever.methods) do
    local s = struct{}
    s.name = "Msg"..k
    k = F.camelCase(k)
    s.entries = v.definition.parameters:map(function(e) return {field = e.symbol.displayname, type = e.symbol.type} end)
    M.Msg.entries[3]:insert({field = k, type = s})
    M.Result.entries[1]:insert({field = k, type = v.type.returntype})
    table.insert(msgenum, string.upper(k))
end

if terralib.sizeof(M.Result) ~= terralib.sizeof(intptr) then
  error("Msg.Result ("..terralib.sizeof(M.Result)..") should be the size of a pointer ("..terralib.sizeof(intptr)..")")
end

M.Kind = Enum(msgenum)

--function M.MakeBehaviorFunction(T) 
--  for k, v in pairs(T.methods) do
--    if M.Reciever.methods[k] ~= nil then
--
--    end
--  end
--end

return M