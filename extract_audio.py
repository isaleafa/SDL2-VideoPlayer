# 导入 moviepy 中的 VideoFileClip 类
from moviepy.editor import VideoFileClip
import os

# --- 配置 ---
# 输入的视频文件路径
video_path = 'frame.mp4'
# 输出的音频文件路径（与C++代码中的要求一致）
output_audio_path = './run/audio.mp3'
# --- 配置结束 ---

def extract_audio(input_path, output_path):
    """
    从视频文件中提取音频并保存为MP3。

    :param input_path: 输入视频文件的路径 (e.g., 'video.mp4')
    :param output_path: 输出音频文件的路径 (e.g., 'audio.mp3')
    """
    # 检查视频文件是否存在
    if not os.path.exists(input_path):
        print(f"错误：找不到视频文件 '{input_path}'")
        return

    print(f"正在从 '{input_path}' 中提取音频...")

    try:
        # 使用 with 语句加载视频文件，可以确保处理后自动关闭文件
        with VideoFileClip(input_path) as video_clip:
            # 获取视频的音轨
            audio_clip = video_clip.audio

            # 将音轨写入新的音频文件
            # codec='mp3' 明确指定编码器
            # bitrate="192k" 可以保证不错的音质
            audio_clip.write_audiofile(output_path, codec='mp3', bitrate="192k")

        print(f"音频提取成功！已保存为 '{output_path}'")

    except Exception as e:
        print(f"提取过程中发生错误: {e}")
        print("请确保你已经安装了 moviepy (pip install moviepy) 并且 FFmpeg 可用。")

# 当直接运行这个脚本时，执行下面的代码
if __name__ == "__main__":
    extract_audio(video_path, output_audio_path)