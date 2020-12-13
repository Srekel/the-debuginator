const std = @import("std");
const debuginator_c = @import("c.zig");

pub export fn draw_rect(position: [*c]debuginator_c.DebuginatorVector2, size: [*c]debuginator_c.DebuginatorVector2, color: [*c]debuginator_c.DebuginatorColor, userdata: ?*c_void) void {}
pub export fn draw_text(text: [*c]const u8, position: [*c]debuginator_c.DebuginatorVector2, color: [*c]debuginator_c.DebuginatorColor, font: [*c]debuginator_c.DebuginatorFont, userdata: ?*c_void) void {}
pub export fn text_size(text: [*c]const u8, font: [*c]debuginator_c.DebuginatorFont, userdata: ?*c_void) debuginator_c.DebuginatorVector2 {
    var size = debuginator_c.DebuginatorVector2{
        .x = 30,
        .y = 10,
    };
    return size;
}

pub export fn word_wrap(text: [*c]const u8, font: [*c]debuginator_c.DebuginatorFont, max_width: f32, row_count: [*c]c_int, row_lengths: [*c]c_int, row_lengths_buffer_size: c_int, userdata: ?*c_void) void {
    // std.debug.print("WW {}\n", .{max_width});
}

pub export fn log(text: [*c]const u8, userdata: ?*c_void) void {
    std.debug.print("LOG {}\n", .{text});
}


pub fn main() void {
    var arena = std.heap.ArenaAllocator.init(std.heap.page_allocator);
    defer arena.deinit();

    const buffer_size = 1024 * 1024;
    var buffer = arena.allocator.alloc(u8, buffer_size) catch unreachable;

    var config = std.mem.zeroes(debuginator_c.TheDebuginatorConfig);
    debuginator_c.debuginator_get_default_config(&config);
    config.memory_arena = @ptrCast([*c]u8, buffer);
    config.memory_arena_capacity = buffer_size;
    config.draw_rect = draw_rect;
    config.draw_text = draw_text;
    config.text_size = text_size;
    config.word_wrap = word_wrap;
    config.log = log;
    config.size.x = 300;
    config.size.y = 1000;
    config.screen_resolution.x = 1200;
    config.screen_resolution.y = config.size.y;
    config.app_user_data = &config;


    std.debug.print("BEGIN {}\n", .{config.create_default_debuginator_items});
    var debuginator: ?*debuginator_c.TheDebuginator = debuginator_c.alloc_debuginator();
    std.debug.assert(debuginator != null);

    std.debug.print("CREATE {}\n", .{config.create_default_debuginator_items});
    debuginator_c.debuginator_create(&config, debuginator);

    std.debug.print("UPDATE {}\n", .{config.create_default_debuginator_items});
    debuginator_c.debuginator_update(debuginator, 0.1);
    debuginator_c.debuginator_draw(debuginator, 0.1);

    std.debug.print("END {}\n", .{config.create_default_debuginator_items});
}
