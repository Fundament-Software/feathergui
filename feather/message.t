local F = require 'feather.shared'

local M = {}
M.Idx = uint16

struct M.Msg {
  type : M.Idx;
  subtype : M.Idx;
  union {
    construct : &opaque;
  }
}

struct M.Result {
  union {
    error : F.Err;
    ptr : &opaque;
    construct : uint;
  }
}

-- fgBehaviorFunction = {fgRoot&, fgElement&, fgMessage&} -> fgMessageResult

return M