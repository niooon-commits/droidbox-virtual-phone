/**
 * @file display_compositor.hpp
 * @brief Virtual Display Compositor & SurfaceFlinger Buffer Management.
 *
 * Emulates Android's SurfaceFlinger using ashmem shared memory buffers,
 * HardwareBuffer, and EGL/OpenGL ES context translation to render the guest
 * virtual screen inside a host floating window or full-screen container.
 */

#ifndef DISPLAY_COMPOSITOR_HPP
#define DISPLAY_COMPOSITOR_HPP

#include <cstdint>
#include <vector>
#include <string>
#include <mutex>

namespace VPhone {

struct DisplayMetrics {
    int width = 1080;
    int height = 2400;
    int dpi = 420;
    int refresh_rate = 60;
    int rotation = 0; // 0, 90, 180, 270
};

struct FramebufferDescriptor {
    int buffer_id;
    int width;
    int height;
    int stride;
    int format; // HAL_PIXEL_FORMAT_RGBA_8888
    void* buffer_memory = nullptr;
    size_t buffer_size = 0;
};

class DisplayCompositor {
public:
    DisplayCompositor();
    ~DisplayCompositor();

    bool initialize(const DisplayMetrics& metrics);
    bool startRenderLoop();
    bool stopRenderLoop();

    // Framebuffer operations
    FramebufferDescriptor acquireNextBuffer();
    bool releaseBuffer(int buffer_id);
    void swapBuffers();

    // Floating window & resize support
    void setWindowMode(bool floating, int float_x = 100, int float_y = 100, int float_w = 400, int float_h = 800);
    void setOrientation(int rotation_degrees);

    // Performance telemetry
    float getCurrentFps() const;
    uint64_t getTotalFramesRendered() const;
    const DisplayMetrics& getMetrics() const;

    // Buffer access for Host UI bridge / JNI TextureView
    const uint8_t* getLatestFrameRgba() const;

private:
    DisplayMetrics metrics_;
    bool is_rendering_ = false;
    bool is_floating_ = false;
    float current_fps_ = 60.0f;
    uint64_t total_frames_ = 0;

    std::vector<uint8_t> frame_buffer_front_;
    std::vector<uint8_t> frame_buffer_back_;
    mutable std::mutex buffer_mutex_;
};

} // namespace VPhone

#endif // DISPLAY_COMPOSITOR_HPP
