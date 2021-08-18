import subprocess
import shutil
import os

# Generate every frames 
for i in range(0, 1000): 
    subprocess.run([
        "./fractal", 
        "./output/image" + str(f'{i:04}') + ".bmp", 
        str(0.190), #0.189
        str(1.035 + i * 0.000025), #0.889        1.035 1.055
        "1.5", 
        "50",
        str(1.3 - i * 0.001299)
    ])
    if i % 100 == 0:
        print(i)

# convert the frames to a video
subprocess.run(
    [
        "ffmpeg", 
        "-r", "40", 
        "-f", "image2",
        "-s", "300*300",
        "-start_number", "1",
        "-i", "./output/image%04d.bmp", 
        "-vframes", "893",
        "-vf", "crop=trunc(iw/2)*2:trunc(ih/2)*2",
        "-vcodec", "libx264", 
        "-crf", "40", 
        "-pix_fmt", "yuv420p",
        "-y",
        "test.mp4"
    ],
    stderr=subprocess.DEVNULL, stdout=subprocess.DEVNULL
)

# copy the vidéo on windows file system
src = r'./test.mp4'
dst =  r'/mnt/s/progra/fractal3/test.mp4'
shutil.copyfile(src, dst)