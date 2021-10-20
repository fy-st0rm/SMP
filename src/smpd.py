import sys
import os
import pafy
from pytube import Playlist

_help = """
	[HELP MENU]
	-l [link]	[Generates actual link of the youtube]
"""


if len(sys.argv) <= 2:
	print(_help)
	exit(1)

cmd = sys.argv[1]
url = sys.argv[2]

if cmd == "-l":
	if "playlist" in url:
		urls = Playlist(url)
		for i in urls:
			track = pafy.new(i)
			title = track.title
			audio_url = track.audiostreams[-1].url

			print(title + "]" + audio_url + "\n")

	else:
		track = pafy.new(url)
		title = track.title
		audio_url = track.audiostreams[-1].url

		print(title + "]" + audio_url + "\n")


