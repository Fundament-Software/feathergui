#if 0
//
// Generated by Microsoft (R) HLSL Shader Compiler 10.1
//
//
// Buffer Definitions: 
//
// cbuffer constants
// {
//
//   float4 rect;                       // Offset:    0 Size:    16
//   float4 angles;                     // Offset:   16 Size:    16
//   float4 color;                      // Offset:   32 Size:    16
//   float4 outlinecolor;               // Offset:   48 Size:    16
//   float outline;                     // Offset:   64 Size:     4
//   float blur;                        // Offset:   68 Size:     4
//
// }
//
//
// Resource Bindings:
//
// Name                                 Type  Format         Dim      HLSL Bind  Count
// ------------------------------ ---------- ------- ----------- -------------- ------
// constants                         cbuffer      NA          NA            cb0      1 
//
//
//
// Input signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// SV_POSITION              0   xyzw        0      POS   float       
// SCENE_POSITION           0   xyzw        1     NONE   float   xy  
//
//
// Output signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// SV_TARGET                0   xyzw        0   TARGET   float   xyzw
//
//
// Constant buffer to DX9 shader constant mappings:
//
// Target Reg Buffer  Start Reg # of Regs        Data Conversion
// ---------- ------- --------- --------- ----------------------
// c0         cb0             0         5  ( FLT, FLT, FLT, FLT)
//
//
// Level9 shader bytecode:
//
    ps_2_x
    def c5, 0.5, 2, -1, 1
    def c6, 0, 0, 0, 0
    dcl t0
    add r0.xy, t0, -c0
    add r0.zw, -c0.xyxy, c0
    rcp r1.x, r0.z
    rcp r1.y, r0.w
    add r0.z, r0.w, r0.z
    mul r0.z, r0.z, c5.x
    rcp r0.z, r0.z
    mul r0.xy, r0, r1
    mad r0.yw, r0.xxzy, c5.y, c5.z
    dp2add r0.y, r0.ywzw, r0.ywzw, c6.x
    rsq r0.y, r0.y
    rcp r0.y, r0.y
    dsx r0.w, r0.x
    dsy r0.x, r0.x
    abs r0.xw, r0
    add r0.x, r0.x, r0.w
    mov r1.xw, c5
    mad r0.w, c1.x, -r0.z, r1.x
    add r1.x, -r0.w, c5.w
    add r1.y, r1.w, c4.y
    mad r1.x, r1.y, -r0.x, r1.x
    mul r0.x, r0.x, r1.y
    add r0.y, r0.y, -r1.x
    mad r1.x, c4.x, r0.z, r0.y
    abs r0.y, r0.y
    add r0.y, -r0.w, r0.y
    mad r0.y, r0.x, -c5.y, r0.y
    mad r1.x, c1.y, -r0.z, r1.x
    abs r1.x, r1.x
    add r0.w, -r0.w, r1.x
    mad r0.w, c4.x, r0.z, r0.w
    mad r0.z, c1.y, r0.z, r0.w
    mad r0.z, r0.x, -c5.y, r0.z
    add r0.x, r0.x, r0.x
    rcp r0.x, -r0.x
    mul_sat r0.z, r0.x, r0.z
    mul_sat r0.x, r0.x, r0.y
    add r0.x, -r0.z, r0.x
    mul r1.xyz, c3.w, c3
    mov r1.w, c3.w
    mul r1, r0.x, r1
    cmp r1, r0.x, r1, c6.x
    mul r2.xyz, c2.w, c2
    mov r2.w, c2.w
    mad r0, r2, r0.z, r1
    mov oC0, r0

