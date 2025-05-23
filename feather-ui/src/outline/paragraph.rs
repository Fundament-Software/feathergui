// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: 2025 Fundament Software SPC <https://fundament.software>

use crate::layout::{base, flex, leaf, Desc, Layout, LayoutWrap};
use crate::outline::text::Text;
use crate::outline::OutlineFrom;
use crate::persist::{FnPersist, VectorMap};
use crate::{gen_id, layout, SourceID, UNSIZED_AXIS};
use core::f32;
use derive_where::derive_where;
use std::rc::Rc;

#[derive_where(Clone)]
pub struct Paragraph<T: flex::Prop + 'static> {
    pub id: Rc<SourceID>,
    pub props: Rc<T>,
    pub children: im::Vector<Option<Box<OutlineFrom<dyn flex::Prop>>>>,
}

struct MinimalFlexChild {
    grow: f32,
}

impl flex::Child for MinimalFlexChild {
    fn grow(&self) -> f32 {
        self.grow
    }

    fn shrink(&self) -> f32 {
        0.0
    }

    fn basis(&self) -> crate::DValue {
        UNSIZED_AXIS.into()
    }
}

impl base::Area for MinimalFlexChild {
    fn area(&self) -> &crate::DRect {
        &crate::AUTO_DRECT
    }
}

impl base::Anchor for MinimalFlexChild {}
impl base::Order for MinimalFlexChild {}
impl base::Margin for MinimalFlexChild {}
impl base::RLimits for MinimalFlexChild {}
impl base::Limits for MinimalFlexChild {}
impl base::Padding for MinimalFlexChild {}
impl leaf::Prop for MinimalFlexChild {}
impl leaf::Padded for MinimalFlexChild {}

impl<T: flex::Prop + 'static> Paragraph<T> {
    pub fn new(id: Rc<SourceID>, props: T) -> Self {
        Self {
            id,
            props: props.into(),
            children: im::Vector::new(),
        }
    }

    #[allow(clippy::too_many_arguments)]
    pub fn set_text(
        &mut self,
        text: &str,
        font_size: f32,
        line_height: f32,
        font: glyphon::FamilyOwned,
        color: glyphon::Color,
        weight: glyphon::Weight,
        style: glyphon::Style,
        fullwidth: bool,
    ) {
        self.children.clear();
        for word in text.split_ascii_whitespace() {
            let text = Text::<MinimalFlexChild> {
                id: gen_id!().into(),
                props: MinimalFlexChild {
                    grow: if fullwidth { 1.0 } else { 0.0 },
                }
                .into(),
                text: word.to_owned() + " ",
                font_size,
                line_height,
                font: font.clone(),
                color,
                weight,
                style,
                wrap: glyphon::Wrap::None,
            };
            self.children.push_back(Some(Box::new(text)));
        }
    }
}

impl<T: flex::Prop + 'static> super::Outline<T> for Paragraph<T> {
    fn id(&self) -> Rc<SourceID> {
        self.id.clone()
    }

    fn init_all(&self, manager: &mut crate::StateManager) -> eyre::Result<()> {
        for child in self.children.iter() {
            manager.init_outline(child.as_ref().unwrap().as_ref())?;
        }
        Ok(())
    }

    fn layout(
        &self,
        state: &crate::StateManager,
        driver: &crate::DriverState,
        dpi: crate::Vec2,
        config: &wgpu::SurfaceConfiguration,
    ) -> Box<dyn Layout<T>> {
        let map = VectorMap::new(
            |child: &Option<Box<OutlineFrom<dyn flex::Prop>>>| -> Option<Box<dyn LayoutWrap<<dyn flex::Prop as Desc>::Child>>> {
                Some(child.as_ref().unwrap().layout(state, driver, dpi, config))
            },
        );

        let (_, children) = map.call(Default::default(), &self.children);
        Box::new(layout::Node::<T, dyn flex::Prop> {
            props: self.props.clone(),
            children,
            id: Rc::downgrade(&self.id),
            renderable: None,
        })
    }
}

crate::gen_outline_wrap!(Paragraph, flex::Prop);
