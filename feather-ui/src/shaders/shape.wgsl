// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: 2025 Fundament Software SPC <https://fundament.software>

//#import "feather.wgsl"
const UNITX = array(0.0, 1.0, 0.0, 1.0, 1.0, 0.0);
const UNITY = array(0.0, 0.0, 1.0, 0.0, 1.0, 1.0);

fn linearstep(low: f32, high: f32, x: f32) -> f32 {
  return clamp((x - low) / (high - low), 0.0f, 1.0f);
}

fn u32_to_vec4(c: u32) -> vec4<f32> {
  return vec4<f32>(f32((c & 0xff000000u) >> 24u) / 255.0, f32((c & 0x00ff0000u) >> 16u) / 255.0, f32((c & 0x0000ff00u) >> 8u) / 255.0, f32(c & 0x000000ffu) / 255.0);
}

fn srgb_to_linear(c: f32) -> f32 {
  if c <= 0.04045 {
    return c / 12.92;
  }
  else {
    return pow((c + 0.055) / 1.055, 2.4);
  }
}

fn srgb_to_linear_vec4(c: vec4<f32>) -> vec4<f32> {
  return vec4f(srgb_to_linear(c.x), srgb_to_linear(c.y), srgb_to_linear(c.z), c.w);
}

@group(0) @binding(0)
var<uniform> MVP: mat4x4f;
@group(0) @binding(1)
var<storage, read> buf: array<Data>;
@group(0) @binding(2)
var<uniform> extent: u32;

struct Data {
  corners: vec4f,
  pos: vec2f,
  dim: vec2f,
  border: f32,
  blur: f32,
  fill: u32,
  outline: u32,
}

struct VertexOutput {
  @invariant @builtin(position) position: vec4<f32>,
  @location(0) uv: vec2f,
  @location(1) @interpolate(flat) index: u32,
}

@vertex
fn vs_main(@builtin(vertex_index) idx: u32) -> VertexOutput {
  let vert = idx % 6;
  let index = idx / 6;
  var vpos = vec2(UNITX[vert], UNITY[vert]);
  let d = buf[index];

  var mv: mat4x4f;
  mv[0] = vec4f(d.dim.x, 0f, 0f, 0f);
  mv[1] = vec4f(0f, d.dim.y, 0f, 0f);
  mv[2] = vec4f(0f, 0f, 1f, 0f);
  mv[3] = vec4f(d.pos.x + d.dim.x * 0.5f, d.pos.y + d.dim.y * 0.5f, 0f, 1f);
  //let outpos = vec4f(d.pos.x, d.pos.y, d.dim.x, d.dim.y);
  let outpos = MVP * mv * vec4(vpos.x - 0.5f, vpos.y - 0.5f, 1f, 1f);

  return VertexOutput(outpos, vpos.xy, index);
}

fn rectangle_sdf(samplePosition: vec2f, halfSize: vec2f, edges: vec4f) -> f32 {
  var edge: f32 = 20.0f;
  if (samplePosition.x > 0.0f) {
    edge = select(edges.z, edges.y, samplePosition.y < 0.0f);
  }
  else {
    edge = select(edges.w, edges.x, samplePosition.y < 0.0f);
  }

  let componentWiseEdgeDistance = abs(samplePosition) - halfSize + vec2f(edge);
  let outsideDistance = length(max(componentWiseEdgeDistance, vec2f(0.0f)));
  let insideDistance = min(max(componentWiseEdgeDistance.x, componentWiseEdgeDistance.y), 0.0f);
  return outsideDistance + insideDistance - edge;
}

@fragment
fn rectangle(input: VertexOutput) -> @location(0) vec4f {
  let d = buf[input.index];
  // Ideally we would get DPI for both height and width, but for now we just assume DPI isn't weird
  let w = fwidth(d.dim.x * input.uv.x) * 0.5f * (1.0f + d.blur);
  let uv = (input.uv * d.dim) - (d.dim * 0.5f);

  let dist = rectangle_sdf(uv, d.dim * 0.5f, d.corners);
  let alpha = linearstep(w, - w, dist);
  let s = linearstep(w, - w, dist + d.border);
  let fill = srgb_to_linear_vec4(u32_to_vec4(d.fill));
  let outline = srgb_to_linear_vec4(u32_to_vec4(d.outline));

  return (vec4f(fill.rgb, 1f) * fill.a * s) + (vec4f(outline.rgb, 1f) * outline.a * clamp(alpha - s, 0.0f, 1.0f));
}

const PI = 3.14159265359;