// approximately 49 instruction slots used
ps_4_0
dcl_constantbuffer CB0[5], immediateIndexed
dcl_input_ps linear v1.xy
dcl_output o0.xyzw
dcl_temps 3
add r0.xy, v1.xyxx, -cb0[0].xyxx
add r0.zw, -cb0[0].xxxy, cb0[0].zzzw
div r0.xy, r0.xyxx, r0.zwzz
add r0.z, r0.w, r0.z
mul r0.z, r0.z, l(0.500000)
mad r0.yw, r0.xxxy, l(0.000000, 2.000000, 0.000000, 2.000000), l(0.000000, -1.000000, 0.000000, -1.000000)
dp2 r0.y, r0.ywyy, r0.ywyy
sqrt r0.y, r0.y
deriv_rtx r0.w, r0.x
deriv_rty r0.x, r0.x
add r0.x, |r0.x|, |r0.w|
div r1.xy, cb0[1].xyxx, r0.zzzz
div r0.z, cb0[4].x, r0.z
add r0.w, -r1.x, l(0.500000)
add r1.x, -r0.w, l(1.000000)
add r1.z, l(1.000000), cb0[4].y
mad r1.x, -r1.z, r0.x, r1.x
mul r0.x, r0.x, r1.z
add r0.y, r0.y, -r1.x
add r1.x, r0.z, r0.y
add r0.y, -r0.w, |r0.y|
mad r0.y, -r0.x, l(2.000000), r0.y
add r1.x, -r1.y, r1.x
add r0.w, -r0.w, |r1.x|
add r0.z, r0.z, r0.w
add r0.z, r1.y, r0.z
mad r0.z, -r0.x, l(2.000000), r0.z
add r0.x, r0.x, r0.x
div_sat r0.z, r0.z, -r0.x
div_sat r0.x, r0.y, -r0.x
add r0.x, -r0.z, r0.x
max r0.x, r0.x, l(0.000000)
mul r1.xyz, cb0[3].wwww, cb0[3].xyzx
mov r1.w, cb0[3].w
mul r1.xyzw, r0.xxxx, r1.xyzw
mul r2.xyz, cb0[2].wwww, cb0[2].xyzx
mov r2.w, cb0[2].w
mad o0.xyzw, r2.xyzw, r0.zzzz, r1.xyzw
ret 
// Approximately 39 instruction slots used
#endif

