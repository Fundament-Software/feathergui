-- This is a brute-force simple partitioner implementation. Slow for complex layouts, but useful for testing.

local F = require 'feather.shared'
local Alloc = require 'std.alloc'
local CT = require 'std.constraint'
local Math = require 'std.math'

local struct Node {
  pos : F.Vec3D
  dim : F.Vec3D
  rot : F.Vec3D
  zindex : F.Veci
  data : &opaque
  next : &Node
  prev : &Node
  child : &Node
  parent : &Node
}

-- Note: if this was a real partition attempt, we would enable custom allocators, but we don't care.
local struct SimplePartition {
  root : &Node
}

SimplePartition.Node = Node

terra SimplePartition:create(parent : &Node, pos : &F.Vec3D, dim : &F.Vec3D, rot : &F.Vec3D, zindex : &F.Veci) : &Node
  var n : &Node = Alloc.alloc(Node, 1)
  n.prev = nil
  var root : &&Node = terralib.select(parent ~= nil, &parent.child, &self.root)
  n.next = @root
  if n.next == nil then
    @root = n
  else
    n.next.prev = n
  end
  self:set(n, pos, dim, rot, zindex)
  return n
end

terra SimplePartition:destroy(node : &Node) : {}
  self:_detach(node)

  while node.child ~= nil do
    self:destroy(node.child)
  end

  Alloc.free(node)
end

-- This is a very inaccurate "fixer" that just creates a maximal bounding box around the sphere equivalent for any rotational values.
terra SimplePartition:fix(node : &Node) : {}
  var cur : &Node = node.child
  if cur ~= nil then
    var pos : F.Vec3D = cur.pos
    var dim : F.Vec3D = [F.Vec3D]{array(0.0f,0.0f,0.0f)}

    while cur ~= nil do
      var extent : F.Vec3D = cur.dim
      if cur.rot.x ~= 0.0f or cur.rot.y ~= 0.0f or cur.rot.z ~= 0.0f then -- give up and treat it like a giant ball
        var r : float = Math.sqrt(dim.x*dim.x + dim.y*dim.y + dim.z*dim.z)
        extent = [F.Vec3D]{array(r,r,r)}
      end
      escape
      for i = 0,2 do emit(quote pos.v[i] = Math.min(pos.v[i], cur.pos.v[i] - extent.v[i]) end) end
      for i = 0,2 do emit(quote dim.v[i] = Math.max(pos.v[i], cur.pos.v[i] + extent.v[i]) end) end
      end
    end

    node.dim = dim / 2.0f
    node.pos = pos + node.dim
  end
end

terra SimplePartition:set(node : &Node, pos : &F.Vec3D, dim : &F.Vec3D, rot : &F.Vec3D, zindex : &F.Veci) : {}
  node.pos = @pos
  node.dim = @dim
  node.rot = @rot
  node.zindex = @zindex
end

terra SimplePartition:setdata(node : &Node, data : &opaque) : {}
  node.data = data
end

terra SimplePartition:setparent(node : &Node, parent : &Node) : {}
  self:_detach(node)
  if(parent == nil) then
    node.next = self.root;
    self.root = node;
  else
    node.next = parent.child;
    parent.child = node;
  end
  if node.next ~= nil then
    node.next.prev = node;
  end
end

terra SimplePartition:_detach(node : &Node) : {}
  if node.prev == nil then
    if node.parent == nil then
      self.root = node.next
    else
      node.parent.child = node.next
    end
  else
    node.prev.next = node.next
  end
end

SimplePartition.methods.orthoquery = CT(function(t) return CT.TemplateMethod(SimplePartition, {F.Vec, CT.Value(t), CT.Function({&Node, CT.Value(t)}, bool)}, nil,
function (S, P, D, FN)
  local terra recurse(n : &Node, p : F.Vec, d : D, f : FN) : bool
    var proj = n.pos.xy / n.pos.z
    var pos = p - proj
    if Math.abs(pos.x) > n.dim.x or Math.abs(pos.y) > n.dim.y then
      return false
    end

    var cur = n.child
    while cur ~= nil and cur.zindex.x >= 0 do -- x is the "topmost" zindex
      if recurse(cur, p, d, f) then
        return true
      end
    end

    if f(n, d) then
      return true
    end

    while cur ~= nil do
      if recurse(cur, p, d, f) then
        return true
      end
    end

    return false
  end

  return terra(self : S, p : P, data : D, fn : FN) : {}
    var cur = self.root
    while cur ~= nil do
      if recurse(cur, p, data, fn) then
        break
      end
    end
  end
end) end, CT.TerraType)

SimplePartition.methods.orthodraw = CT(function(t) return CT.TemplateMethod(SimplePartition, {F.Rect, CT.Value(t), CT.Function({&Node, CT.Value(t), &F.Rect})}, nil,
function (self, rect, data, fn)

end) end, CT.TerraType)

terra SimplePartition:init() : {}
  self.root = nil
end

terra SimplePartition:destruct() : {}
  while self.root ~= nil do
    var n : &Node = self.root.next
    Alloc.free(self.root)
    self.root = n
  end
end

return SimplePartition