@fragment
fn circle(input: VertexOutput) -> @location(0) vec4f {
  let d = buf[input.index];
  let l = (d.dim.x + d.dim.y) * 0.5;
  let uv = (input.uv * 2.0) - 1.0;
  let w1 = (1.0 + d.blur) * fwidth(input.uv.x);

  let border = (d.border / l) * 2.0;
  // double because UV is in range [-1,1], not [0,1]
  let t = 0.50 - (d.corners.x / l);
  // We have to compensate for needing to do smoothstep starting from 0, which combined with abs()
  // acts as a ceil() function, creating one extra half pixel.
  let r = 1.0 - t - w1;

  // SDF for circle
  let inner = (d.corners.y / l) * 2.0;
  let d0 = abs(length(uv) - r + (border * 0.5) - (inner * 0.5)) - t + (border * 0.5) + (inner * 0.5);
  let d1 = abs(length(uv) - r) - t;
  let s = pow(linearstep(w1 * 2.0, 0.0, d0), 2.2);
  let alpha = pow(linearstep(w1 * 2.0, 0.0, d1), 2.2);
  let fill = srgb_to_linear_vec4(u32_to_vec4(d.fill));
  let outline = srgb_to_linear_vec4(u32_to_vec4(d.outline));

  // Output to screen
  return (vec4f(fill.rgb, 1) * fill.a * s) + (vec4f(outline.rgb, 1) * outline.a * clamp(alpha - s, 0.0, 1.0));
}

fn linetopoint(p1: vec2f, p2: vec2f, p: vec2f) -> f32 {
  let n = p2 - p1;
  let v = vec2f(n.y, - n.x);
  return dot(normalize(v), p1 - p);
}

@fragment
fn triangle(input: VertexOutput) -> @location(0) vec4f {
  let d = buf[input.index];
  let p = input.uv * d.dim + vec2f(- 0.5, 0.5);
  let c = d.corners;
  let p2 = vec2f(c.w * d.dim.x, 0.0);
  let r1 = linetopoint(p2, vec2f(0.0, d.dim.y), p);
  let r2 = - linetopoint(p2, d.dim, p);
  var r = max(r1, r2);
  r = max(r, p.y - d.dim.y);

  // Ideally we would get DPI for both height and width, but for now we just assume DPI isn't weird
  let w = fwidth(p.x) * (1.0 + d.blur);
  let s = 1.0 - linearstep(1.0 - d.border - w * 2.0, 1.0 - d.border - w, r);
  let alpha = linearstep(1.0 - w, 1.0 - w * 2.0, r);
  let fill = srgb_to_linear_vec4(u32_to_vec4(d.fill));
  let outline = srgb_to_linear_vec4(u32_to_vec4(d.outline));

  return (vec4(fill.rgb, 1.0) * fill.a * s) + (vec4(outline.rgb, 1.0) * outline.a * clamp(alpha - s, 0.0, 1.0));
}

fn rotate(p: vec2f, a: f32) -> vec2f {
  return vec2f(p.x * cos(a) + p.y * sin(a), p.x * sin(a) - p.y * cos(a));
}

@fragment
fn arcs(input: VertexOutput) -> @location(0) vec4f {
  let data = buf[input.index];
  let l = (data.dim.x + data.dim.y) * .5;
  let uv = (input.uv * 2.) - 1.;
  let width = fwidth(input.uv.x);
  let w1 = (1. + data.blur) * width;

  let border = (data.border / l) * 2.;
  // double because UV is in range [-1,1], not [0,1]
  let t = .50 - (data.corners.z / l) + w1 * 1.5;
  // We have to compensate for needing to do smoothstep starting from 0, which combined with abs()
  // acts as a ceil() function, creating one extra half pixel.
  let r = 1. - t + w1;

  // SDF for circle
  let d0 = abs(length(uv) - r) - t + border;
  let d1 = abs(length(uv) - r) - t;

  // SDF for lines that make up arc
  let omega1 = rotate(uv, data.corners.x - data.corners.y);
  let omega2 = rotate(uv, data.corners.x + data.corners.y);
  var d = 0.0;

  // TODO: This cannot deal with non-integer circle radii, but it might be generalizable to those cases.
  if (abs(- omega1.y) + abs(omega2.y) < width) {
    d = ((data.corners.y / PI) - 0.5) * 2.0 * width;
  }
  else if (data.corners.y > PI * 0.5) {
    d = max(- omega1.y, omega2.y);
  }
  else {
    d = min(- omega1.y, omega2.y);
  }

  // Compensate for blur so the circle is still full or empty at 2pi and 0.
  d += (clamp(data.corners.y / PI, 0.0, 1.0) - 0.5) * 2.0 * (data.blur * width) + border;

  let d2 = d - border + w1;
  let d3 = min(d, omega1.x + data.corners.y) + w1;

  // Merge results of both SDFs
  let s = linearstep(- w1, w1, min(- d0, d2) - w1);
  let alpha = linearstep(- w1, w1, min(- d1, d3) - w1);
  let fill = srgb_to_linear_vec4(u32_to_vec4(data.fill));
  let outline = srgb_to_linear_vec4(u32_to_vec4(data.outline));

  // Output to screen
  return vec4(fill.rgb, 1) * fill.a * s + vec4(outline.rgb, 1) * outline.a * clamp(alpha - s, 0.0, 1.0);
}

