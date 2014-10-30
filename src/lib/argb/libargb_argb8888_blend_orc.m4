include(src/lib/argb/libargb_argb8888_core_orc.m4)dnl
dnl
dnl Blend (dst32, tmp64, a64, argb64)
dnl dst64[inout]: 0a,0r,0g,0b => argb8888_blend()
dnl a64[in]: A,A,A,A
dnl argb64[in]: 0a,0r,0g,0b
define(`argb8888_blend',`
argb8888_mul_256($1, $1, $2)
x4 addw $1, $1, $3')dnl

.function argb8888_sp_none_color_none_blend_orc
.flags 1d
.dest 4 d uint32_t
.param 4 color uint32_t
.temp 8 w_color
.temp 8 w_d
.temp 8 w_a
.temp 4 t2
.temp 4 t_d
.temp 4 t_c
.temp 2 a16

loadl t_d, d
loadpl t_c, color
argb8888_alpha_inv_get(a16, t2, color)
argb8888_c16_pack(w_a, t2, a16)
argb8888_pack(w_color, t_c)
argb8888_pack(w_d, t_d)
argb8888_blend(w_d, w_a, w_color)
argb8888_unpack(t_d, w_d)
storel d, t_d

.function argb8888_sp_argb8888_none_none_blend_orc
.flags 1d
.dest 4 d uint32_t
.source 4 s uint32_t
.temp 8 w_s
.temp 8 w_d
.temp 8 w_a
.temp 4 t2
.temp 4 t_s
.temp 4 t_d
.temp 2 a16

loadl t_s, s
loadl t_d, d
argb8888_alpha_inv_get(a16, t2, t_s)
argb8888_c16_pack(w_a, t2, a16)
argb8888_pack(w_s, t_s)
argb8888_pack(w_d, t_d)
argb8888_blend(w_d, w_a, w_s)
argb8888_unpack(t_d, w_d)
storel d, t_d

.function argb8888_sp_argb8888_color_none_blend_orc
.flags 1d
.dest 4 d uint32_t
.source 4 s uint32_t
.param 4 color uint32_t
.temp 8 w_s
.temp 8 w_d
.temp 8 w_a
.temp 8 w_color
.temp 8 w_mul
.temp 8 t1
.temp 4 t2
.temp 4 t_s
.temp 4 t_d
.temp 4 t_c
.temp 2 a16

loadl t_s, s
loadl t_d, d
loadpl t_c, color
argb8888_pack(w_s, t_s)
argb8888_pack(w_color, t_c)
argb8888_mul_sym(w_mul, t1, w_s, w_color)
argb8888_packed_alpha_inv_get(a16, t2, t1, w_mul)
argb8888_c16_pack(w_a, t2, a16)
argb8888_pack(w_d, t_d)
argb8888_blend(w_d, w_a, w_mul)
argb8888_unpack(t_d, w_d)
storel d, t_d
