#pragma once

#include <common.h>
#include <RendererBackendConfig.h>

#include <functional>

namespace efreet::engine::renderer::backend {

b8 create(const RendererBackendConfig& config);
void destroy();


BackendType type();
u32 version();
// u64 frameNumber_{ 0 };

b8 init(const RendererBackendConfig& config);
void cleanup();




// b8 (*init)(const RendererBackendConfig& config);
// void (*cleanup)();
// void (*resized)(u16 width, u16 height);
// b8 (*begin_frame)(RendererBackend* backend, f32 delta_time);
// b8 (*end_frame)(RendererBackend* backend, f32 delta_time);
// b8 (*begin_renderpass)(RendererBackend* backend, u8 renderpass_id);
// b8 (*end_renderpass)(RendererBackend* backend, u8 renderpass_id);
// void (*draw_geometry)(geometry_render_data data);
// void (*texture_create)(const u8* pixels, struct texture* texture);
// void (*texture_destroy)(struct texture* texture);
// void (*texture_create_writeable)(texture* t);
// void (*texture_resize)(texture* t, u32 new_width, u32 new_height);
// void (*texture_write_data)(texture* t, u32 offset, u32 size, const u8* pixels);
// b8 (*create_geometry)(geometry* geometry, u32 vertex_size, u32 vertex_count, const void* vertices, u32 index_size, u32 index_count, const void* indices);
// void (*destroy_geometry)(geometry* geometry);
// b8 (*shader_create)(struct shader* shader, u8 renderpass_id, u8 stage_count, const char** stage_filenames, shader_stage* stages);
// void (*shader_destroy)(struct shader* shader);
// b8 (*shader_initialize)(struct shader* shader);
// b8 (*shader_use)(struct shader* shader);
// b8 (*shader_bind_globals)(struct shader* s);
// b8 (*shader_bind_instance)(struct shader* s, u32 instance_id);
// b8 (*shader_apply_globals)(struct shader* s);
// b8 (*shader_apply_instance)(struct shader* s, b8 needs_update);
// b8 (*shader_acquire_instance_resources)(struct shader* s, texture_map** maps, u32* out_instance_id);
// b8 (*shader_release_instance_resources)(struct shader* s, u32 instance_id);
// b8 (*shader_set_uniform)(struct shader* frontend_shader, struct shader_uniform* uniform, const void* value);
// b8 (*texture_map_acquire_resources)(struct texture_map* map);
// void (*texture_map_release_resources)(struct texture_map* map);

} // namespace efreet::engine::renderer::backend