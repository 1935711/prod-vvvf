#!/bin/bash
ffmpeg -i audio.wav -f u8 -ar 20k -ac 1 audio.bin
