// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: 2025 Fundament Software SPC <https://fundament.software>

use crate::shaders::{Vertex, to_bytes};
use crate::{DriverState, RenderLambda};
use std::cell::RefCell;
use std::rc::Rc;
use ultraviolet::Vec2;
use wgpu::util::DeviceExt;

pub struct Pipeline {
    pub this: std::rc::Weak<Pipeline>,
    pub pipeline: wgpu::RenderPipeline,
    pub group: wgpu::BindGroup,
    pub pos: RefCell<(Vec2, Vec2)>,
    pub _buffers: [wgpu::Buffer; 3],
}

impl super::Renderable for Pipeline {
    fn render(
        &self,
        _: crate::AbsRect,
        driver: &DriverState,
    ) -> im::Vector<crate::RenderInstruction> {
        // We have to put this in an RC because it needs to be cloneable across multiple render passes
        let (start, end) = *self.pos.borrow();
        let verts = Rc::new(
            driver
                .device
                .create_buffer_init(&wgpu::util::BufferInitDescriptor {
                    label: Some("LineVertices"),
                    contents: to_bytes(&[Vertex { pos: start.into() }, Vertex { pos: end.into() }]),
                    usage: wgpu::BufferUsages::VERTEX,
                }),
        );

        let weak = self.this.clone();
        let mut result = im::Vector::new();
        result.push_back(Some(Box::new(move |pass: &mut wgpu::RenderPass| {
            if let Some(this) = weak.upgrade() {
                pass.set_vertex_buffer(0, verts.slice(..));
                pass.set_bind_group(0, &this.group, &[]);
                pass.set_pipeline(&this.pipeline);
                pass.draw(
                    //0..(this.vertices.size() as u32 / size_of::<Vertex>() as u32),
                    0..2,
                    0..1,
                );
            }
        }) as Box<dyn RenderLambda>));
        result
    }
}
