// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: 2025 Fundament Software SPC <https://fundament.software>

use feather_macro::*;
use feather_ui::color::sRGB;
use feather_ui::component::button::Button;
use feather_ui::component::region::Region;
use feather_ui::component::shape::{Shape, ShapeKind};
use feather_ui::component::text::Text;
use feather_ui::component::window::Window;
use feather_ui::component::{mouse_area, ChildOf};
use feather_ui::layout::fixed;
use feather_ui::persist::FnPersist;
use feather_ui::ultraviolet::Vec4;
use feather_ui::{
    gen_id, im, AbsRect, App, DRect, RelRect, Slot, SourceID, WrapEventEx, FILL_DRECT, ZERO_RECT,
};
use std::any::{Any, TypeId};
use std::f32;
use std::rc::Rc;
use std::sync::Arc;

#[cfg(target_os = "windows")]
use feather_ui::winit::platform::windows::WindowAttributesExtWindows;

uniffi::include_scaffolding!("calculator");

pub trait Calculator: Send + Sync {
    fn add_digit(&self, digit: u8);
    fn backspace(&self);
    fn apply_op(&self);
    fn set_op(&self, op: CalcOp);
    fn get(&self) -> f64;
    fn toggle_decimal(&self);
    fn copy(&self) -> Arc<dyn Calculator>;
    fn eq(&self, rhs: Arc<dyn Calculator>) -> bool;
}

#[derive(Default, Empty, Area, Anchor, ZIndex)]
struct FixedData {
    area: DRect,
    anchor: feather_ui::DPoint,
    zindex: i32,
}

impl feather_ui::layout::base::Limits for FixedData {}
impl feather_ui::layout::base::RLimits for FixedData {}
impl fixed::Prop for FixedData {}
impl fixed::Child for FixedData {}

impl dyn Calculator {
    #[inline]
    pub fn is<T: Any>(&self) -> bool {
        // Get `TypeId` of the type this function is instantiated with.
        let t = TypeId::of::<T>();

        // Get `TypeId` of the type in the trait object (`self`).
        let concrete = self.type_id();

        // Compare both `TypeId`s on equality.
        t == concrete
    }

    /// # Safety
    /// caller guarantees that T is the correct type
    #[inline]
    pub unsafe fn downcast_ref_unchecked<T: Any>(&self) -> &T {
        debug_assert!(self.is::<T>());
        // SAFETY: caller guarantees that T is the correct type
        unsafe { &*(self as *const dyn Calculator as *const T) }
    }

    #[inline]
    pub fn downcast_ref<T: Any>(&self) -> Option<&T> {
        if self.is::<T>() {
            // SAFETY: just checked whether we are pointing to the correct type, and we can rely on
            // that check for memory safety because we have implemented Any for all types; no other
            // impls can exist as they would conflict with our impl.
            unsafe { Some(self.downcast_ref_unchecked()) }
        } else {
            None
        }
    }
}

// Doesn't include instant actions that take no argument, like clear, backspace, pi, square, root, etc.
#[derive(Debug, Copy, Clone, PartialEq, Eq, Default)]
pub enum CalcOp {
    #[default]
    None,
    Add,
    Sub,
    Mul,
    Div,
    Mod,
    Pow,
    Square,
    Sqrt,
    Inv,
    Negate,
    Clear,
}

struct CalcFFI(Arc<dyn Calculator>);

impl PartialEq for CalcFFI {
    fn eq(&self, other: &Self) -> bool {
        self.0.eq(other.0.clone())
    }
}

impl Clone for CalcFFI {
    fn clone(&self) -> Self {
        CalcFFI(self.0.copy())
    }
}

struct CalcApp {
    init_calc: Arc<dyn Calculator>,
}

