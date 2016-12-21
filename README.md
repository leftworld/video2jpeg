video2jpeg

video to jpeg 

Video one frame convert to jpeg .

for lua or openresty .

use ffmpeg libs

test.lua:

	local video2mpeg = require("video2mpeg")

	video2mpeg.out2jpeg(0.1, "./a.mp4", "b.jpg")

function out2jpeg args:

1. time point, float type.

2. video source path.

3. to jpeg file path.

return:

0 is succeed.

minus is error.

