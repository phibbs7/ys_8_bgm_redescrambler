#!/bin/bash
for i in *.ogg; do echo "Descrambling $i..."; ys_8_bgm_redescrambler d $i $i.descrambled.ogg; done;