const CLEAR_COLOR: sRGB = sRGB::new(0.8, 0.3, 0.3, 1.0);
const OP_COLOR: sRGB = sRGB::new(0.3, 0.3, 0.3, 0.7);
const NUM_COLOR: sRGB = sRGB::new(0.3, 0.3, 0.3, 1.0);
const EQ_COLOR: sRGB = sRGB::new(0.3, 0.8, 0.3, 1.0);

use std::sync::LazyLock;
type BoxedAction = Box<dyn Sync + Send + Fn(&Arc<dyn Calculator>)>;
static BUTTONS: LazyLock<[(&str, BoxedAction, sRGB); 24]> = LazyLock::new(|| {
    [
        (
            "C",
            Box::new(|calc| calc.set_op(CalcOp::Clear)),
            CLEAR_COLOR,
        ),
        ("x²", Box::new(|calc| calc.set_op(CalcOp::Square)), OP_COLOR),
        ("√x", Box::new(|calc| calc.set_op(CalcOp::Sqrt)), OP_COLOR),
        ("←", Box::new(|calc| calc.backspace()), OP_COLOR),
        ("yˣ", Box::new(|calc| calc.set_op(CalcOp::Pow)), OP_COLOR),
        ("¹∕ₓ", Box::new(|calc| calc.set_op(CalcOp::Inv)), OP_COLOR),
        ("%", Box::new(|calc| calc.set_op(CalcOp::Mod)), OP_COLOR),
        ("÷", Box::new(|calc| calc.set_op(CalcOp::Div)), OP_COLOR),
        ("7", Box::new(|calc| calc.add_digit(7)), NUM_COLOR),
        ("8", Box::new(|calc| calc.add_digit(8)), NUM_COLOR),
        ("9", Box::new(|calc| calc.add_digit(9)), NUM_COLOR),
        ("×", Box::new(|calc| calc.set_op(CalcOp::Mul)), OP_COLOR),
        ("4", Box::new(|calc| calc.add_digit(4)), NUM_COLOR),
        ("5", Box::new(|calc| calc.add_digit(5)), NUM_COLOR),
        ("6", Box::new(|calc| calc.add_digit(6)), NUM_COLOR),
        ("−", Box::new(|calc| calc.set_op(CalcOp::Sub)), OP_COLOR),
        ("1", Box::new(|calc| calc.add_digit(1)), NUM_COLOR),
        ("2", Box::new(|calc| calc.add_digit(2)), NUM_COLOR),
        ("3", Box::new(|calc| calc.add_digit(3)), NUM_COLOR),
        ("+", Box::new(|calc| calc.set_op(CalcOp::Add)), OP_COLOR),
        (
            "⁺∕₋",
            Box::new(|calc| calc.set_op(CalcOp::Negate)),
            OP_COLOR,
        ),
        ("0", Box::new(|calc| calc.add_digit(0)), NUM_COLOR),
        (".", Box::new(|calc| calc.toggle_decimal()), OP_COLOR),
        ("=", Box::new(|calc| calc.apply_op()), EQ_COLOR),
    ]
});

impl FnPersist<CalcFFI, im::HashMap<Arc<SourceID>, Option<Window>>> for CalcApp {
    type Store = (CalcFFI, im::HashMap<Arc<SourceID>, Option<Window>>);

