video2jpeg jpg, use ffmpeg

video to jpeg jpg, mp4, h264 and so.

Video one frame convert to jpeg jpg.

for lua or openresty .

use ffmpeg libs

test.lua:

	local video2jpeg = require("video2jpeg")

	video2jpeg.out2jpeg(0.1, "./a.mp4", "b.jpg")

function out2jpeg args:

1. time point, float type.

2. video source path.

3. to jpeg file path.

return:

0 is succeed.

minus is error.

