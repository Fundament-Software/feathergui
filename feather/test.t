local C = terralib.includecstring [[
  #include <stdio.h>
  #include <stdlib.h>
]]

local Array = require 'feather.array'
require 'feather.backend'
local Color = require 'feather.color'
local Enum = require 'feather.enum'
local Flags = require 'feather.flags'
local Message = require 'feather.message'
require 'feather.os'
require 'feather.rect'
require 'feather.vec'

struct TestHarness {
  passed : int;
  total : int;
}

terra Test(self : &TestHarness, exp : bool)
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
  var c : fgColor = fgColor{ 0 }
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
  
  var c2 = fgColor { 0x12345678 }
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
  var f = TestFlags.First + TestFlags.THIRD
  Test(self, not (f.First and f.second))
  f.THIRD = false
  f.second = true
  Test(self, f.First and f.second)
end

terra TestHarness:message()

end

terra TestHarness:rect()

end

terra TestHarness:vec()
  var a = fgVeci{array(1,2)}
  Test(self, a.x == 1)
  Test(self, a.y == 2)
  Test(self, a.v[0] == 1)
  Test(self, a.v[1] == 2)
  a.x = 5
  a.y = 6
  Test(self, a.x == 5)
  Test(self, a.y == 6)

  var b = fgVeci{array(-3,-4)}
  var c = a + b
  Test(self, c.x == 2)
  Test(self, c.y == 2)
  c = a - b
  Test(self, c.x == 8)
  Test(self, c.y == 10)
  c = a * b
  Test(self, c.x == -3)
  Test(self, c.y == -8)
  c = b / a
  Test(self, c.x == -3)
  Test(self, c.y == -2)

  Test(self, a ~= b)
  b.x = a.x
  b.y = a.y
  Test(self, a == b)
end

local SUBTESTLEN = 11

terra PrintColumns(a : rawstring, b : rawstring, c : rawstring) : {}
  C.printf("%-*s %-*s %-*s\n", 24, a, [SUBTESTLEN], b, 8, c);
end

terra main(argc : int, argv : &rawstring) : int
  C.printf("Feather v%i.%i.%i Test Utility\n\n", [fgVersion[1]], [fgVersion[2]], [fgVersion[3]])

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

