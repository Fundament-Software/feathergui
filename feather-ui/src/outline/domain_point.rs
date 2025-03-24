// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: 2025 Fundament Software SPC <https://fundament.software>

use crate::layout::domain_write;
use crate::SourceID;
use derive_where::derive_where;
use std::rc::Rc;

// This simply writes it's area to the given cross-reference domain during the layout phase
#[derive_where(Clone)]
pub struct DomainPoint<T: domain_write::Prop + 'static> {
    pub id: Rc<SourceID>,
    props: Rc<T>,
}

impl<T: domain_write::Prop + 'static> DomainPoint<T> {
    pub fn new(id: Rc<SourceID>, props: T) -> Self {
        Self {
            id,
            props: props.into(),
        }
    }
}

impl<T: domain_write::Prop + 'static> super::Outline<T> for DomainPoint<T>
where
    for<'a> &'a T: Into<&'a (dyn domain_write::Prop + 'static)>,
{
    fn id(&self) -> Rc<SourceID> {
        self.id.clone()
    }

    fn init_all(&self, _: &mut crate::StateManager) -> eyre::Result<()> {
        Ok(())
    }

    fn layout(
        &self,
        _: &crate::StateManager,
        _: &crate::DriverState,
        _: &wgpu::SurfaceConfiguration,
    ) -> Box<dyn crate::layout::Layout<T>> {
        Box::new(crate::layout::Node::<T, dyn domain_write::Prop> {
            props: self.props.clone(),
            children: Default::default(),
            id: Rc::downgrade(&self.id),
            renderable: None,
        })
    }
}
