import os
import subprocess # 引入 subprocess 用于更底层的命令执行和调试

# --- 【重要】请在这里修改为你自己的 FFmpeg 路径 ---
# 1. 找到你之前解压的 ffmpeg 文件夹。
# 2. 进入里面的 bin 文件夹。
# 3. 将 ffmpeg.exe 的完整路径复制到下面，注意要使用正斜杠 / 或者双反斜杠 \\
# 示例: "C:/ffmpeg/bin/ffmpeg.exe" 或 "E:\\ffmpeg\\bin\\ffmpeg.exe"
#
# 如果你不确定，可以先留空，脚本会尝试从系统环境变量中寻找。
ffmpeg_path = "E:\\Apps\\ffmpeg\\bin\\ffmpeg.exe" 
# ----------------------------------------------------


# --- 配置 ---
# 输入的 M4A 文件名
input_file = 'audio.m4a'
# 输出的 MP3 文件名
output_file = './run/audio.mp3'
# --- 配置结束 ---

def check_ffmpeg(path):
    """检查 FFmpeg 是否配置正确并可执行"""
    print("--- 开始进行 FFmpeg 环境诊断 ---")
    
    # 步骤 1: 确定 ffmpeg.exe 的最终路径
    final_ffmpeg_path = path if path else "ffmpeg"
    print(f"[诊断] 最终将要执行的命令是: '{final_ffmpeg_path}'")

    # 步骤 2: 检查 ffmpeg.exe 文件是否存在 (仅当路径被明确指定时)
    if path and not os.path.exists(path):
        print(f"[诊断失败] 错误：在指定的路径 '{path}' 中找不到 ffmpeg.exe 文件。")
        return False

    # 步骤 3: 尝试直接运行 ffmpeg -version 命令，这是最可靠的测试
    try:
        # 使用 subprocess 来捕获命令的输出和错误
        process = subprocess.Popen(
            [final_ffmpeg_path, "-version"],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            creationflags=subprocess.CREATE_NO_WINDOW # 在 Windows 上不创建新窗口
        )
        stdout, stderr = process.communicate()
        
        if process.returncode != 0:
            print("[诊断失败] 错误：执行 'ffmpeg -version' 命令失败。")
            print("FFmpeg 返回的错误信息是:")
            print(stderr.decode('utf-8', errors='ignore'))
            return False
        else:
            print("[诊断成功] 'ffmpeg -version' 命令执行成功！FFmpeg 环境配置正确。")
            print("FFmpeg 版本信息:")
            print(stdout.decode('utf-8', errors='ignore').splitlines()[0])
            return True

    except FileNotFoundError:
        print(f"[诊断失败] 错误：系统找不到命令 '{final_ffmpeg_path}'。")
        print("这通常意味着你既没有在脚本中指定正确路径，也没有将 FFmpeg 的 bin 目录添加到系统环境变量中。")
        return False
    except Exception as e:
        print(f"[诊断失败] 发生未知错误: {e}")
        return False


def convert_audio_with_ffmpeg(m4a_path, mp3_path, ffmpeg_exec_path):
    """
    直接调用 ffmpeg.exe 将 M4A 文件转换为 MP3 文件。
    """
    # 检查输入文件是否存在
    if not os.path.exists(m4a_path):
        print(f"错误：找不到输入的音频文件 '{m4a_path}'")
        return

    print(f"\n准备使用 FFmpeg 进行转换...")
    
    # 构建 ffmpeg 命令
    # -i: 指定输入文件
    # -vn: 忽略视频流
    # -acodec libmp3lame: 使用 LAME 编码器来创建 MP3
    # -ab 192k: 设置比特率为 192kbps
    # -y: 如果输出文件已存在，直接覆盖
    command = [
        ffmpeg_exec_path,
        '-i', m4a_path,
        '-vn',
        '-acodec', 'libmp3lame',
        '-ab', '192k',
        '-y',
        mp3_path
    ]
    
    try:
        print(f"正在执行命令: {' '.join(command)}")
        # 执行命令
        process = subprocess.Popen(
            command,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            creationflags=subprocess.CREATE_NO_WINDOW
        )
        stdout, stderr = process.communicate()

        if process.returncode != 0:
            print("\n转换失败！FFmpeg 返回了错误信息：")
            print(stderr.decode('utf-8', errors='ignore'))
        else:
            print(f"\n转换成功！文件已保存为 '{mp3_path}'")
            print("现在你可以运行你的 C++ 视频播放器了。")

    except Exception as e:
        print(f"\n执行 FFmpeg 命令时发生错误: {e}")


if __name__ == "__main__":
    # 在转换前，先进行环境诊断
    if check_ffmpeg(ffmpeg_path):
        # 如果诊断成功，再执行转换
        final_path = ffmpeg_path if ffmpeg_path else "ffmpeg"
        convert_audio_with_ffmpeg(input_file, output_file, final_path)
    else:
        print("\n由于 FFmpeg 环境诊断失败，转换过程已中止。请根据上面的诊断信息修正你的环境配置。")
