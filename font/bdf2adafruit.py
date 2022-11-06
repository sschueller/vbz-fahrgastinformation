#!/usr/bin/python3

# MIT License.

# Copyright (c) 2022 William Skellenger, Stefan Schüller
#
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the "Software"),
# to deal in the Software without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense,
# and/or sell copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
# OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
# IN THE SOFTWARE.

# Based on version from:
#
# This small script is designed to mostly take a BDF file and convert it to a
# format that can largely be cut/pasted as an Adafruit-format font.
# It was written in an hour or so and did what I needed it to do.
# I used it for one file. Maybe it bombs on other files.
#
# William Skellenger, Feb 2016
# (email: williamj@skellenger.net)
# (Twitter: @skelliam)
#
# Modifications:
#
# Script modified as it did not work with the outputed BDF files I got out of
# "gbdfed" http://sofia.nmsu.edu/~mleisher/Software/gbdfed/. 
#
# - It assumes a fix max width of 16. Zero fills right
# - Does not require that all glyphs are defined
# - Uses FONT name defined in BDF
# - Outputs working .h file
# - Added extended comments (ASCII character)
# - Assumes glyphs 0 to 127 only (hardcoded)
#
# Stefan Schüller, Oct 2022, https://github.com/sschueller
#
# Usage: bdf2adafruit.py somefont.bdf > somefont.h

import sys
import re

myfile = open(sys.argv[1])

processing = 0
getting_rows = 0

chars = []
bitmapData = []

def bytes(integer):
    return divmod(integer, 0x100)

class Glyph:
    font_bb_w = 0
    font_bb_h = 0
    font_bb_offx = 0
    font_bb_offy = 0    
    encoding = -1
    rows = []
    comment = ""
    offset = -1
    width = 0
    height = 0
    advance = 0
    xoffs = 0
    yoffs = 0
    def __init__(self, comment):
        self.comment = comment
        self.rows = []

for line in myfile.readlines():

    if 'FONT ' in line:
        values = line.split()
        fontName = re.sub('[\W_]+', '', values[1]) 

    elif 'FONTBOUNDINGBOX' in line:
        values = line.split()
        font_bb_w = int(values[1])
        font_bb_h = int(values[2])
        font_bb_offx = int(values[3])
        font_bb_offy = int(values[4])

    elif 'STARTCHAR' in line:
        processing = 1
        vals = line.split()
        g = Glyph(vals[1])
        #g.width = 8  #in this example always 8 bits wide

    elif 'ENDCHAR' in line:
        # dataByteCompressed = 0
        # dataByteCompressedIndex = 8
        g.height = len(bitmapData)
        for value in bitmapData:
            # split data into high and low hex
            high, low = bytes(value)
            g.rows.append(high)
            g.rows.append(low)

        # y offset from top
        # if glyph does not fill the whole box set the offset from the top
        if ((len(g.rows) / 2) + g.decent) < (font_bb_h + font_bb_offy):
            g.yoffs =  (font_bb_h + font_bb_offy) - ((len(g.rows) / 2) + g.decent)

        chars.append(g)  #append the completed glyph into list
        processing = 0
        getting_rows = 0
        bitmapData.clear()

    if processing:
        if 'ENCODING' in line:
            # The word ENCODING followed by one of the following forms:
            # <n> − the glyph index, that is, a positive integer representing the character code used to
            # access the glyph in X requests, as defined by the encoded character set given by the
            # CHARSET_REGISTRY-CHARSET_ENCODING font properties for XLFD conforming
            # fonts. If these XLFD font properties are not defined, the encoding scheme is font-depen-
            # dent.            
            vals = line.split()
            g.encoding = int(vals[1])
        elif 'DWIDTH' in line:
            # The word DWIDTH followed by the width in x and y of the character in device units. Like the
            # SWIDTH, this width information is a vector indicating the position of the next character’s ori-
            # gin relative to the origin of this character. Note that the DWIDTH of a given ‘‘hand-tuned’’
            # WYSIWYG glyph may deviate slightly from its ideal device-independent width given by
            # SWIDTH in order to improve its typographic characteristics on a display. The DWIDTH y
            # value should always be zero for a standard X font.
            vals = line.split()
            g.advance = int(vals[1])  #cursor advance seems to be the first number in DWIDTH
        elif 'BBX' in line:
            # The word BBX followed by the width in x (BBw), height in y (BBh), and x and y displacement
            # (BBox, BBoy) of the lower left corner from the origin of the character.
            vals = line.split()
            g.xoffs = int(vals[3]) + 1
            g.yoffs = 0
            g.decent = (int(vals[4]))
            # g.advance = (int(vals[1]) + 1)  #x bounding box + 1
            g.width = int(vals[1])
        elif 'BITMAP' in line:
            getting_rows = 1
        elif getting_rows:
            # h lines of hex-encoded bitmap, padded on the right with zeros to the nearest byte (that is, multi-
            # ple of 8).
            #g.rows.append(int(line, 16))  #append pixel rows into glyph's list of rows
            # zero pad right
            bitmapData.append(int(line.rstrip().ljust(4, '0'), 16))

print

print ("#include <Adafruit_GFX.h>\n\n", end='')

print ("const uint8_t %sBitmap[] PROGMEM = {	\n" %(fontName))

i=0
for char in chars:
    char.offset = i
    if char.rows:
        print ("\t", end='')

    for row in char.rows:
        print ("0x%02X," %(row), end = ''),
        i+=1

    if char.rows:
        print ("\t/* 0x%02X %s '%s' */" %(char.encoding, char.comment, chr(char.encoding)))

print ("};\n\n")
print ("const GFXglyph %sGlyph[] PROGMEM = {\n" %(fontName))

print("/* offset, bit-width, bit-height, advance cursor, x offset, y offset */")

for char in chars:
    # offset, bit-width, bit-height, advance cursor, x offset, y offset
    character = chr(char.encoding)

    if not char.rows:
        character = "No Bitmap Defined"

    print ("\t{ %d, %d, %d, %d, %d, %d }, /* 0x%02X %s '%s' */" %(
            char.offset, 16, char.height,
            char.advance, char.xoffs, char.yoffs,
            char.encoding, char.comment, character))

print ("};\n\n")
print ("const GFXfont %s PROGMEM = {" %(fontName))
print ("\t(uint8_t *)%sBitmap," %(fontName))
print ("\t(GFXglyph *)%sGlyph, " %(fontName))
print ("\t00,127, %d //ASCII start, ASCII stop,y Advance" %(font_bb_h))
print ("};\n")
