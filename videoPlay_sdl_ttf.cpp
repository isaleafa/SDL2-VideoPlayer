// =================================================================================
// 新方案：使用 SDL2 的图形化视频播放器 (v10 - 紧凑行距最终版)
//
// 【重要】此版本需要将所有资源文件（DLLs, data, audio, font）都放在 run 目录中。
//
// 编译指令示例 (无变化):
// g++ videoPlay_sdl_ttf.cpp -o ./run/videoPlay_sdl_ttf.exe -I./SDL2/include -L./SDL2/lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_mixer -lSDL2_ttf
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
#include <SDL2/SDL_ttf.h> // 用于处理字体的头文件

// --- 全局变量，用于存储从文件读取的视频信息 ---
uint32_t g_frame_width = 0;
uint32_t g_frame_height = 0;
uint32_t g_frame_count = 0;
float    g_fps = 0.0f; 

// --- 路径已根据你的修改进行统一 ---
const std::string FRAME_DATA_PATH = "./run/frames.data";

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
    
    if ((size_t)file.gcount() != total_pixels * sizeof(FramePixel)) {
        std::cerr << "错误：文件大小与元数据不匹配。" << std::endl;
        return false;
    }

    return true;
}


int main(int argc, char* argv[]) {
    // --- 设置控制台输出为 UTF-8 编码 ---
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    
    // --- 打印版本信息以供调试 ---
    std::cout << "正在运行 [# 字符复古版 v4 - 紧凑行距最终版]" << std::endl;

    // --- 1. 初始化 SDL 和相关库 ---
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL 初始化失败: " << SDL_GetError() << std::endl;
        return -1;
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "SDL_mixer 初始化失败: " << Mix_GetError() << std::endl;
        return -1;
    }
    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf 初始化失败: " << TTF_GetError() << std::endl;
        return -1;
    }

    // --- 2. 加载数据和字体 ---
    std::cout << "正在加载帧数据..." << std::endl;
    if (!readFramesData()) {
        std::cerr << "加载帧数据失败！" << std::endl;
        return -1;
    }

    // 自动检测并加载音频文件
    Mix_Music *music = nullptr;
    std::vector<std::string> audio_paths = {"./run/audio.mp3", "./run/audio.m4a", "./run/audio.ogg", "./run/audio.wav"};
    for (const auto& path : audio_paths) {
        music = Mix_LoadMUS(path.c_str());
        if (music) {
            std::cout << "成功加载音频文件: " << path << std::endl;
            break;
        }
    }
    if (!music) {
        std::cerr << "错误：在 ./run/ 目录中找不到任何支持的音频文件。" << std::endl;
        return -1;
    }

    // 加载字体文件
    const int FONT_SIZE = 10;
    TTF_Font* font = TTF_OpenFont("./run/font.ttf", FONT_SIZE);
    if (!font) {
        std::cerr << "字体文件 './run/font.ttf' 加载失败: " << TTF_GetError() << std::endl;
        return -1;
    }

    // --- 3. 创建窗口和渲染器 ---
    int char_width, char_height;
    TTF_SizeText(font, "#", &char_width, &char_height);
    
    // --- 核心修改：引入垂直重叠，消除行间距 ---
    // 你可以微调这个值，2 通常是一个不错的开始
    constexpr int VERTICAL_OVERLAP = 2; 
    const int vertical_step = char_height - VERTICAL_OVERLAP;

    // 根据字符的实际渲染尺寸和步长来计算窗口大小
    int window_width = g_frame_width * char_width;
    // 精确计算窗口高度，以确保最后一行能完整显示
    int window_height = (g_frame_height > 0) ? ((g_frame_height - 1) * vertical_step + char_height) : 0;


    SDL_Window* window = SDL_CreateWindow(
        "视频播放器 (文明倒退版)",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        window_width, window_height,
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

    // --- 性能优化：预先渲染一个白色的“#”字符纹理 ---
    SDL_Surface* text_surface = TTF_RenderUTF8_Solid(font, "#", {255, 255, 255, 255});
    if (!text_surface) {
        std::cerr << "无法创建'#'字符表面: " << TTF_GetError() << std::endl;
        return -1;
    }
    SDL_Texture* hash_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    SDL_FreeSurface(text_surface);
    if (!hash_texture) {
        std::cerr << "无法从表面创建'#'字符纹理: " << SDL_GetError() << std::endl;
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
        int current_frame_index = static_cast<int>((elapsed_ms / 1000.0) * g_fps);

        if (current_frame_index >= g_frame_count) {
            running = false;
            continue;
        }

        size_t frame_offset = (size_t)current_frame_index * g_frame_width * g_frame_height;

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // --- 性能优化后的渲染循环 ---
        for (uint32_t y = 0; y < g_frame_height; ++y) {
            for (uint32_t x = 0; x < g_frame_width; ++x) {
                const auto& p = g_all_pixels[frame_offset + y * g_frame_width + x];
                
                SDL_SetTextureColorMod(hash_texture, p.R, p.G, p.B);
                
                // --- 核心修改：使用新的 vertical_step 来计算 Y 坐标 ---
                SDL_Rect dest_rect = { (int)x * char_width, (int)y * vertical_step, char_width, char_height };

                SDL_RenderCopy(renderer, hash_texture, NULL, &dest_rect);
            }
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(10);
    }

    // --- 5. 清理和退出 ---
    std::cout << "播放结束。" << std::endl;
    SDL_DestroyTexture(hash_texture);
    Mix_FreeMusic(music);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    Mix_CloseAudio();
    SDL_Quit();

    return 0;
}
