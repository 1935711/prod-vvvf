#include "shader.h"
#include "fb.h"
#include "pbar.h"
#include "rp.h"
#include "time.h"
#include <math.h>

#define SHADER_CALL(t, w, h, f, c)                                             \
    do                                                                         \
    {                                                                          \
        static float const dx = 1.0F / (float)w;                               \
        static float const dy = 1.0F / (float)h;                               \
        u16_t yi = 0U;                                                         \
        for (float y = -0.5F; y < 0.5F; y += dy, ++yi)                         \
        {                                                                      \
            u16_t xi = 0U;                                                     \
            for (float x = -0.5F; x < 0.5F; x += dx, ++xi)                     \
            {                                                                  \
                vec3u32_st const shade = f(t, x * (16.0F / 9.0F), y);          \
                c;                                                             \
            }                                                                  \
        }                                                                      \
    } while (0);

typedef struct vec3f_s
{
    float x;
    float y;
    float z;
} vec3f_st;

typedef struct vec3u32_s
{
    u32_t x;
    u32_t y;
    u32_t z;
} vec3u32_st;

static inline float min(float const a, float const b)
{
    return a < b ? a : b;
}

static inline float max(float const a, float const b)
{
    return a > b ? a : b;
}

static inline float lerp(float const a, float const b, float const f)
{
    return a + f * (b - a);
}

static inline vec3f_st vec3f_scalar_add(vec3f_st const v, float const s)
{
    return (vec3f_st){v.x + s, v.y + s, v.z + s};
}

static inline vec3f_st vec3f_scalar_sub(vec3f_st const v, float const s)
{
    return (vec3f_st){v.x - s, v.y - s, v.z - s};
}

static inline vec3f_st vec3f_scalar_mul(vec3f_st const v, float const s)
{
    return (vec3f_st){v.x * s, v.y * s, v.z * s};
}

static inline vec3f_st vec3f_vec_add(vec3f_st const v, vec3f_st const w)
{
    return (vec3f_st){v.x + w.x, v.y + w.y, v.z + w.z};
}

static inline vec3f_st vec3f_vec_sub(vec3f_st const v, vec3f_st const w)
{
    return (vec3f_st){v.x - w.x, v.y - w.y, v.z - w.z};
}

static inline vec3f_st vec3f_floor(vec3f_st const v)
{
    return (vec3f_st){floor(v.x), floor(v.y), floor(v.z)};
}

static inline vec3f_st vec3f_mod(vec3f_st const v, float const s)
{
    return (vec3f_st){fmod(v.x, s), fmod(v.y, s), fmod(v.z, s)};
}

static inline vec3f_st vec3f_abs(vec3f_st const v)
{
    return (vec3f_st){fabs(v.x), fabs(v.y), fabs(v.z)};
}

static inline vec3f_st vec3f_cos(vec3f_st const v)
{
    return (vec3f_st){cos(v.x), cos(v.y), cos(v.z)};
}

static inline vec3f_st vec3f_sin(vec3f_st const v)
{
    return (vec3f_st){sin(v.x), sin(v.y), sin(v.z)};
}

static inline vec3f_st vec3f_min(vec3f_st const v, vec3f_st const w)
{
    return (vec3f_st){v.x < w.x ? v.x : w.x, v.y < w.y ? v.y : w.y,
                      v.z < w.z ? v.z : w.z};
}

static inline vec3f_st vec3f_max(vec3f_st const v, vec3f_st const w)
{
    return (vec3f_st){v.x > w.x ? v.x : w.x, v.y > w.y ? v.y : w.y,
                      v.z > w.z ? v.z : w.z};
}

static inline float dot(vec3f_st const m, vec3f_st const n)
{
    return m.x * n.x + m.y * n.y + m.z * n.z;
}

