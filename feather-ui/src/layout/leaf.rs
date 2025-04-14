// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: 2025 Fundament Software SPC <https://fundament.software>

use super::base;
use super::base::Empty;
use super::Concrete;
use super::Desc;
use super::LayoutWrap;
use super::Renderable;
use super::Staged;
use crate::rtree;
use crate::AbsRect;
use crate::SourceID;
use crate::URect;
use std::marker::PhantomData;
use std::rc::Rc;

pub trait Prop: base::Area {}

crate::gen_from_to_dyn!(Prop);

impl Prop for URect {}

impl Desc for dyn Prop {
    type Props = dyn Prop;
    type Child = dyn Empty;
    type Children = PhantomData<dyn LayoutWrap<Self::Child>>;

    fn stage<'a>(
        props: &Self::Props,
        mut outer_area: AbsRect,
        _: &Self::Children,
        id: std::rc::Weak<SourceID>,
        renderable: Option<Rc<dyn Renderable>>,
        _: &crate::DriverState,
    ) -> Box<dyn Staged + 'a> {
        if outer_area.bottomright.x.is_infinite() {
            outer_area.bottomright.x = outer_area.topleft.x;
        }
        if outer_area.bottomright.y.is_infinite() {
            outer_area.bottomright.y = outer_area.topleft.y;
        }

        let myarea = *props.area() * outer_area;

        Box::new(Concrete {
            area: myarea,
            render: renderable,
            rtree: Rc::new(rtree::Node::new(myarea, None, Default::default(), id)),
            children: Default::default(),
        })
    }
}
