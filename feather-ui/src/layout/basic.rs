use super::Concrete;
use super::Desc;
use super::Layout;
use super::Renderable;
use super::Staged;
use crate::rtree;
use crate::AbsRect;
use crate::URect;
use crate::Vec2;
use dyn_clone::DynClone;
use std::rc::Rc;

#[derive(Clone, Default)]
pub struct Inherited {
    pub margin: URect,
    pub area: URect,
}

#[derive(Clone, Default)]
pub struct Basic {
    pub padding: URect,
    pub zindex: i32,
}

impl Desc for Basic {
    type Props = Basic;
    type Impose = Inherited;
    type Children<A: DynClone + ?Sized> = im::Vector<Option<Box<dyn Layout<Self::Impose>>>>;

    fn stage<'a>(
        props: &Self::Props,
        true_area: AbsRect,
        parent_pos: Vec2,
        children: &Self::Children<dyn Layout<Self::Impose> + '_>,
        id: std::rc::Weak<crate::SourceID>,
        renderable: Option<Rc<dyn Renderable>>,
        driver: &crate::DriverState,
    ) -> Box<dyn Staged + 'a> {
        // Calculate area for children
        let inner_area = {
            let dim = crate::AbsDim(true_area.bottomright - true_area.topleft);
            AbsRect {
                topleft: true_area.topleft + (props.padding.topleft * dim),
                bottomright: (true_area.bottomright - (props.padding.bottomright * dim)).into(),
            }
        };

        let mut staging: im::Vector<Option<Box<dyn Staged>>> = im::Vector::new();
        let mut nodes: im::Vector<Option<Rc<rtree::Node>>> = im::Vector::new();

        for child in children.iter() {
            let props = child.as_ref().unwrap().get_imposed();
            let dim = crate::AbsDim(inner_area.bottomright - inner_area.topleft);
            let area = props.area * inner_area;
            let result = AbsRect {
                topleft: area.topleft + (props.margin.topleft * dim),
                bottomright: (area.bottomright - (props.margin.bottomright * dim)).into(),
            };

            let stage = child
                .as_ref()
                .unwrap()
                .stage(result, true_area.topleft, driver);
            if let Some(node) = stage.get_rtree().upgrade() {
                nodes.push_back(Some(node));
            }
            staging.push_back(Some(stage));
        }

        // Our area does not change based on child sizes, so we have no bottom-up resolution step to do here

        Box::new(Concrete {
            area: true_area - parent_pos,
            render: renderable
                .map(|x| x.render(true_area, driver))
                .unwrap_or_default(),
            rtree: Rc::new(rtree::Node::new(
                true_area - parent_pos,
                Some(props.zindex),
                nodes,
                id,
            )),
            children: staging,
        })
    }
}
