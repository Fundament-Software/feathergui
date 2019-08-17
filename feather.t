terralib.vshome = [[C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC\14.22.27905\]]
local sdkroot = [[C:\Program Files (x86)\Windows Kits\10\]]
local sdkversion = [[10.0.18362.0]]

function terralib.getvclinker()
  local linker = terralib.vshome..[[bin\Hostx86\x64\link.exe]]
  local vclib = terralib.vshome..[[lib\x64;]]..sdkroot..[[Lib\]]..sdkversion..[[\um\x64\;]]..sdkroot..[[Lib\]]..sdkversion..[[\ucrt\x64\;]]
  local vcpath = terralib.vcpath or (os.getenv("Path") or "")..";"..terralib.vshome..[[bin\Hostx86\x64\;]]
  vclib,vcpath = "LIB="..vclib,"Path="..vcpath
  return linker,vclib,vcpath
end

terralib.includepath =  terralib.vshome .. [[include;]] .. sdkroot .. [[Include\]]..sdkversion..[[\ucrt;]] .. sdkroot .. [[Include\]]..sdkversion..[[\shared;]] .. sdkroot .. [[Include\]]..sdkversion..[[\um;]]

local Backend = require 'feather.backend'
local Test = require 'feather.test'

local flags = {"\\legacy_stdio_definitions.lib"}
terralib.saveobj("feather.exe", {main = Test.main}, flags)
Test.main(0, nil)