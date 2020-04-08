local F = require 'feather.shared'
local CT = require 'std.constraint'

-- This defines an abstract interface for the spatial partitioning data structure used by feather.
return CT.MetaConstraint(function(a) return CT.All(
  CT.Method("create", {CT.Pointer(a), &F.Vec3D, &F.Vec3D, &F.Vec3D, &F.Veci}, CT.Pointer(a)),
  CT.Method("destroy", {CT.Pointer(a)}, nil),
  CT.Method("fix", {CT.Pointer(a)}, nil),
  CT.Method("set", {CT.Pointer(a), &F.Vec3D, &F.Vec3D, &F.Vec3D, &F.Veci}, nil),
  CT.Method("setdata", {CT.Pointer(a), &opaque}, nil),
  CT.Method("setparent", {CT.Pointer(a), CT.Pointer(a)}, nil),
  CT.MetaConstraint(function(t) return CT.Method("orthoquery", {F.Vec, CT.Value(t), CT.Function({CT.Pointer(a), CT.Value(t)}, bool)}, nil) end, CT.TerraType),
  CT.MetaConstraint(function(t) return CT.Method("orthodraw", {F.Rect, CT.Value(t), CT.Function({CT.Pointer(a), CT.Value(t), &F.Rect})}, nil) end, CT.TerraType)
) end, CT.TerraType)