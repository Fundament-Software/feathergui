local Backend = require 'feather.backend'
local Test = require 'feather.test'

local flags = {"\\legacy_stdio_definitions.lib"}
terralib.saveobj("feather.exe", {main = Test.main}, flags)
Test.main(0, nil)