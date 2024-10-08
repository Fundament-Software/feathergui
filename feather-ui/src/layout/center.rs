use super::Concrete;
use super::Desc;
use super::EventList;
use super::Layout;
use super::Renderable;
use super::Staged;
use crate::rtree;
use crate::AbsRect;
use crate::UPoint;
use crate::URect;
use dyn_clone::DynClone;
use std::rc::Rc;

#[derive(Clone, Default)]
pub struct Inherited {
    pub center: UPoint,
}

#[derive(Clone, Default)]
pub struct Center {
    pub padding: URect,
    pub zindex: i32,
}

impl<AppData: 'static> Desc<AppData> for Center {
    type Props = Center;
    type Impose = Inherited;
    type Children<A: DynClone + ?Sized> =
        im::Vector<Option<Box<dyn Layout<Self::Impose, AppData>>>>;

    fn stage<'a>(
        props: &Self::Props,
        area: AbsRect,
        children: &Self::Children<dyn Layout<Self::Impose, AppData>>,
        events: Option<Rc<EventList<AppData>>>,
        renderable: Option<Rc<dyn Renderable<AppData>>>,
        driver: &crate::DriverState,
    ) -> Box<dyn Staged<AppData> + 'a>
    where
        AppData: 'a,
    {
        let padding = props.padding * area;
        let area = AbsRect {
            topleft: area.topleft + padding.topleft,
            bottomright: (area.bottomright - padding.bottomright).into(),
        };

        let mut inspect: im::Vector<(
            &Box<dyn Layout<Self::Impose, AppData>>,
            Box<dyn Staged<AppData>>,
        )> = im::Vector::new();

        // First we let the children calculate their own size using our entire padded area
        for child in children.iter() {
            inspect.push_back((
                child.as_ref().unwrap(),
                child.as_ref().unwrap().stage(area, driver),
            ));
        }

        let mut nodes: im::Vector<Option<Rc<rtree::Node<AppData>>>> = im::Vector::new();
        let mut staging: im::Vector<Option<Box<dyn Staged<AppData>>>> = im::Vector::new();

        // Now we check to see if the child ended up centered or if we need to update again
        for child in inspect.into_iter() {
            let props = child.0.get_imposed();
            let target = props.center * area;
            let childarea = child.1.get_area();
            let center = childarea.topleft + ((childarea.bottomright - childarea.topleft) / 2.0f32);
            let diff = center - target;
            if diff.x < std::f32::EPSILON && diff.y < std::f32::EPSILON {
                if let Some(node) = child.1.get_rtree().upgrade() {
                    nodes.push_back(Some(node));
                }
                staging.push_back(Some(child.1));
            } else {
                let targetarea = childarea + diff;
                let stage = child.0.stage(targetarea, driver);
                if let Some(node) = stage.get_rtree().upgrade() {
                    nodes.push_back(Some(node));
                }
                staging.push_back(Some(stage));
            }
        }

        Box::new(Concrete {
            area,
            render: renderable
                .map(|x| x.render(area, driver))
                .unwrap_or_default(),
            rtree: Rc::new(rtree::Node::new(area, Some(props.zindex), nodes, events)),
            children: staging,
        })
    }
}
