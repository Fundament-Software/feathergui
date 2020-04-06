local Backend = require 'feather.backend'
local Test = require 'feather.test'

if terralib.saveobj("feather.exe", {main = Test.main}) ~= nil then
  return -1
end
Test.main(0, nil)
return 0