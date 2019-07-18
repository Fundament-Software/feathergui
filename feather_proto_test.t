terralib.nativetarget = terralib.newtarget {
   Triple = "x86_64-pc-linux-gnu"; -- LLVM target triple
   CPU = "x86-64";  -- LLVM CPU name,
                                           }

print(terralib.includepath)


local feather = require 'feather'

local test_ui = feather.ui {
  feather.window {
    title = "Feather Test!",
    feather.layout_row_dynamic {
      height = 30,
      --width = 60,
      feather.button {
        text = "Hello",
        on_click = ":inc"
      },
      feather.button {
        text = "Goodbye",
        on_click = ":dec"
      }
    },
    feather.layout_row_dynamic {
      --width = 160,
      height = 50,

      feather.text_wrap {
        text = "test text that is much longer and might overflow the box"
      }
    },
    feather.layout_row_dynamic {
      height = 50,
      feather.text_wrap {
        text = "Count: "
      },
      feather.text_wrap {
        bind_text = ".count"
      }
    }
  }
}

local struct count_data {
  count: int
                        }

local C = terralib.includec "stdio.h"

terra count_data:inc()
  C.printf("inc clicked\n")
  self.count = self.count + 1
end

terra count_data:dec()
  C.printf("dec clicked\n")
  self.count = self.count - 1
end

local bindings = feather.binding(&count_data)

local test_launch = feather.launchUI(test_ui, bindings)

terra main()
  var data: count_data
  data.count = 0
  test_launch(&data)
  C.printf("final count %d\n", data.count)
  return 0
end

--print(main())


terralib.saveobj("feather_test", {main = main}, {"nkc.o", "-L.", "-l", "X11", "-lm"})