static inline float length(vec3f_st const v)
{
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

static inline vec3f_st normalize(vec3f_st const v)
{
    float const l = length(v);
    return (vec3f_st){v.x / l, v.y / l, v.z / l};
}

static inline vec3u32_st vec3f_u32(vec3f_st const v)
{
    return (vec3u32_st){v.x, v.y, v.z};
}

static inline float sdf_cave(vec3f_st const p)
{
    return cos(p.x) + cos(p.y * 1.8F) + cos(p.y * 18.0F) * 0.1F + cos(p.z);
}

static inline vec3u32_st shader_raymarch(float const t, float const x,
                                         float const y)
{
    float const t_delta = (t * 8.0F);
    vec3f_st const origin = {0.0F, 0.0F, t_delta};
    vec3f_st p = origin;

    vec3f_st delta = {x, y, 1.0F};
    delta = normalize(delta);

    float const eps = 0.01F;
    float const step_max = 14.0F;
    for (float step = 0.0F; step < step_max; ++step)
    {
        float dst = sdf_cave(p);
        if (dst < eps)
        {
            u8_t const c = (1.0F - (step / step_max)) * UINT8_MAX;
            return (vec3u32_st){c, 0U, 0U};
            break;
        }
        p = (vec3f_st){p.x + delta.x * dst, p.y + delta.y * dst,
                       p.z + delta.z * dst};
    }
    return (vec3u32_st){0U, 0U, 0U};
}

static inline vec3u32_st shader_wave(float const t, float const x,
                                     float const y)
{
    vec3f_st uv = {x * 2.0F, y * 2.0F, 0.0F};
    float d = length(uv);
    float const a = atan2(uv.y, uv.x) + sin(t * 0.7F) * 0.5F;
    uv.y = sin(a) * d;
    uv.x = cos(a) * d + sin(uv.y * 2.0F + t) * 0.2F;
    d -= t;
    uv = vec3f_vec_add(
        uv, vec3f_scalar_mul(
                vec3f_sin(vec3f_scalar_add(vec3f_scalar_mul(uv, 1935.711F), t)),
                (fmod(t, 1.0F) - 0.5F) * 0.001F));

    u8_t const c_ch =
        fabs(fmod(uv.y + uv.x * d * 2.0F, uv.x * 1.1F)) * (float)UINT8_MAX;
    vec3u32_st const c = {c_ch > 220 ? UINT8_MAX : 8U, c_ch * 0.4F, c_ch};
    return c;
}

static inline vec3u32_st shader_field(float const t, float const x,
                                      float const y)
{
    vec3f_st uv = (vec3f_st){x, y, 0.0F};
    vec3f_st p = (vec3f_st){sin(t * 0.5F) * 0.7F, 0.2F * cos(t * 1.2F), 0.0F};

    float const neg = atan2(uv.x, uv.y);
    float const pos = atan2(p.x, p.y);
    float const np = atan2(uv.x - p.x, uv.y - p.y);

    vec3f_st const f = vec3f_vec_sub(
        (vec3f_st){0.5F, 0.5F, 0.5F},
        vec3f_scalar_mul(vec3f_sin((vec3f_st){
                             np - pos - M_PI * 0.5F,
                             (neg + np - 2.0F * pos) + M_PI * 0.5F,
                             neg - pos + M_PI * 0.5F,
                         }),
                         0.25F));
    return (vec3u32_st){(f.x - f.y / 2.0F) * (float)UINT8_MAX, 0U,
                        (f.z - f.y / 2.0F) * (float)UINT8_MAX};
}

static inline float sdf_malina(vec3f_st const p, float const t)
{
    vec3f_st const norm = normalize(p);
    vec3f_st uv = vec3f_scalar_mul(
        (vec3f_st){0.5F + atan(norm.z / norm.x) / (2.0F * M_PI),
                   0.5F - asin(norm.y * 0.61) / M_PI, 0.0F},
        25.0F);

    float bump = sin(uv.x + (t * 2.0F)) * sin(uv.y + M_PI) +
                 cos(uv.x - t) * cos(uv.y + M_PI);
    bump /= 8.0;
    return length(p) - 1.0 - bump;
}

static inline vec3u32_st shader_malina(float const t, float const x,
                                       float const y)
{
    vec3f_st const delta = normalize((vec3f_st){x, y, 1.0});
    vec3f_st const origin = {0.0F, -0.2F, -3.5F};

    float s = 0.0F;
    vec3f_st p = origin;
    float dst;

    u8_t const step_max = 5U;
    for (u8_t step = 0U; step < step_max; ++step)
    {
        dst = sdf_malina(p, t);
        s += dst * 0.4F;
        p = vec3f_vec_add(p, vec3f_scalar_mul(delta, dst));
    }

    float const malina_inside = length(p) - 1.05;
    float const c_raw = (dst < malina_inside ? 1.0F : 0.0F) / s;
    u8_t const c = (c_raw > 1.0F ? 1.0F : c_raw) * (float)UINT8_MAX;
    return (vec3u32_st){c, 0U, 0U};
}

static inline vec3u32_st shader_malina_leaves(float const t, float const x,
                                              float const y)
{
    static float const leaves = 4.0F;
    static float const sharpness = 0.01F;

    vec3f_st const uv = {x, y, 0.0F};
    float const a = atan2(uv.x, uv.y);
    float const r = length(uv);
    float const s = 0.5F + 0.5F * sin(leaves * a + 4.7F);
    float const g = sin(80.0F * a);
    float const d = 0.15F + 0.3F * sqrt(s) + sharpness * g;
    float const h = r / d;
    vec3f_st const c_raw = (vec3f_st){0.7F * h, 0.8F * h + 0.1F, 0.0F};
    if (c_raw.y < 0.7 && c_raw.y > 0.0)
    {
        vec3u32_st const c = vec3f_u32(
            vec3f_scalar_mul((vec3f_st){c_raw.x, c_raw.y, c_raw.z}, UINT8_MAX));
        return c;
    }
    else
    {
        return (vec3u32_st){0.0, 0.0, 0.0};
    }
}

void shader_raymarch_draw(float const time, u32_t const frame_idx)
{
    volatile u8_t const(
            *const frame)[SHADER_RAYMARCH_FRAME_COUNT][SHADER_RAYMARCH_FRAME_H]
                         [SHADER_RAYMARCH_FRAME_W][SHADER_RAYMARCH_FRAME_D] =
                             (u8_t const(*const)[SHADER_RAYMARCH_FRAME_COUNT]
                                                [SHADER_RAYMARCH_FRAME_H]
                                                [SHADER_RAYMARCH_FRAME_W]
                                                [SHADER_RAYMARCH_FRAME_D])
                                 SHADER_RAYMARCH_FRAME_ADDR;

    u8_t const color_effect = time >= (time_step[0U] + 15.0F) ? 1U : 0U;
    u8_t const time_delta = time - (time_step[0U] + 15.0F);

    for (u16_t y = 0U; y < SHADER_RAYMARCH_FRAME_H; ++y)
    {
        for (u16_t x = 0U; x < SHADER_RAYMARCH_FRAME_W; ++x)
        {
            u8_t const shade_raw = (*frame)[frame_idx][y][x][0U];
            u8_t const r = color_effect
                               ? (x / (float)SHADER_RAYMARCH_FRAME_W) *
                                         (shade_raw / (float)UINT8_MAX) *
                                         (float)UINT8_MAX -
                                     time_delta
                               : shade_raw,
                       g = color_effect
                               ? (y / (float)SHADER_RAYMARCH_FRAME_H) *
                                         (shade_raw / (float)UINT8_MAX) *
                                         (float)UINT8_MAX -
                                     time_delta
                               : shade_raw,
                       b = color_effect ? shade_raw - time_delta
                                        : shade_raw - 0.1F;
            for (u8_t py = 0U; py < SHADER_RAYMARCH_FRAME_DIV; ++py)
            {
                for (u8_t px = 0U; px < SHADER_RAYMARCH_FRAME_DIV; ++px)
                {
                    u16_t const frame_x = x * SHADER_RAYMARCH_FRAME_DIV + px;
                    u16_t const frame_y = y * SHADER_RAYMARCH_FRAME_DIV + py;
                    FB_DRAW_PIX(FB_BUF_A, frame_x, frame_y, r, g, b);
                }
            }
        }
    }
}

void shader_wave_draw(float const time)
{
    SHADER_CALL(time, SHADER_WAVE_FRAME_W, SHADER_WAVE_FRAME_H, shader_wave, {
        for (u8_t py = 0U; py < SHADER_WAVE_FRAME_DIV / 2; ++py)
        {
            for (u8_t px = 0U; px < SHADER_WAVE_FRAME_DIV / 2; ++px)
            {
                FB_DRAW_PIX(FB_BUF_A,
                            xi * SHADER_WAVE_FRAME_DIV + px + (py % 2U == 0U),
                            yi * SHADER_WAVE_FRAME_DIV + py + (py % 2U == 0U),
                            shade.x, shade.y, shade.z);
            }
        }
    });
}

void shader_field_draw(float const time)
{
    SHADER_CALL(
        time, SHADER_FIELD_FRAME_W, SHADER_FIELD_FRAME_H, shader_field, {
            for (u16_t py = 0U; py < SHADER_FIELD_FRAME_DIV; ++py)
            {
                for (u16_t px = 0U; px < SHADER_FIELD_FRAME_DIV; ++px)
                {
                    u16_t const draw_y = (yi * SHADER_FIELD_FRAME_DIV) + py;
                    FB_DRAW_PIX(FB_BUF_A,
                                (xi * SHADER_FIELD_FRAME_DIV) + px +
                                    (draw_y % 2U == 0U),
                                draw_y, shade.x, shade.y, shade.z);
                }
            }
        });
}

void shader_malina_draw(float const time, u32_t const frame_idx)
{
    volatile u8_t const(*const frame)
        [SHADER_MALINA_FRAME_COUNT][SHADER_MALINA_FRAME_H]
        [SHADER_MALINA_FRAME_W][SHADER_MALINA_FRAME_D] =
            (u8_t const(*const)[SHADER_MALINA_FRAME_COUNT]
                               [SHADER_MALINA_FRAME_H][SHADER_MALINA_FRAME_W]
                               [SHADER_MALINA_FRAME_D])SHADER_MALINA_FRAME_ADDR;

    for (u16_t y = 0U; y < SHADER_MALINA_FRAME_H; ++y)
    {
        for (u16_t x = 0U; x < SHADER_MALINA_FRAME_W; ++x)
        {
            u8_t const r = (*frame)[frame_idx][y][x][0U],
                       g = (*frame)[frame_idx][y][x][1U],
                       b = (*frame)[frame_idx][y][x][2U];
            for (u8_t py = 0U; py < SHADER_MALINA_FRAME_DIV; ++py)
            {
                for (u8_t px = 0U; px < SHADER_MALINA_FRAME_DIV; ++px)
                {
                    u16_t const frame_x = x * SHADER_MALINA_FRAME_DIV + px;
                    u16_t const frame_y = y * SHADER_MALINA_FRAME_DIV + py;
                    FB_DRAW_PIX(FB_BUF_A, frame_x, frame_y, r, g, b);
                }
            }
        }
    }
}

void shader_raymarch_calc_frames(float const time_delta)
{
    volatile u8_t(*const frame)
        [SHADER_RAYMARCH_FRAME_COUNT][SHADER_RAYMARCH_FRAME_H]
        [SHADER_RAYMARCH_FRAME_W][SHADER_RAYMARCH_FRAME_D] =
            (u8_t(*const)[SHADER_RAYMARCH_FRAME_COUNT][SHADER_RAYMARCH_FRAME_H]
                         [SHADER_RAYMARCH_FRAME_W][SHADER_RAYMARCH_FRAME_D])
                SHADER_RAYMARCH_FRAME_ADDR;

    float time = 0.0F;
    for (u32_t frame_idx = 0U; frame_idx < SHADER_RAYMARCH_FRAME_COUNT;
         ++frame_idx)
    {
        SHADER_CALL(time, SHADER_RAYMARCH_FRAME_W, SHADER_RAYMARCH_FRAME_H,
                    shader_raymarch,
                    { (*frame)[frame_idx][yi][xi][0U] = shade.x; });
        time += time_delta;

        if (frame_idx % (SHADER_RAYMARCH_FRAME_COUNT / 40U) == 0U)
        {
            pbar_delta(1U, 'R');
        }
    }
}

void shader_malina_calc_frames(float const time_delta)
{
    volatile u8_t(*const leaves)[SHADER_MALINA_LEAVES_FRAME_H]
                                [SHADER_MALINA_LEAVES_FRAME_W]
                                [SHADER_MALINA_LEAVES_FRAME_D] = (volatile u8_t(
                                        *const)[SHADER_MALINA_LEAVES_FRAME_H]
                                               [SHADER_MALINA_LEAVES_FRAME_W]
                                               [SHADER_MALINA_LEAVES_FRAME_D])
                                    SHADER_MALINA_LEAVES_FRAME_ADDR;

    volatile u8_t(*const frame)
        [SHADER_MALINA_FRAME_COUNT][SHADER_MALINA_FRAME_H]
        [SHADER_MALINA_FRAME_W][SHADER_MALINA_FRAME_D] =
            (u8_t(*const)[SHADER_MALINA_FRAME_COUNT][SHADER_MALINA_FRAME_H]
                         [SHADER_MALINA_FRAME_W][SHADER_MALINA_FRAME_D])
                SHADER_MALINA_FRAME_ADDR;

    float time = 0.0F;
    for (u32_t frame_idx = 0U; frame_idx < SHADER_MALINA_FRAME_COUNT;
         ++frame_idx)
    {
        SHADER_CALL(time, SHADER_MALINA_FRAME_W, SHADER_MALINA_FRAME_H,
                    shader_malina, {
                        (*frame)[frame_idx][yi][xi][0U] = shade.x;
                        (*frame)[frame_idx][yi][xi][1U] = shade.y;
                        (*frame)[frame_idx][yi][xi][2U] = shade.z;
                    });

        /* Ignore the bottom half. */
        for (u16_t ly = 3U; ly < (SHADER_MALINA_LEAVES_FRAME_H / 2U) + 2U; ++ly)
        {
            for (u16_t lx = 0U; lx < SHADER_MALINA_LEAVES_FRAME_W; ++lx)
            {
                u8_t const r = (*leaves)[ly][lx][0U], g = (*leaves)[ly][lx][1U],
                           b = (*leaves)[ly][lx][2U];
                if (r || g || b)
                {
                    (*frame)[frame_idx][ly - 13U][lx][0U] = r;
                    (*frame)[frame_idx][ly - 13U][lx][1U] = g;
                    (*frame)[frame_idx][ly - 13U][lx][2U] = b;
                }
            }
        }

        time += time_delta;

        if (frame_idx % (SHADER_MALINA_FRAME_COUNT / 8U) == 0U)
        {
            pbar_delta(1U, 'M');
        }
    }
}

void shader_malina_calc_leaves(void)
{
    volatile u8_t(*const leaves)[SHADER_MALINA_LEAVES_FRAME_H]
                                [SHADER_MALINA_LEAVES_FRAME_W]
                                [SHADER_MALINA_LEAVES_FRAME_D] = (volatile u8_t(
                                        *const)[SHADER_MALINA_LEAVES_FRAME_H]
                                               [SHADER_MALINA_LEAVES_FRAME_W]
                                               [SHADER_MALINA_LEAVES_FRAME_D])
                                    SHADER_MALINA_LEAVES_FRAME_ADDR;

    SHADER_CALL(0.0F, SHADER_MALINA_LEAVES_FRAME_W,
                SHADER_MALINA_LEAVES_FRAME_H, shader_malina_leaves, {
                    (*leaves)[yi][xi][0U] = shade.x * 0.8F;
                    (*leaves)[yi][xi][1U] = shade.y * 0.8F;
                    (*leaves)[yi][xi][2U] = shade.z * 0.8F;
                });
}
