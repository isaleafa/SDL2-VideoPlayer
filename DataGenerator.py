import cv2
import os
import sys
import struct
import config

# --- 配置 ---
# 视频文件路径
video_path = config.VIDEO_PATH
# 输出帧数据文件
output_data_file = config.OUTPUT_DATA_FILE
# --- 配置结束 ---

def process_video(input_path, output_path):
    """
    读取视频文件，将其每一帧缩放到适合的尺寸，并将带有元数据文件头的
    像素数据写入二进制文件。
    """
    # 1. 打开视频文件
    cap = cv2.VideoCapture(input_path)
    if not cap.isOpened():
        print(f"错误：无法打开视频文件 '{input_path}'")
        return

    # 2. 获取视频的原始信息
    frame_count = int(cap.get(cv2.CAP_PROP_FRAME_COUNT))
    source_width = cap.get(cv2.CAP_PROP_FRAME_WIDTH)
    source_height = cap.get(cv2.CAP_PROP_FRAME_HEIGHT)
    # 获取视频的帧率
    fps = cap.get(cv2.CAP_PROP_FPS)

    if source_width == 0 or source_height == 0:
        print("错误：无法获取视频的尺寸信息。")
        cap.release()
        return

    # 3. 计算最终用于渲染的宽度和高度
    render_width = 150
    # 根据原始视频的宽高比，计算出成比例的高度
    render_height = int((source_height / source_width) * render_width)

    print("--- 视频信息 ---")
    print(f"原始尺寸: {int(source_width)}x{int(source_height)}")
    print(f"处理后尺寸: {render_width}x{render_height}")
    print(f"总帧数: {frame_count}")
    print(f"帧率 (FPS): {fps:.2f}") # 新增打印帧率
    print("------------------")

    # 4. 写入文件
    try:
        with open(output_path, 'wb') as f:
            # 我们依次写入宽度、高度、帧数和帧率
            header = struct.pack('IIIf', render_width, render_height, frame_count, fps)
            f.write(header)
            
            print("\n开始处理视频帧...")
            # --- 循环处理每一帧 ---
            for i in range(frame_count):
                ret, frame = cap.read()
                if not ret:
                    print(f"\n警告：提前到达视频末尾，只处理了 {i} 帧。")
                    break

                # 缩放帧到我们计算出的尺寸
                resized_frame = cv2.resize(frame, (render_width, render_height))
                # 将 BGR 图像转换为 RGB
                frame_rgb = cv2.cvtColor(resized_frame, cv2.COLOR_BGR2RGB)
                # 将帧数据转换为字节流并直接写入
                f.write(frame_rgb.tobytes())
                
                # 打印动态更新的进度条
                progress = (i + 1) / frame_count
                bar_length = 40
                bar = '█' * int(bar_length * progress) + '-' * (bar_length - int(bar_length * progress))
                print(f'\r处理进度: |{bar}| {progress:.1%}', end='')
                sys.stdout.flush()

        print("\n\n处理完成！")

    except Exception as e:
        print(f"\n处理过程中发生错误: {e}")
    finally:
        cap.release()

if __name__ == "__main__":
    process_video(video_path, output_data_file)
