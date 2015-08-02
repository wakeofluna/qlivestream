# qlivestream
A desktop app written using C++/Qt for controlling your livestreams such as Twitch

I find the website interfaces rather sluggish to control, but that's really not so much a problem. What's more a problem is that there are no real options to trigger on changes regarding followers, subscribers or other such events. Sure there are these websites that monitor this for you and allow you to trigger a popup or something, but that is yet another external service and they even take some of your money/income if that's your thing from streaming.

This program aims to let you have basic control over your stream and provide simple triggers and reactions for events. Maybe in the future, support for the chat interface may be added as well.

Planned available triggers:
* Change in followers
* Change in subscribers
* Change in viewers
* Local file changed
* Listening network socket
* (Phrase in chat)

Planned available reactions:
* Write to file/pipe
* Write to socket
* (Message in chat)

This program is not meant to be the most epic in flexibility and extensibility. I don't have that much time :) But I do hope it will be good enough to last a while...

## 3rd party resources
The main framework used is an unmodified version of "Qt" (>= version 5.4), see http://www.qt.io/ for their website. As Qlivestream is licensed under GPLv2, we use Qt under the obligations of the LGPLv2 release of Qt. See http://www.qt.io/licensing/ for their terms. When running the program, please see the "Help|About Qt" menu option for more details.

# Sources
* Sources for Qlivestream can be downloaded at https://github.com/wakeofluna/qlivestream
* Full sources for the open source release of Qt can be downloaded from their website at http://www.qt.io/download-open-source/
