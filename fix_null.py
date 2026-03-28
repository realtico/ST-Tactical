with open("src/video/test_video.c", "r", encoding="utf-8") as f:
    text = f.read()

text = text.replace("'\x00'", "'\\0'")

with open("src/video/test_video.c", "w", encoding="utf-8") as f:
    f.write(text)

