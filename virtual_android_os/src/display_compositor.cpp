/**
 * @file display_compositor.cpp
 * @brief Implementation of Virtual Display Compositor & SurfaceFlinger.
 */

#include "display_compositor.hpp"
#include <iostream>
#include <cstring>

namespace VPhone {

DisplayCompositor::DisplayCompositor() {}

DisplayCompositor::~DisplayCompositor() {
    stopRenderLoop();
}

bool DisplayCompositor::initialize(const DisplayMetrics& metrics) {
    metrics_ = metrics;
    size_t frame_size = metrics_.width * metrics_.height * 4; // RGBA_8888

    {
        std::lock_guard<std::mutex> lock(buffer_mutex_);
        frame_buffer_front_.resize(frame_size, 0);
        frame_buffer_back_.resize(frame_size, 0);

        // Fill default background with dark Slate Android System color (#121212)
        for (size_t i = 0; i < frame_size; i += 4) {
            frame_buffer_front_[i] = 18;      // R
            frame_buffer_front_[i + 1] = 18;  // G
            frame_buffer_front_[i + 2] = 18;  // B
            frame_buffer_front_[i + 3] = 255; // A
        }
    }

    return true;
}

bool DisplayCompositor::startRenderLoop() {
    is_rendering_ = true;
    current_fps_ = 60.0f;
    return true;
}

bool DisplayCompositor::stopRenderLoop() {
    is_rendering_ = false;
    current_fps_ = 0.0f;
    return true;
}

FramebufferDescriptor DisplayCompositor::acquireNextBuffer() {
    FramebufferDescriptor desc;
    desc.buffer_id = 1;
    desc.width = metrics_.width;
    desc.height = metrics_.height;
    desc.stride = metrics_.width * 4;
    desc.format = 1; // RGBA_8888
    desc.buffer_size = frame_buffer_back_.size();
    desc.buffer_memory = frame_buffer_back_.data();
    return desc;
}

bool DisplayCompositor::releaseBuffer(int /* buffer_id */) {
    swapBuffers();
    total_frames_++;
    return true;
}

void DisplayCompositor::swapBuffers() {
    std::lock_guard<std::mutex> lock(buffer_mutex_);
    std::swap(frame_buffer_front_, frame_buffer_back_);
}

void DisplayCompositor::setWindowMode(bool floating, int /* float_x */, int /* float_y */, int /* float_w */, int /* float_h */) {
    is_floating_ = floating;
}

void DisplayCompositor::setOrientation(int rotation_degrees) {
    metrics_.rotation = rotation_degrees;
}

float DisplayCompositor::getCurrentFps() const {
    return is_rendering_ ? 60.0f : 0.0f;
}

uint64_t DisplayCompositor::getTotalFramesRendered() const {
    return total_frames_;
}

const DisplayMetrics& DisplayCompositor::getMetrics() const {
    return metrics_;
}

const uint8_t* DisplayCompositor::getLatestFrameRgba() const {
    return frame_buffer_front_.data();
}

} // namespace VPhone
