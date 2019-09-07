local C = require 'feather.libc'

local Array = require 'feather.array'
local F = require 'feather.shared'
local Backend = require 'feather.backend'
local Enum = require 'feather.enum'
local Flags = require 'feather.flags'
local Message = require 'feather.message'
local Hash = require 'feather.hash'
local cond = require 'std.cond'
local OS = require 'feather.os'
local Constraint = require 'std.constraint'
--local String = require 'feather.string'

local struct TestHarness {
  passed : int;
  total : int;
}

TestHarness.methods.Test = macro(function(self, result, expected, op)
  op = op or "__eq"
  if self:gettype() ~= TestHarness then
    error ("self must point to TestHarness: " .. self:gettype():layoutstring())
  end
  return quote
    self.total = self.total + 1
    var r = result
    var e = expected
    if operator(op, r, e) then
      self.passed = self.passed + 1
    else
      --s = String:append("Test Failed, expected ", e, " but got ", r, ": ", [ result:prettystring() ])
      --buf = s:torawstring()
      --C.printf(buf)
      
      C.printf("Test Failed: %s", [ result:prettystring() ])
    end
  end
end)

terra TestHarness:array()
  var a : Array(int)
  a:add(1)
  self:Test(a(0), 1)
  self:Test(a.size, 1)
  a:reserve(5)
  self:Test(a.capacity, 5)
  self:Test(a.size, 1)
  a:add(2)
  self:Test(a.size, 2)
  a:add(3)
  self:Test(a.size, 3)
  a:add(4)
  self:Test(a.size, 4)
  a:insert(5, 4)
  self:Test(a.size, 5)
  self:Test(a(1), 2)
  self:Test(a(2), 3)
  self:Test(a(3), 4)
  self:Test(a(4), 5)
  a:insert(0, 0)
  self:Test(a.size, 6)
  self:Test(a.capacity, 6, "__ge")
  self:Test(a(0), 0)
  self:Test(a(1), 1)
  self:Test(a(2), 2)
  self:Test(a(3), 3)
  self:Test(a(4), 4)
  self:Test(a(5), 5)

  a:clear()
  self:Test(a.size, 0)
  a:add(1)
  self:Test(a.size, 1)
  self:Test(a(0), 1)
end

terra TestHarness:backend()

end

terra TestHarness:color()
  var c : F.Color = F.Color{ 0 }
  self:Test(c.r, 0)
  self:Test(c.g, 0)
  self:Test(c.b, 0)
  self:Test(c.a, 0)
  self:Test(c.v, 0)

  c.r = 0xFF
  self:Test(c.r, 0xFF)
  self:Test(c.g, 0)
  self:Test(c.b, 0)
  self:Test(c.a, 0)
  self:Test(c.v, 0x00FF0000)

  c.g = 0xFF
  c.b = 0xFF
  c.a = 0xFF
  self:Test(c.r, 0xFF)
  self:Test(c.g, 0xFF)
  self:Test(c.b, 0xFF)
  self:Test(c.a, 0xFF)
  self:Test(c.v, 0xFFFFFFFF)

  c.r = 0x34
  c.g = 0x56
  c.b = 0x78
  c.a = 0x12
  self:Test(c.r, 0x34)
  self:Test(c.g, 0x56)
  self:Test(c.b, 0x78)
  self:Test(c.a, 0x12)
  self:Test(c.v, 0x12345678)
  
  var c2 = F.Color { 0x12345678 }
  self:Test(c2.r, 0x34)
  self:Test(c2.g, 0x56)
  self:Test(c2.b, 0x78)
  self:Test(c2.a, 0x12)
  self:Test(c2.v, 0x12345678)
end

local TestEnum = Enum{
  "First",
  "second",
  "THIRD",
}

terra TestHarness:enum()
  self:Test(TestEnum.First.v, 0)
  self:Test(TestEnum.second.v, 1)
  self:Test(TestEnum.THIRD.v, 2)

  var e : TestEnum = TestEnum.First
  self:Test(e.v, 0)
  e.v = 0
  e = TestEnum.second
  e.v = 1
  self:Test(e.v, 1)
  e = TestEnum.THIRD
  self:Test(e.v, 2)
  e.v = 2
  self:Test(e, TestEnum.THIRD)
end

local TestFlags = Flags{
  "First",
  "second",
  "THIRD",
}

terra TestHarness:flags()
  self:Test(TestFlags.First.val, 1)
  self:Test(TestFlags.second.val, 2)
  self:Test(TestFlags.THIRD.val, 4)
  var f = TestFlags.First + TestFlags.THIRD
  self:Test(f.First and f.second, false)
  self:Test(f.First, true)
  self:Test(not f.second, true)
  self:Test(f.THIRD, true)
  f.THIRD = false
  f.second = true
  self:Test(f.First and f.second, true)
  self:Test(f.First and f.second and not f.THIRD, true)
