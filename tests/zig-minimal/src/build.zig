

const std = @import("std");
const builtin = @import("builtin");

pub fn build(b: *std.build.Builder) anyerror!void {
    b.release_mode = builtin.Mode.Debug;
    const mode = b.standardReleaseOptions();

    const mainFile = "main.zig";
    var exe = b.addExecutable("debuginator-zig-demo", "../src/" ++ mainFile);
    exe.addIncludeDir("../src/");
    exe.addIncludeDir("../../..");
    exe.setBuildMode(mode);

    const cFlags = [_][]const u8{"-std=c99"};
    exe.addCSourceFile("the_debuginator_wrapper.c", &cFlags);

    exe.linkLibC();
    exe.linkSystemLibrary("user32");
    exe.linkSystemLibrary("gdi32");

    const run_cmd = exe.run();
    const run_step = b.step("run", "Run 'The Debuginator Zig Demo'");
    run_step.dependOn(&run_cmd.step);

    b.default_step.dependOn(&exe.step);
    b.installArtifact(exe);
}
