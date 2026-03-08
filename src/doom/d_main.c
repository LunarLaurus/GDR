


void D_ResetFrameStats(void)
{
    frame_total_time_ms = 0;
    frame_tic_time_ms = 0;
    frame_render_time_ms = 0;
    frame_display_time_ms = 0;
    frame_count = 0;
}

void D_PrintFrameProfile(void)
{
    if (frame_count == 0)
    {
        DEH_printf("No frames recorded.\n");
        return;
    }

    DEH_printf("=== Frame Time Profile ===\n");
    DEH_printf("Frames: %d\n", frame_count);
    DEH_printf("Total frame time: %d ms\n", frame_total_time_ms);
    DEH_printf("Avg frame time: %.2f ms\n", (float)frame_total_time_ms / frame_count);
    DEH_printf("Avg FPS: %.1f\n", (float)frame_count * 1000 / frame_total_time_ms);
    DEH_printf("Tic (game logic): %d ms (%d%%)\n", frame_tic_time_ms,
        frame_total_time_ms > 0 ? (frame_tic_time_ms * 100) / frame_total_time_ms : 0);
    DEH_printf("Render: %d ms (%d%%)\n", frame_render_time_ms,
        frame_total_time_ms > 0 ? (frame_render_time_ms * 100) / frame_total_time_ms : 0);
    DEH_printf("Display: %d ms (%d%%)\n", frame_display_time_ms,
        frame_total_time_ms > 0 ? (frame_display_time_ms * 100) / frame_total_time_ms : 0);
}