const BYTE Circle_main[] =
{
     68,  88,  66,  67, 205,  77, 
     19,  96, 131,  17,  94, 158, 
     13,  47, 109, 103,  54,  98, 
    133,  27,   1,   0,   0,   0, 
      4,  11,   0,   0,   6,   0, 
      0,   0,  56,   0,   0,   0, 
    136,   3,   0,   0, 132,   8, 
      0,   0,   0,   9,   0,   0, 
    116,  10,   0,   0, 208,  10, 
      0,   0,  65, 111, 110,  57, 
     72,   3,   0,   0,  72,   3, 
      0,   0,   0,   2, 255, 255, 
     24,   3,   0,   0,  48,   0, 
      0,   0,   1,   0,  36,   0, 
      0,   0,  48,   0,   0,   0, 
     48,   0,   0,   0,  36,   0, 
      0,   0,  48,   0,   0,   0, 
      0,   0,   5,   0,   0,   0, 
      0,   0,   0,   0,   1,   2, 
    255, 255,  81,   0,   0,   5, 
      5,   0,  15, 160,   0,   0, 
      0,  63,   0,   0,   0,  64, 
      0,   0, 128, 191,   0,   0, 
    128,  63,  81,   0,   0,   5, 
      6,   0,  15, 160,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  31,   0,   0,   2, 
      0,   0,   0, 128,   0,   0, 
     15, 176,   2,   0,   0,   3, 
      0,   0,   3, 128,   0,   0, 
    228, 176,   0,   0, 228, 161, 
      2,   0,   0,   3,   0,   0, 
     12, 128,   0,   0,  68, 161, 
      0,   0, 228, 160,   6,   0, 
      0,   2,   1,   0,   1, 128, 
      0,   0, 170, 128,   6,   0, 
      0,   2,   1,   0,   2, 128, 
      0,   0, 255, 128,   2,   0, 
      0,   3,   0,   0,   4, 128, 
      0,   0, 255, 128,   0,   0, 
    170, 128,   5,   0,   0,   3, 
      0,   0,   4, 128,   0,   0, 
    170, 128,   5,   0,   0, 160, 
      6,   0,   0,   2,   0,   0, 
      4, 128,   0,   0, 170, 128, 
      5,   0,   0,   3,   0,   0, 
      3, 128,   0,   0, 228, 128, 
      1,   0, 228, 128,   4,   0, 
      0,   4,   0,   0,  10, 128, 
      0,   0,  96, 128,   5,   0, 
     85, 160,   5,   0, 170, 160, 
     90,   0,   0,   4,   0,   0, 
      2, 128,   0,   0, 237, 128, 
      0,   0, 237, 128,   6,   0, 
      0, 160,   7,   0,   0,   2, 
      0,   0,   2, 128,   0,   0, 
     85, 128,   6,   0,   0,   2, 
      0,   0,   2, 128,   0,   0, 
     85, 128,  91,   0,   0,   2, 
      0,   0,   8, 128,   0,   0, 
      0, 128,  92,   0,   0,   2, 
      0,   0,   1, 128,   0,   0, 
      0, 128,  35,   0,   0,   2, 
      0,   0,   9, 128,   0,   0, 
    228, 128,   2,   0,   0,   3, 
      0,   0,   1, 128,   0,   0, 
      0, 128,   0,   0, 255, 128, 
      1,   0,   0,   2,   1,   0, 
      9, 128,   5,   0, 228, 160, 
      4,   0,   0,   4,   0,   0, 
      8, 128,   1,   0,   0, 160, 
      0,   0, 170, 129,   1,   0, 
      0, 128,   2,   0,   0,   3, 
      1,   0,   1, 128,   0,   0, 
    255, 129,   5,   0, 255, 160, 
      2,   0,   0,   3,   1,   0, 
      2, 128,   1,   0, 255, 128, 
      4,   0,  85, 160,   4,   0, 
      0,   4,   1,   0,   1, 128, 
      1,   0,  85, 128,   0,   0, 
      0, 129,   1,   0,   0, 128, 
      5,   0,   0,   3,   0,   0, 
      1, 128,   0,   0,   0, 128, 
      1,   0,  85, 128,   2,   0, 
      0,   3,   0,   0,   2, 128, 
      0,   0,  85, 128,   1,   0, 
      0, 129,   4,   0,   0,   4, 
      1,   0,   1, 128,   4,   0, 
      0, 160,   0,   0, 170, 128, 
      0,   0,  85, 128,  35,   0, 
      0,   2,   0,   0,   2, 128, 
      0,   0,  85, 128,   2,   0, 
      0,   3,   0,   0,   2, 128, 
      0,   0, 255, 129,   0,   0, 
     85, 128,   4,   0,   0,   4, 
      0,   0,   2, 128,   0,   0, 
      0, 128,   5,   0,  85, 161, 
      0,   0,  85, 128,   4,   0, 
      0,   4,   1,   0,   1, 128, 
      1,   0,  85, 160,   0,   0, 
    170, 129,   1,   0,   0, 128, 
     35,   0,   0,   2,   1,   0, 
      1, 128,   1,   0,   0, 128, 
      2,   0,   0,   3,   0,   0, 
      8, 128,   0,   0, 255, 129, 
      1,   0,   0, 128,   4,   0, 
      0,   4,   0,   0,   8, 128, 
      4,   0,   0, 160,   0,   0, 
    170, 128,   0,   0, 255, 128, 
      4,   0,   0,   4,   0,   0, 
      4, 128,   1,   0,  85, 160, 
      0,   0, 170, 128,   0,   0, 
    255, 128,   4,   0,   0,   4, 
      0,   0,   4, 128,   0,   0, 
      0, 128,   5,   0,  85, 161, 
      0,   0, 170, 128,   2,   0, 
      0,   3,   0,   0,   1, 128, 
      0,   0,   0, 128,   0,   0, 
      0, 128,   6,   0,   0,   2, 
      0,   0,   1, 128,   0,   0, 
      0, 129,   5,   0,   0,   3, 
      0,   0,  20, 128,   0,   0, 
      0, 128,   0,   0, 170, 128, 
      5,   0,   0,   3,   0,   0, 
     17, 128,   0,   0,   0, 128, 
      0,   0,  85, 128,   2,   0, 
      0,   3,   0,   0,   1, 128, 
      0,   0, 170, 129,   0,   0, 
      0, 128,   5,   0,   0,   3, 
      1,   0,   7, 128,   3,   0, 
    255, 160,   3,   0, 228, 160, 
      1,   0,   0,   2,   1,   0, 
      8, 128,   3,   0, 255, 160, 
      5,   0,   0,   3,   1,   0, 
     15, 128,   0,   0,   0, 128, 
      1,   0, 228, 128,  88,   0, 
      0,   4,   1,   0,  15, 128, 
      0,   0,   0, 128,   1,   0, 
    228, 128,   6,   0,   0, 160, 
      5,   0,   0,   3,   2,   0, 
      7, 128,   2,   0, 255, 160, 
      2,   0, 228, 160,   1,   0, 
      0,   2,   2,   0,   8, 128, 
      2,   0, 255, 160,   4,   0, 
      0,   4,   0,   0,  15, 128, 
      2,   0, 228, 128,   0,   0, 
    170, 128,   1,   0, 228, 128, 
      1,   0,   0,   2,   0,   8, 
     15, 128,   0,   0, 228, 128, 
    255, 255,   0,   0,  83,  72, 
     68,  82, 244,   4,   0,   0, 
     64,   0,   0,   0,  61,   1, 
      0,   0,  89,   0,   0,   4, 
     70, 142,  32,   0,   0,   0, 
      0,   0,   5,   0,   0,   0, 
     98,  16,   0,   3,  50,  16, 
     16,   0,   1,   0,   0,   0, 
    101,   0,   0,   3, 242,  32, 
     16,   0,   0,   0,   0,   0, 
    104,   0,   0,   2,   3,   0, 
      0,   0,   0,   0,   0,   9, 
     50,   0,  16,   0,   0,   0, 
      0,   0,  70,  16,  16,   0, 
      1,   0,   0,   0,  70, 128, 
     32, 128,  65,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,  10, 
    194,   0,  16,   0,   0,   0, 
      0,   0,   6, 132,  32, 128, 
     65,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
    166, 142,  32,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     14,   0,   0,   7,  50,   0, 
     16,   0,   0,   0,   0,   0, 
     70,   0,  16,   0,   0,   0, 
      0,   0, 230,  10,  16,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   7,  66,   0,  16,   0, 
      0,   0,   0,   0,  58,   0, 
     16,   0,   0,   0,   0,   0, 
     42,   0,  16,   0,   0,   0, 
      0,   0,  56,   0,   0,   7, 
     66,   0,  16,   0,   0,   0, 
      0,   0,  42,   0,  16,   0, 
      0,   0,   0,   0,   1,  64, 
      0,   0,   0,   0,   0,  63, 
     50,   0,   0,  15, 162,   0, 
     16,   0,   0,   0,   0,   0, 
      6,   4,  16,   0,   0,   0, 
      0,   0,   2,  64,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,  64,   0,   0,   0,   0, 
      0,   0,   0,  64,   2,  64, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 128, 191,   0,   0, 
      0,   0,   0,   0, 128, 191, 
     15,   0,   0,   7,  34,   0, 
     16,   0,   0,   0,   0,   0, 
    214,   5,  16,   0,   0,   0, 
      0,   0, 214,   5,  16,   0, 
      0,   0,   0,   0,  75,   0, 
      0,   5,  34,   0,  16,   0, 
      0,   0,   0,   0,  26,   0, 
     16,   0,   0,   0,   0,   0, 
     11,   0,   0,   5, 130,   0, 
     16,   0,   0,   0,   0,   0, 
     10,   0,  16,   0,   0,   0, 
      0,   0,  12,   0,   0,   5, 
     18,   0,  16,   0,   0,   0, 
      0,   0,  10,   0,  16,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   9,  18,   0,  16,   0, 
      0,   0,   0,   0,  10,   0, 
     16, 128, 129,   0,   0,   0, 
      0,   0,   0,   0,  58,   0, 
     16, 128, 129,   0,   0,   0, 
      0,   0,   0,   0,  14,   0, 
      0,   8,  50,   0,  16,   0, 
      1,   0,   0,   0,  70, 128, 
     32,   0,   0,   0,   0,   0, 
      1,   0,   0,   0, 166,  10, 
     16,   0,   0,   0,   0,   0, 
     14,   0,   0,   8,  66,   0, 
     16,   0,   0,   0,   0,   0, 
     10, 128,  32,   0,   0,   0, 
      0,   0,   4,   0,   0,   0, 
     42,   0,  16,   0,   0,   0, 
      0,   0,   0,   0,   0,   8, 
    130,   0,  16,   0,   0,   0, 
      0,   0,  10,   0,  16, 128, 
     65,   0,   0,   0,   1,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0,   0,  63,   0,   0, 
      0,   8,  18,   0,  16,   0, 
      1,   0,   0,   0,  58,   0, 
     16, 128,  65,   0,   0,   0, 
      0,   0,   0,   0,   1,  64, 
      0,   0,   0,   0, 128,  63, 
      0,   0,   0,   8,  66,   0, 
     16,   0,   1,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
    128,  63,  26, 128,  32,   0, 
      0,   0,   0,   0,   4,   0, 
      0,   0,  50,   0,   0,  10, 
     18,   0,  16,   0,   1,   0, 
      0,   0,  42,   0,  16, 128, 
     65,   0,   0,   0,   1,   0, 
      0,   0,  10,   0,  16,   0, 
      0,   0,   0,   0,  10,   0, 
     16,   0,   1,   0,   0,   0, 
     56,   0,   0,   7,  18,   0, 
     16,   0,   0,   0,   0,   0, 
     10,   0,  16,   0,   0,   0, 
      0,   0,  42,   0,  16,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   8,  34,   0,  16,   0, 
      0,   0,   0,   0,  26,   0, 
     16,   0,   0,   0,   0,   0, 
     10,   0,  16, 128,  65,   0, 
      0,   0,   1,   0,   0,   0, 
      0,   0,   0,   7,  18,   0, 
     16,   0,   1,   0,   0,   0, 
     42,   0,  16,   0,   0,   0, 
      0,   0,  26,   0,  16,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   9,  34,   0,  16,   0, 
      0,   0,   0,   0,  58,   0, 
     16, 128,  65,   0,   0,   0, 
      0,   0,   0,   0,  26,   0, 
     16, 128, 129,   0,   0,   0, 
      0,   0,   0,   0,  50,   0, 
      0,  10,  34,   0,  16,   0, 
      0,   0,   0,   0,  10,   0, 
     16, 128,  65,   0,   0,   0, 
      0,   0,   0,   0,   1,  64, 
      0,   0,   0,   0,   0,  64, 
     26,   0,  16,   0,   0,   0, 
      0,   0,   0,   0,   0,   8, 
     18,   0,  16,   0,   1,   0, 
      0,   0,  26,   0,  16, 128, 
     65,   0,   0,   0,   1,   0, 
      0,   0,  10,   0,  16,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   9, 130,   0,  16,   0, 
      0,   0,   0,   0,  58,   0, 
     16, 128,  65,   0,   0,   0, 
      0,   0,   0,   0,  10,   0, 
     16, 128, 129,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   7,  66,   0,  16,   0, 
      0,   0,   0,   0,  42,   0, 
     16,   0,   0,   0,   0,   0, 
     58,   0,  16,   0,   0,   0, 
      0,   0,   0,   0,   0,   7, 
     66,   0,  16,   0,   0,   0, 
      0,   0,  26,   0,  16,   0, 
      1,   0,   0,   0,  42,   0, 
     16,   0,   0,   0,   0,   0, 
     50,   0,   0,  10,  66,   0, 
     16,   0,   0,   0,   0,   0, 
     10,   0,  16, 128,  65,   0, 
      0,   0,   0,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
      0,  64,  42,   0,  16,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   7,  18,   0,  16,   0, 
      0,   0,   0,   0,  10,   0, 
     16,   0,   0,   0,   0,   0, 
     10,   0,  16,   0,   0,   0, 
      0,   0,  14,  32,   0,   8, 
     66,   0,  16,   0,   0,   0, 
      0,   0,  42,   0,  16,   0, 
      0,   0,   0,   0,  10,   0, 
     16, 128,  65,   0,   0,   0, 
      0,   0,   0,   0,  14,  32, 
      0,   8,  18,   0,  16,   0, 
      0,   0,   0,   0,  26,   0, 
     16,   0,   0,   0,   0,   0, 
     10,   0,  16, 128,  65,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   8,  18,   0, 
     16,   0,   0,   0,   0,   0, 
     42,   0,  16, 128,  65,   0, 
      0,   0,   0,   0,   0,   0, 
     10,   0,  16,   0,   0,   0, 
      0,   0,  52,   0,   0,   7, 
     18,   0,  16,   0,   0,   0, 
      0,   0,  10,   0,  16,   0, 
      0,   0,   0,   0,   1,  64, 
      0,   0,   0,   0,   0,   0, 
     56,   0,   0,   9, 114,   0, 
     16,   0,   1,   0,   0,   0, 
    246, 143,  32,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
     70, 130,  32,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
     54,   0,   0,   6, 130,   0, 
     16,   0,   1,   0,   0,   0, 
     58, 128,  32,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
     56,   0,   0,   7, 242,   0, 
     16,   0,   1,   0,   0,   0, 
      6,   0,  16,   0,   0,   0, 
      0,   0,  70,  14,  16,   0, 
      1,   0,   0,   0,  56,   0, 
      0,   9, 114,   0,  16,   0, 
      2,   0,   0,   0, 246, 143, 
     32,   0,   0,   0,   0,   0, 
      2,   0,   0,   0,  70, 130, 
     32,   0,   0,   0,   0,   0, 
      2,   0,   0,   0,  54,   0, 
      0,   6, 130,   0,  16,   0, 
      2,   0,   0,   0,  58, 128, 
     32,   0,   0,   0,   0,   0, 
      2,   0,   0,   0,  50,   0, 
      0,   9, 242,  32,  16,   0, 
      0,   0,   0,   0,  70,  14, 
     16,   0,   2,   0,   0,   0, 
    166,  10,  16,   0,   0,   0, 
      0,   0,  70,  14,  16,   0, 
      1,   0,   0,   0,  62,   0, 
      0,   1,  83,  84,  65,  84, 
    116,   0,   0,   0,  39,   0, 
      0,   0,   3,   0,   0,   0, 
      0,   0,   0,   0,   2,   0, 
      0,   0,  36,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   2,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     82,  68,  69,  70, 108,   1, 
      0,   0,   1,   0,   0,   0, 
     72,   0,   0,   0,   1,   0, 
      0,   0,  28,   0,   0,   0, 
      0,   4, 255, 255,   0,   1, 
      0,   0,  65,   1,   0,   0, 
     60,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   1,   0, 
      0,   0,  99, 111, 110, 115, 
    116,  97, 110, 116, 115,   0, 
    171, 171,  60,   0,   0,   0, 
      6,   0,   0,   0,  96,   0, 
      0,   0,  80,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 240,   0,   0,   0, 
      0,   0,   0,   0,  16,   0, 
      0,   0,   2,   0,   0,   0, 
    248,   0,   0,   0,   0,   0, 
      0,   0,   8,   1,   0,   0, 
     16,   0,   0,   0,  16,   0, 
      0,   0,   2,   0,   0,   0, 
    248,   0,   0,   0,   0,   0, 
      0,   0,  15,   1,   0,   0, 
     32,   0,   0,   0,  16,   0, 
      0,   0,   2,   0,   0,   0, 
    248,   0,   0,   0,   0,   0, 
      0,   0,  21,   1,   0,   0, 
     48,   0,   0,   0,  16,   0, 
      0,   0,   2,   0,   0,   0, 
    248,   0,   0,   0,   0,   0, 
      0,   0,  34,   1,   0,   0, 
     64,   0,   0,   0,   4,   0, 
      0,   0,   2,   0,   0,   0, 
     44,   1,   0,   0,   0,   0, 
      0,   0,  60,   1,   0,   0, 
     68,   0,   0,   0,   4,   0, 
      0,   0,   2,   0,   0,   0, 
     44,   1,   0,   0,   0,   0, 
      0,   0, 114, 101,  99, 116, 
      0, 171, 171, 171,   1,   0, 
      3,   0,   1,   0,   4,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  97, 110, 103, 108, 
    101, 115,   0,  99, 111, 108, 
    111, 114,   0, 111, 117, 116, 
    108, 105, 110, 101,  99, 111, 
    108, 111, 114,   0, 111, 117, 
    116, 108, 105, 110, 101,   0, 
    171, 171,   0,   0,   3,   0, 
      1,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     98, 108, 117, 114,   0,  77, 
    105,  99, 114, 111, 115, 111, 
    102, 116,  32,  40,  82,  41, 
     32,  72,  76,  83,  76,  32, 
     83, 104,  97, 100, 101, 114, 
     32,  67, 111, 109, 112, 105, 
    108, 101, 114,  32,  49,  48, 
     46,  49,   0, 171, 171, 171, 
     73,  83,  71,  78,  84,   0, 
      0,   0,   2,   0,   0,   0, 
      8,   0,   0,   0,  56,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   3,   0, 
      0,   0,   0,   0,   0,   0, 
     15,   0,   0,   0,  68,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   1,   0,   0,   0, 
     15,   3,   0,   0,  83,  86, 
     95,  80,  79,  83,  73,  84, 
     73,  79,  78,   0,  83,  67, 
     69,  78,  69,  95,  80,  79, 
     83,  73,  84,  73,  79,  78, 
      0, 171,  79,  83,  71,  78, 
     44,   0,   0,   0,   1,   0, 
      0,   0,   8,   0,   0,   0, 
     32,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   0,   0, 
      0,   0,  15,   0,   0,   0, 
     83,  86,  95,  84,  65,  82, 
     71,  69,  84,   0, 171, 171
};
