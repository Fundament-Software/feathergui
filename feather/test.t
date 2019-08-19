local C = terralib.includecstring [[
  #include <stdio.h>
  #include <stdlib.h>
]]

local Array = require 'feather.array'
local F = require 'feather.shared'
local Backend = require 'feather.backend'
local Enum = require 'feather.enum'
local Flags = require 'feather.flags'
local Message = require 'feather.message'
local OS = require 'feather.os'

local struct TestHarness {
  passed : int;
  total : int;
}

local terra Test(self : &TestHarness, exp : bool)
    self.total = self.total + 1
    if exp then
      self.passed = self.passed + 1
    end
end

--[[local Test = macro(function(self, exp)
  return quote
    self.total = self.total + 1
    if exp then
      self.passed = self.passed + 1
    else
      C.printf("Test Failed: %s", [ exp:printpretty() ])
    end
  end
end) --]]

terra TestHarness:array()
  var a : &Array(int) = Array(int).new()
  a:add(1)
  Test(self, a(0) == 1)
  Test(self, a.size == 1)
  a:reserve(5)
  Test(self, a.capacity == 5)
  Test(self, a.size == 1)
  a:add(2)
  Test(self, a.size == 2)
  a:add(3)
  Test(self, a.size == 3)
  a:add(4)
  Test(self, a.size == 4)
  a:insert(5, 4)
  Test(self, a.size == 5)
  Test(self, a(1) == 2)
  Test(self, a(2) == 3)
  Test(self, a(3) == 4)
  Test(self, a(4) == 5)
  a:insert(0, 0)
  Test(self, a.size == 6)
  Test(self, a.capacity >= 6)
  Test(self, a(0) == 0)
  Test(self, a(1) == 1)
  Test(self, a(2) == 2)
  Test(self, a(3) == 3)
  Test(self, a(4) == 4)
  Test(self, a(5) == 5)

  a:clear()
  Test(self, a.size == 0)
  a:add(1)
  Test(self, a.size == 1)
  Test(self, a(0) == 1)
end

terra TestHarness:backend()

end

terra TestHarness:color()
  var c : F.Color = F.Color{ 0 }
  Test(self, c.r == 0)
  Test(self, c.g == 0)
  Test(self, c.b == 0)
  Test(self, c.a == 0)
  Test(self, c.v == 0)

  c.r = 0xFF
  Test(self, c.r == 0xFF)
  Test(self, c.g == 0)
  Test(self, c.b == 0)
  Test(self, c.a == 0)
  Test(self, c.v == 0x00FF0000)

  c.g = 0xFF
  c.b = 0xFF
  c.a = 0xFF
  Test(self, c.r == 0xFF)
  Test(self, c.g == 0xFF)
  Test(self, c.b == 0xFF)
  Test(self, c.a == 0xFF)
  Test(self, c.v == 0xFFFFFFFF)

  c.r = 0x34
  c.g = 0x56
  c.b = 0x78
  c.a = 0x12
  Test(self, c.r == 0x34)
  Test(self, c.g == 0x56)
  Test(self, c.b == 0x78)
  Test(self, c.a == 0x12)
  Test(self, c.v == 0x12345678)
  
  var c2 = F.Color { 0x12345678 }
  Test(self, c2.r == 0x34)
  Test(self, c2.g == 0x56)
  Test(self, c2.b == 0x78)
  Test(self, c2.a == 0x12)
  Test(self, c2.v == 0x12345678)
end

local TestEnum = Enum{
  "First",
  "second",
  "THIRD",
}

terra TestHarness:enum()
  Test(self, TestEnum.First.v == 0)
  Test(self, TestEnum.second.v == 1)
  Test(self, TestEnum.THIRD.v == 2)

  var e : TestEnum = TestEnum.First
  Test(self, e.v == 0)
  e.v = 0
  e = TestEnum.second
  e.v = 1
  Test(self, e.v == 1)
  e = TestEnum.THIRD
  Test(self, e.v == 2)
  e.v = 2
  Test(self, e == TestEnum.THIRD)
end

local TestFlags = Flags{
  "First",
  "second",
  "THIRD",
}