end

terra TestHarness:hash()

end

terra TestHarness:message()

end

terra TestHarness:rect()
  var a = F.Rect{array(1f,2f,3f,4f)}
  self:Test(a.l, 1)
  self:Test(a.t, 2)
  self:Test(a.r, 3)
  self:Test(a.b, 4)
  self:Test(a.left, 1)
  self:Test(a.top, 2)
  self:Test(a.right, 3)
  self:Test(a.bottom, 4)
  self:Test(a.data[0], 1)
  self:Test(a.data[1], 2)
  self:Test(a.data[2], 3)
  self:Test(a.data[3], 4)

  var b = F.Rect{array(5f,6f,7f,8f)}
  var c = a + b
  self:Test(c.l, 6f)
  self:Test(c.t, 8f)
  self:Test(c.r, 10f)
  self:Test(c.b, 12f)
  c = b - a
  self:Test(c.l, 4f)
  self:Test(c.t, 4f)
  self:Test(c.r, 4f)
  self:Test(c.b, 4f)
  c = a - b
  self:Test(c.l, -4f)
  self:Test(c.t, -4f)
  self:Test(c.r, -4f)
  self:Test(c.b, -4f)
  c = b * a
  self:Test(c.l, 5f)
  self:Test(c.t, 12f)
  self:Test(c.r, 21f)
  self:Test(c.b, 32f)
  c = b / a
  self:Test(c.l, 5f)
  self:Test(c.t, 3f)
  self:Test(c.r, (7f/3f))
  self:Test(c.b, 2f)

  c = a + 1f
  self:Test(c.l, 2f)
  self:Test(c.t, 3f)
  self:Test(c.r, 4f)
  self:Test(c.b, 5f)
  c = 1f + a
  self:Test(c.l, 2f)
  self:Test(c.t, 3f)
  self:Test(c.r, 4f)
  self:Test(c.b, 5f)
  c = 2f * a
  self:Test(c.l, 2f)
  self:Test(c.t, 4f)
  self:Test(c.r, 6f)
  self:Test(c.b, 8f)

  var d = a.topleft + b.bottomright
  self:Test(d.x, 8)
  self:Test(d.y, 10)

  self:Test(a, b, "__ne")
  self:Test(a, a)
  self:Test(a == b, false)
end

terra TestHarness:vec()
  var a = F.Veci{array(1,2)}
  self:Test(a.x, 1)
  self:Test(a.y, 2)
  self:Test(a.v[0], 1)
  self:Test(a.v[1], 2)
  a.x = 5
  a.y = 6
  self:Test(a.x, 5)
  self:Test(a.y, 6)

  var b = F.Veci{array(-3,-4)}
  var c = a + b
  self:Test(c.x, 2)
  self:Test(c.y, 2)
  c = a - b
  self:Test(c.x, 8)
  self:Test(c.y, 10)
  c = a * b
  self:Test(c.x, -15)
  self:Test(c.y, -24)
  a.x = 1
  a.y = 2
  c = b / a
  self:Test(c.x, -3)
  self:Test(c.y, -2)

  c = a + 3
  self:Test(c.x, 4)
  self:Test(c.y, 5)

  c = 4 - b
  self:Test(c.x, 7)
  self:Test(c.y, 8)

  self:Test(a, b, "__ne")
  b.x = a.x
  b.y = a.y
  self:Test(a, b)
end

local SUBTESTLEN = 11

local terra PrintColumns(a : rawstring, b : rawstring, c : rawstring) : {}
  C.printf("%-*s %-*s %-*s\n", 24, a, [SUBTESTLEN], b, 8, c);
end

local T = {}

terra T.main(argc : int, argv : &rawstring) : int
  C.printf("Feather v%i.%i.%i Test Utility\n\n", [F.Version[1]], [F.Version[2]], [F.Version[3]])

  var harness = TestHarness{ }
  
  PrintColumns("Internal Tests", "Subtests", "Pass/Fail")
  PrintColumns("--------------", "--------", "---------")

  escape 
    for k, v in pairs(TestHarness.methods) do
      if not terralib.ismacro(v) then
        emit(quote
          harness.passed, harness.total = 0, 0
          harness:[k]()
          var buf : int8[SUBTESTLEN + 1]
          C.snprintf(buf, [SUBTESTLEN + 1], "%u/%u", harness.passed, harness.total);
          PrintColumns(k, buf, terralib.select(harness.passed == harness.total, "PASS", "FAIL"))
        end)
      end
    end
  end

  return 0;
end

return T