    fn init(&self) -> Self::Store {
        (CalcFFI(self.init_calc.clone()), im::HashMap::new())
    }
    fn call(
        &mut self,
        mut store: Self::Store,
        args: &CalcFFI,
    ) -> (Self::Store, im::HashMap<Arc<SourceID>, Option<Window>>) {
        //if store.0.eq(args) {
        let mut children: im::Vector<Option<Box<ChildOf<dyn fixed::Prop>>>> = im::Vector::new();

        for (i, (txt, _, color)) in BUTTONS.iter().enumerate() {
            let rect = Shape::<DRect, { ShapeKind::RoundRect as u8 }>::new(
                gen_id!(),
                FILL_DRECT.into(),
                0.0,
                0.0,
                Vec4::broadcast(10.0),
                *color,
                sRGB::transparent(),
            );

            let text = Text::<DRect> {
                id: gen_id!(),
                props: FILL_DRECT.into(),
                text: txt.to_string(),
                font_size: 40.0,
                line_height: 56.0,
                ..Default::default()
            };

            const ROW_COUNT: usize = 4;
            let (x, y) = (i % ROW_COUNT, (i / ROW_COUNT) + 1);
            let (w, h) = (1.0 / ROW_COUNT as f32, 1.0 / 7.0);

            let btn = Button::<FixedData>::new(
                gen_id!(gen_id!(), i),
                FixedData {
                    area: feather_ui::URect {
                        abs: AbsRect::new(4.0, 4.0, -4.0, -4.0),
                        rel: RelRect::new(
                            w * x as f32,
                            h * y as f32,
                            w * (x + 1) as f32,
                            h * (y + 1) as f32,
                        ),
                    }
                    .into(),
                    anchor: Default::default(),
                    zindex: 0,
                },
                Slot(feather_ui::APP_SOURCE_ID.into(), i as u64),
                feather_ui::children![fixed::Prop, rect, text],
            );

            children.push_back(Some(Box::new(btn)));
        }

        let display = Text::<DRect> {
            id: gen_id!(),
            props: FILL_DRECT.into(),
            text: args.0.get().to_string(),
            font_size: 60.0,
            line_height: 42.0,
            ..Default::default()
        };

        let text_bg = Shape::<DRect, { ShapeKind::RoundRect as u8 }>::new(
            gen_id!(),
            Rc::new(DRect {
                px: ZERO_RECT,
                dp: ZERO_RECT,
                rel: RelRect::new(0.0, 0.0, 1.0, 1.0 / 7.0),
            }),
            0.0,
            0.0,
            Vec4::broadcast(25.0),
            sRGB::new(0.2, 0.2, 0.2, 1.0),
            Default::default(),
        );

        children.push_back(Some(Box::new(text_bg)));
        children.push_back(Some(Box::new(display)));

        let region = Region::<FixedData>::new(
            gen_id!(),
            FixedData {
                area: FILL_DRECT,
                ..Default::default()
            }
            .into(),
            children,
        );

        #[cfg(target_os = "windows")]
        let window = Window::new(
            gen_id!(),
            feather_ui::winit::window::Window::default_attributes()
                .with_title(env!("CARGO_CRATE_NAME"))
                .with_drag_and_drop(false)
                .with_resizable(true),
            Box::new(region),
        );
        #[cfg(not(target_os = "windows"))]
        let window = Window::new(
            gen_id!(),
            feather_ui::winit::window::Window::default_attributes()
                .with_title(env!("CARGO_CRATE_NAME"))
                .with_resizable(true),
            Box::new(region),
        );

        store.1 = im::HashMap::new();
        store.1.insert(window.id.clone(), Some(window));
        store.0 = args.clone();
        //}
        let windows = store.1.clone();
        (store, windows)
    }
}

pub fn register(calc: ::std::sync::Arc<dyn Calculator>) {
    #[allow(clippy::type_complexity)]
    let mut inputs: Vec<
        Box<dyn FnMut((u64, Box<dyn Any>), CalcFFI) -> Result<CalcFFI, CalcFFI>>,
    > = Vec::new();

    for (_, f, _) in BUTTONS.iter() {
        inputs.push(Box::new(
            |_: mouse_area::MouseAreaEvent, appdata: CalcFFI| -> Result<CalcFFI, CalcFFI> {
                {
                    f(&appdata.0);
                    Ok(appdata)
                }
            }
            .wrap(),
        ));
    }

    let init_calc = calc.copy();
    let (mut app, event_loop): (
        App<CalcFFI, CalcApp>,
        feather_ui::winit::event_loop::EventLoop<()>,
    ) = App::new(CalcFFI(calc), inputs, CalcApp { init_calc }, |_| ()).unwrap();

    event_loop.run_app(&mut app).unwrap();
}