terra TestHarness:flags()
  Test(self, TestFlags.First.val == 1)
  Test(self, TestFlags.second.val == 2)
  Test(self, TestFlags.THIRD.val == 4)
  var f = TestFlags.First + TestFlags.THIRD
  Test(self, not (f.First and f.second))
  Test(self, f.First)
  Test(self, not f.second)
  Test(self, f.THIRD)
  f.THIRD = false
  f.second = true
  Test(self, f.First and f.second)
  Test(self, f.First and f.second and not f.THIRD)
end

terra TestHarness:message()

end

terra TestHarness:rect()
  var a = F.Rect{array(1f,2f,3f,4f)}
  Test(self, a.l == 1)
  Test(self, a.t == 2)
  Test(self, a.r == 3)
  Test(self, a.b == 4)
  Test(self, a.left == 1)
  Test(self, a.top == 2)
  Test(self, a.right == 3)
  Test(self, a.bottom == 4)
  Test(self, a.data[0] == 1)
  Test(self, a.data[1] == 2)
  Test(self, a.data[2] == 3)
  Test(self, a.data[3] == 4)

  var b = F.Rect{array(5f,6f,7f,8f)}
  var c = a + b
  Test(self, c.l == 6f)
  Test(self, c.t == 8f)
  Test(self, c.r == 10f)
  Test(self, c.b == 12f)
  c = b - a
  Test(self, c.l == 4f)
  Test(self, c.t == 4f)
  Test(self, c.r == 4f)
  Test(self, c.b == 4f)
  c = a - b
  Test(self, c.l == -4f)
  Test(self, c.t == -4f)
  Test(self, c.r == -4f)
  Test(self, c.b == -4f)
  c = b * a
  Test(self, c.l == 5f)
  Test(self, c.t == 12f)
  Test(self, c.r == 21f)
  Test(self, c.b == 32f)
  c = b / a
  Test(self, c.l == 5f)
  Test(self, c.t == 3f)
  Test(self, c.r == (7f/3f))
  Test(self, c.b == 2f)

  c = a + 1f
  Test(self, c.l == 2f)
  Test(self, c.t == 3f)
  Test(self, c.r == 4f)
  Test(self, c.b == 5f)
  c = 1f + a
  Test(self, c.l == 2f)
  Test(self, c.t == 3f)
  Test(self, c.r == 4f)
  Test(self, c.b == 5f)
  c = 2f * a
  Test(self, c.l == 2f)
  Test(self, c.t == 4f)
  Test(self, c.r == 6f)
  Test(self, c.b == 8f)

  var d = a.topleft + b.bottomright
  Test(self, d.x == 8)
  Test(self, d.y == 10)

  Test(self, a ~= b)
  Test(self, a == a)
  Test(self, not (a == b))
end

terra TestHarness:vec()
  var a = F.Veci{array(1,2)}
  Test(self, a.x == 1)
  Test(self, a.y == 2)
  Test(self, a.v[0] == 1)
  Test(self, a.v[1] == 2)
  a.x = 5
  a.y = 6
  Test(self, a.x == 5)
  Test(self, a.y == 6)

  var b = F.Veci{array(-3,-4)}
  var c = a + b
  Test(self, c.x == 2)
  Test(self, c.y == 2)
  c = a - b
  Test(self, c.x == 8)
  Test(self, c.y == 10)
  c = a * b
  Test(self, c.x == -15)
  Test(self, c.y == -24)
  a.x = 1
  a.y = 2
  c = b / a
  Test(self, c.x == -3)
  Test(self, c.y == -2)

  c = a + 3
  Test(self, c.x == 4)
  Test(self, c.y == 5)

  c = 4 - b
  Test(self, c.x == 7)
  Test(self, c.y == 8)

  Test(self, a ~= b)
  b.x = a.x
  b.y = a.y
  Test(self, a == b)
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
      emit(quote
        harness.passed, harness.total = 0, 0
        harness:[k]()
        var buf : int8[SUBTESTLEN + 1]
        C.snprintf(buf, [SUBTESTLEN + 1], "%u/%u", harness.passed, harness.total);
        PrintColumns(k, buf, terralib.select(harness.passed == harness.total, "PASS", "FAIL"))
      end)
    end
  end

  return 0;
end

return T
