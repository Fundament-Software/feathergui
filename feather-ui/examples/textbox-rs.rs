// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: 2025 Fundament Software SPC <https://fundament.software>

use feather_ui::color::sRGB;
use feather_ui::layout::{fixed, leaf};
use feather_ui::text::{EditBuffer, EditView};
use feather_ui::{DAbsRect, gen_id};

use feather_ui::component::region::Region;
use feather_ui::component::textbox;
use feather_ui::component::textbox::TextBox;
use feather_ui::component::window::Window;
use feather_ui::layout::base;
use feather_ui::persist::FnPersist;
use feather_ui::{AbsRect, App, DRect, FILL_DRECT, RelRect, SourceID, cosmic_text};
use std::sync::Arc;

#[derive(PartialEq, Clone, Debug, Default)]
struct TextState {
    text: EditView,
}

struct BasicApp {}

#[derive(Default, Clone, feather_macro::Empty, feather_macro::Area)]
struct MinimalArea {
    area: DRect,
}

impl base::ZIndex for MinimalArea {}
impl base::Anchor for MinimalArea {}
impl base::Limits for MinimalArea {}
impl fixed::Prop for MinimalArea {}

#[derive(
    Clone,
    feather_macro::Empty,
    feather_macro::Area,
    feather_macro::TextEdit,
    feather_macro::Padding,
)]
struct MinimalText {
    area: DRect,
    padding: DAbsRect,
    textedit: EditView,
}
impl base::Direction for MinimalText {}
impl base::ZIndex for MinimalText {}
impl base::Limits for MinimalText {}
impl base::RLimits for MinimalText {}
impl base::Anchor for MinimalText {}
impl leaf::Padded for MinimalText {}
impl leaf::Prop for MinimalText {}
impl fixed::Child for MinimalText {}
impl textbox::Prop for MinimalText {}

impl FnPersist<TextState, im::HashMap<Arc<SourceID>, Option<Window>>> for BasicApp {
    type Store = (TextState, im::HashMap<Arc<SourceID>, Option<Window>>);

    fn init(&self) -> Self::Store {
        (
            TextState {
                ..Default::default()
            },
            im::HashMap::new(),
        )
    }
    fn call(
        &mut self,
        mut store: Self::Store,
        args: &TextState,
    ) -> (Self::Store, im::HashMap<Arc<SourceID>, Option<Window>>) {
        if store.0 != *args {
            let textbox = TextBox::new(
                gen_id!(),
                MinimalText {
                    area: FILL_DRECT,
                    padding: AbsRect::broadcast(12.0).into(),
                    textedit: args.text.clone(), // Be careful to take the value from args, not store.0, which is stale.
                },
                40.0,
                56.0,
                cosmic_text::FamilyOwned::SansSerif,
                sRGB::white(),
                Default::default(),
                Default::default(),
                cosmic_text::Wrap::Word,
            );

            let region = Region::new(
                gen_id!(),
                MinimalArea {
                    area: feather_ui::URect {
                        abs: AbsRect::new(90.0, 0.0, -90.0, -180.0),
                        rel: RelRect::new(0.0, 0.0, 1.0, 1.0),
                    }
                    .into(),
                }
                .into(),
                feather_ui::children![fixed::Prop, textbox],
            );
            let window = Window::new(
                gen_id!(),
                winit::window::Window::default_attributes()
                    .with_title(env!("CARGO_CRATE_NAME"))
                    .with_inner_size(winit::dpi::PhysicalSize::new(600, 400))
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

fn main() {
    let (mut app, event_loop): (App<TextState, BasicApp>, winit::event_loop::EventLoop<()>) =
        App::new(
            TextState {
                text: EditBuffer::new("new text", (0, 0)).into(),
            },
            vec![],
            BasicApp {},
            |_| (),
        )
        .unwrap();

    event_loop.run_app(&mut app).unwrap();
}
