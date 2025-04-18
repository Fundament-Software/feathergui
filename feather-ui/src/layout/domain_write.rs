// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: 2025 Fundament Software SPC <https://fundament.software>

use super::base::Empty;
use super::Concrete;
use super::Desc;
use super::LayoutWrap;
use super::Renderable;
use super::Staged;
use crate::outline::CrossReferenceDomain;
use crate::rtree;
use crate::AbsRect;
use crate::SourceID;
use std::marker::PhantomData;
use std::rc::Rc;

// A DomainWrite layout spawns a renderable that writes it's area to the target cross-reference domain
pub trait Prop {
    fn domain(&self) -> Rc<CrossReferenceDomain>;
}

crate::gen_from_to_dyn!(Prop);

impl Prop for Rc<CrossReferenceDomain> {
    fn domain(&self) -> Rc<CrossReferenceDomain> {
        self.clone()
    }
}

impl Empty for Rc<CrossReferenceDomain> {}

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

        Box::new(Concrete {
            area: outer_area,
            render: Some(Rc::new(DomainWrite {
                id: id.clone(),
                domain: props.domain().clone(),
                base: renderable,
            })),
            rtree: Rc::new(rtree::Node::new(outer_area, None, Default::default(), id)),
            children: Default::default(),
        })
    }
}

struct DomainWrite {
    pub(crate) id: std::rc::Weak<SourceID>,
    pub(crate) domain: Rc<CrossReferenceDomain>,
    pub(crate) base: Option<Rc<dyn Renderable>>,
}

impl Renderable for DomainWrite {
    fn render(
        &self,
        area: AbsRect,
        driver: &crate::DriverState,
    ) -> im::Vector<crate::RenderInstruction> {
        if let Some(idref) = self.id.upgrade() {
            self.domain.write_area(idref, area);
        }

        self.base
            .as_ref()
            .map(|x| x.render(area, driver))
            .unwrap_or_default()
    }
}
