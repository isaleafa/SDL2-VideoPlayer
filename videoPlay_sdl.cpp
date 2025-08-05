// =================================================================================
// 新方案：使用 SDL2 的图形化视频播放器 (v6 - 增强音频错误诊断)
//
// 【重要】此版本需要使用新版的 Python 脚本来生成带有文件头的数据文件。
//
// 编译脚本：videoPlay_sdl.bat
//
// =================================================================================

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <thread>
#include <chrono>
#include <cstdint> // 用于 uint32_t
#include <windows.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

// --- 全局变量，用于存储从文件读取的视频信息 ---
uint32_t g_frame_width = 0;
uint32_t g_frame_height = 0;
uint32_t g_frame_count = 0;
float    g_fps = 0.0f; 

const std::string FRAME_DATA_PATH = "./run/frames.data";
// --- 不再需要固定的音频路径常量 ---
// const std::string AUDIO_DATA_PATH = "audio.mp3";

// 像素结构保持不变
struct FramePixel {
    unsigned char R, G, B;
};

// 使用一个大的 vector 来存储整个视频的所有像素数据
std::vector<FramePixel> g_all_pixels;

// 读取带有文件头的数据文件
bool readFramesData() {
    std::ifstream file(FRAME_DATA_PATH, std::ios::binary);
    if (!file) {
        std::cerr << "错误：无法打开 " << FRAME_DATA_PATH << std::endl;
        return false;
    }

    // 1. 读取文件头部的元数据
    file.read(reinterpret_cast<char*>(&g_frame_width), sizeof(g_frame_width));
    file.read(reinterpret_cast<char*>(&g_frame_height), sizeof(g_frame_height));
    file.read(reinterpret_cast<char*>(&g_frame_count), sizeof(g_frame_count));
    file.read(reinterpret_cast<char*>(&g_fps), sizeof(g_fps));

    if (!file) {
        std::cerr << "错误：读取文件头失败。" << std::endl;
        return false;
    }
    
    std::cout << "从文件读取视频信息: " 
              << g_frame_width << "x" << g_frame_height 
              << ", " << g_frame_count << " 帧"
              << ", " << g_fps << " FPS" << std::endl;

    // 2. 根据元数据准备内存并读取所有像素
    size_t total_pixels = (size_t)g_frame_width * g_frame_height * g_frame_count;
    g_all_pixels.resize(total_pixels);

    file.read(reinterpret_cast<char*>(g_all_pixels.data()), total_pixels * sizeof(FramePixel));
    
    // 检查是否读取了预期大小的数据
    if ((size_t)file.gcount() != total_pixels * sizeof(FramePixel)) {
        std::cerr << "错误：文件大小与元数据不匹配。" << std::endl;
        return false;
    }

    return true;
}


int main(int argc, char* argv[]) {
    // --- 设置控制台输出为 UTF-8 编码，解决中文乱码问题 ---
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    // --- 1. 初始化 SDL ---
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL 初始化失败: " << SDL_GetError() << std::endl;
        return -1;
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "SDL_mixer 初始化失败: " << Mix_GetError() << std::endl;
        return -1;
    }

    // --- 2. 加载数据 ---
    std::cout << "正在加载帧数据..." << std::endl;
    if (!readFramesData()) {
        std::cerr << "加载帧数据失败！" << std::endl;
        return -1;
    }

    // --- 关键修改：自动检测并加载音频文件，并提供详细错误信息 ---
    Mix_Music *music = nullptr;
    std::vector<std::string> audio_paths = {"./run/audio.mp3", "./run/audio.m4a", "./run/audio.ogg", "./run/audio.wav"};
    for (const auto& path : audio_paths) {
        // 首先检查文件是否存在
        std::ifstream f(path.c_str());
        if (!f.good()) {
            continue; // 文件不存在，直接跳过
        }
        
        music = Mix_LoadMUS(path.c_str());
        if (music) {
            std::cout << "成功加载音频文件: " << path << std::endl;
            break; // 一旦成功加载，就跳出循环
        } else {
            // 如果加载失败，打印出具体原因
            std::cerr << "尝试加载 " << path << " 失败: " << Mix_GetError() << std::endl;
        }
    }

    if (!music) {
        std::cerr << "错误：在项目目录中找不到任何可成功加载的音频文件。" << std::endl;
        return -1;
    }
    // ------------------------------------

    // --- 3. 创建窗口和渲染器 ---
    const int PIXEL_SCALE = 10;
    // 使用从文件读取的尺寸创建窗口
    SDL_Window* window = SDL_CreateWindow(
        "视频播放器",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        g_frame_width * PIXEL_SCALE, g_frame_height * PIXEL_SCALE,
        SDL_WINDOW_SHOWN
    );
    if (!window) {
        std::cerr << "窗口创建失败: " << SDL_GetError() << std::endl;
        return -1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "渲染器创建失败: " << SDL_GetError() << std::endl;
        return -1;
    }

    // --- 4. 播放和主循环 ---
    std::cout << "开始播放..." << std::endl;
    Mix_PlayMusic(music, 1);
    Uint32 start_time = SDL_GetTicks();
    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        Uint32 elapsed_ms = SDL_GetTicks() - start_time;
        // 使用从文件读取的 g_fps 进行计算
        int current_frame_index = static_cast<int>((elapsed_ms / 1000.0) * g_fps);

        if (current_frame_index >= g_frame_count) {
            running = false;
            continue;
        }

        // 计算当前帧在总像素数据中的起始位置
        size_t frame_offset = (size_t)current_frame_index * g_frame_width * g_frame_height;

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        for (uint32_t y = 0; y < g_frame_height; ++y) {
            for (uint32_t x = 0; x < g_frame_width; ++x) {
                // 计算当前像素的索引
                const auto& p = g_all_pixels[frame_offset + y * g_frame_width + x];
                
                SDL_Rect pixel_rect = { (int)x * PIXEL_SCALE, (int)y * PIXEL_SCALE, PIXEL_SCALE, PIXEL_SCALE };
                
                SDL_SetRenderDrawColor(renderer, p.R, p.G, p.B, 255);
                SDL_RenderFillRect(renderer, &pixel_rect);
            }
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(10);
    }

    // --- 5. 清理和退出 ---
    std::cout << "播放结束。" << std::endl;
    Mix_FreeMusic(music);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    Mix_CloseAudio();
    SDL_Quit();

    return 0;
}
