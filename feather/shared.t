local S = {}

S.Err = int
S.Delegate = &opaque -> {}
S.Version = { 0, 1, 0 }

local Color = require 'feather.color'

S.Color = Color(8,8,8,8)
S.Color16 = Color(16,16,16,16)

local Rect = require 'feather.rect'

S.Rect = Rect(float)
S.Rect.c_export = [[union {
    struct
    {
      float left;
      float top;
      float right;
      float bottom;
    };

    struct
    {
      FG_Vec topleft;
      FG_Vec bottomright;
    };

    float ltrb[4];
  };]]

struct S.URect {
  abs : S.Rect 
  rel : S.Rect
}

local Vec = require 'feather.vec'

S.Vec = Vec(float, 2)
S.Vec3D = Vec(float, 3)
S.Veci = Vec(int, 2)

S.Vec.c_export = [[float x;
  float y;]]
S.Veci.c_export = [[int x;
  int y;]]
S.Vec3D.c_export = [[float x;
  float y;
  float z;]]

struct S.UVec {
  abs : S.Vec
  rel : S.Vec
}

function S.camelCase(k) return string.lower(k:sub(1,1)) .. k:sub(2, -1) end

return S
