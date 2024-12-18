use feather_ui::gen_id;
use feather_ui::layout::basic;
use feather_ui::layout::basic::Basic;
use feather_ui::layout::root;
use feather_ui::outline::button;
use feather_ui::outline::button::Button;
use feather_ui::outline::mouse_area;
use feather_ui::outline::region::Region;
use feather_ui::outline::round_rect::RoundRect;
use feather_ui::outline::text::Text;
use feather_ui::outline::window::Window;
use feather_ui::outline::OutlineFrom;
use feather_ui::persist::FnPersist;
use feather_ui::AbsRect;
use feather_ui::App;
use feather_ui::Slot;
use feather_ui::SourceID;
use std::rc::Rc;
use ultraviolet::Vec4;

#[derive(PartialEq, Clone, Debug)]
struct CounterState {
    count: i32,
}

fn counter_increment(st: CounterState) -> CounterState {
    CounterState {
        count: st.count + 1,
    }
}

fn counter_decrement(st: CounterState) -> CounterState {
    CounterState {
        count: st.count - 1,
    }
}

struct BasicApp {}

impl FnPersist<CounterState, im::HashMap<Rc<SourceID>, Option<Window>>> for BasicApp {
    type Store = (CounterState, im::HashMap<Rc<SourceID>, Option<Window>>);

    fn init(&self) -> Self::Store {
        (CounterState { count: -1 }, im::HashMap::new())
    }
    fn call(
        &self,
        mut store: Self::Store,
        args: &CounterState,
    ) -> (Self::Store, im::HashMap<Rc<SourceID>, Option<Window>>) {
        if store.0 != *args {
            let button = {
                let rect = RoundRect::<basic::Inherited> {
                    id: gen_id!().into(),
                    fill: Vec4::new(0.2, 0.7, 0.4, 1.0),
                    corners: Vec4::broadcast(10.0),
                    props: basic::Inherited {
                        area: feather_ui::FILL_URECT,
                        margin: Default::default(),
                    },
                    ..Default::default()
                };

                let text = Text::<basic::Inherited> {
                    id: gen_id!().into(),
                    props: basic::Inherited {
                        area: feather_ui::FILL_URECT,
                        margin: Default::default(),
                    },
                    text: format!("Clicks: {}", args.count),
                    font_size: 30.0,
                    line_height: 42.0,
                    ..Default::default()
                };

                let mut children: im::Vector<Option<Box<OutlineFrom<Basic>>>> = im::Vector::new();
                children.push_back(Some(Box::new(text)));
                children.push_back(Some(Box::new(rect)));

                Button::<basic::Inherited>::new(
                    gen_id!().into(),
                    basic::Inherited {
                        area: feather_ui::FILL_URECT,
                        margin: Default::default(),
                    },
                    Default::default(),
                    Slot(feather_ui::APP_SOURCE_ID.into(), 0),
                    children,
                )
            };

            let mut children: im::Vector<Option<Box<OutlineFrom<Basic>>>> = im::Vector::new();
            children.push_back(Some(Box::new(button)));

            let region = Region {
                id: gen_id!().into(),
                props: root::Inherited {
                    area: AbsRect::new(90.0, 90.0, 400.0, 200.0).into(),
                },
                basic: Basic {
                    padding: Default::default(),
                    zindex: 0,
                },
                children,
            };
            let window = Window::new(
                gen_id!().into(),
                winit::window::Window::default_attributes()
                    .with_title("basic-rs")
                    .with_resizable(true),
                Box::new(region),
            );

            store.1 = im::HashMap::new();
            store.1.insert(window.id.clone(), Some(window));
            store.0 = args.clone();
        }
        let windows = store.1.clone();
        (store, windows)
    }
}

use feather_ui::WrapEventEx;

fn main() {
    let onclick = Box::new(
        |_: mouse_area::MouseAreaEvent,
         mut appdata: CounterState|
         -> Result<CounterState, CounterState> {
            {
                appdata.count += 1;
                Ok(appdata)
            }
        }
        .wrap(),
    );

    let (mut app, event_loop): (
        App<CounterState, BasicApp>,
        winit::event_loop::EventLoop<()>,
    ) = App::new(CounterState { count: 0 }, vec![onclick], BasicApp {}).unwrap();

    event_loop.run_app(&mut app).unwrap();
}
