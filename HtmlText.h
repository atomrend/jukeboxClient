#ifndef HTML_TEXT_H
#define HTML_TEXT_H

#define SONG_TEXT "\
<div id=\"%s\" onclick=\"songSelection(this)\" class=\"unselectedSong\">\n\
    %0*i\n\
    <svg width=\"%upx\" height=\"%upx\" viewBox=\"-6 -6 472 656\">\n\
        <use id=\"chevron_%s\" xlink:href=\"#noChevron\"/>\n\
    </svg>\n\
    %s\n\
</div>\n"

#define ALBUM_TEXT "\
<div id=\"%s\" onclick=\"albumSelection(this)\" class=\"unselectedAlbum\">\n\
    <svg width=\"%upx\" height=\"%upx\" viewBox=\"-6 -6 472 656\">\n\
        <use id=\"chevron_%s\" xlink:href=\"#noChevron\"/>\n\
    </svg>\n\
    %s\n\
</div>\n"

#define HTML_TEXT "\
content-type: text/html\r\n\r\n\
<html>\n\
<head>\n\
<title>Jukebox</title>\n\
<link rel=\"icon\" type=\"image/svg+xml\" sizes=\"any\" \
href=\"/favicon/jukebox.svg\">\n\
<style>\n\
div.albumList {\n\
    border: 1px solid #ffffff;\n\
    clip: auto;\n\
    overflow: auto;\n\
    height: 99%%;\n\
    width: 49%%;\n\
    white-space: nowrap;\n\
    display: inline-block;\n\
    background-color: #000000;\n\
}\n\n\
div.songList {\n\
    border: 1px solid #ffffff;\n\
    clip: auto;\n\
    overflow: auto;\n\
    height: 99%%;\n\
    width: 50%%;\n\
    white-space: nowrap;\n\
    display: inline-block;\n\
    background-color: #000000;\n\
}\n\n\
div.selectedAlbum {\n\
    background-color: #ffffff;\n\
    height: %upx;\n\
    line-height: %upx;\n\
    font-size: %u%%;\n\
    color: #000000;\n\
}\n\n\
div.unselectedAlbum {\n\
    background-color: none;\n\
    height: %upx;\n\
    line-height: %upx;\n\
    font-size: %u%%;\n\
    color: #ffffff;\n\
    cursor: pointer;\n\
}\n\n\
div.selectedSong {\n\
    background-color: #ffffff;\n\
    height: %upx;\n\
    line-height: %upx;\n\
    font-size: %u%%;\n\
    color: #000000;\n\
}\n\n\
div.unselectedSong {\n\
    background-color: none;\n\
    height: %upx;\n\
    line-height: %upx;\n\
    font-size: %u%%;\n\
    color: #ffffff;\n\
    cursor: pointer;\n\
}\n\n\
progress {\n\
    border: 1px solid #ffffff;\n\
    background: #000000;\n\
}\n\
progress[value]::-webkit-progress-bar {\n\
    background-color: #000000;\n\
}\n\
progress[value]::-webkit-progress-value {\n\
    background-color: #ffffff;\n\
}\n\
progress[value]::-moz-progress-bar {\n\
    background-color: #ffffff;\n\
}\n\n\
</style>\n</head>\n\
<body bgcolor=\"#000000\" text=\"#ffffff\" onload=\"onLoad()\" \
style=\"height: 100%%; margin:0; padding:0;\">\n\n\
<script type=\"text/javascript\">\n\
var selectedAlbum = null;\n\
var selectedSong = null;\n\
var currentAlbumChevron = null;\n\
var currentSongChevron = null;\n\
var status = 0;\n\n\
function requestData(request) {\n\
    var xmlDoc = new XMLHttpRequest();\n\
    var response = \"\";\
    if (xmlDoc != null) {\n\
        xmlDoc.onreadystatechange = function() {\n\
            if (xmlDoc.readyState == 4 && xmlDoc.status == 200) {\n\
                response = xmlDoc.responseText;\n\
            }\n\
        };\n\
        xmlDoc.open(\"GET\", \"/jukebox?\" + request, false);\n\
        xmlDoc.send(null);\n\
    }\n\n\
    return response;\n\
}\n\n\
function updateStatus() {\n\
    var title = document.getElementById(\"currentTitle\");\n\
    var album = document.getElementById(\"currentAlbum\");\n\
    var artist = document.getElementById(\"currentArtist\");\n\
    var time = document.getElementById(\"currentTime\");\n\
    var progress = document.getElementById(\"timeProgress\");\n\
    var playSvg = document.getElementById(\"PlayPauseSvg\");\n\
    var playUse = document.getElementById(\"PlayPauseUse\");\n\
    var frame = parseInt(requestData(\"getCurrentFrame=0\"));\n\
    var songLength = parseInt(requestData(\"getNumberOfFrames=0\"));\n\
    var chevron = document.getElementById(\"chevron_\" + \
requestData(\"getPlayingAlbumId=0\"));\n\n\
    status = requestData(\"getStatus=0\");\n\n\
    if (playSvg != null && playUse != null) {\n\
        if (status == 1) {\n\
            playSvg.setAttribute(\"viewBox\", \"0 0 137 147\");\n\
            playUse.setAttribute(\"xlink:href\", \"#pauseSvg\");\n\
        } else {\n\
            playSvg.setAttribute(\"viewBox\", \"115.9 98 106 121\");\n\
            playUse.setAttribute(\"xlink:href\", \"#playSvg\");\n\
        }\n\
    }\n\n\
    if (title != null) {\n\
        if (status == 1 || status == 2) {\n\
            title.innerHTML = requestData(\"getPlayingTitle=0\");\n\
        } else {\n\
            title.innerHTML = \"&nbsp;\";\n\
        }\n\
    }\n\n\
    if (album != null) {\n\
        if (status == 1 || status == 2) {\n\
            album.innerHTML = requestData(\"getPlayingAlbum=0\");\n\
        } else {\n\
            album.innerHTML = \"&nbsp;\";\n\
        }\n\
    }\n\n\
    if (artist != null) {\n\
        if (status == 1 || status == 2) {\n\
            artist.innerHTML = requestData(\"getPlayingArtist=0\");\n\
        } else {\n\
            artist.innerHTML = \"&nbsp;\";\n\
        }\n\
    }\n\n\
    if (time != null) {\n\
        time.innerHTML = frameToString(frame) + \" / \" + \
frameToString(songLength);\n\
    }\n\n\
    if (progress != null) {\n\
        progress.setAttribute(\"value\", frame);\n\
        progress.setAttribute(\"max\", songLength);\n\
    }\n\n\
    if (currentAlbumChevron != null && currentAlbumChevron != chevron) {\n\
        currentAlbumChevron.setAttribute(\"xlink:href\", \"#noChevron\");\n\
    }\n\
    if (chevron != null) {\n\
        chevron.setAttribute(\"xlink:href\", \"#chevron\");\n\
        currentAlbumChevron = chevron;\n\
    }\n\n\
    chevron = document.getElementById(\"chevron_\" + \
requestData(\"getPlayingSongId=0\"));\n\
    if (currentSongChevron != null && currentSongChevron != chevron) {\n\
        currentSongChevron.setAttribute(\"xlink:href\", \"#noChevron\");\n\
    }\n\
    if (chevron != null) {\n\
        chevron.setAttribute(\"xlink:href\", \"#chevron\");\n\
        currentSongChevron = chevron;\n\
    }\n\
}\n\n\
function albumSelection(item) {\n\
    var songList = document.getElementById(\"songList\");\n\
    var repeat = document.getElementById(\"repeatSvg\");\n\
    var shuffle = document.getElementById(\"shuffleSvg\");\n\
    var albumArt = document.getElementById(\"albumArt\");\n\
    var chevron = null;\n\
    var playingSong = requestData(\"getPlayingSongId=0\");\n\n\
    if (selectedAlbum != null) {\n\
        selectedAlbum.className = \"unselectedAlbum\";\n\
    }\n\
    item.className = \"selectedAlbum\";\n\
    selectedAlbum = item;\n\
    requestData(\"setCurrentAlbum=\" + selectedAlbum.id);\n\n\
    if (songList != null) {\n\
        songList.innerHTML = requestData(\"getSongListForAlbum=\" + item.id);\n\
        chevron = document.getElementById(\"chevron_\" + playingSong);\n\
        if (chevron != null) {\n\
            chevron.setAttribute(\"xlink:href\", \"#chevron\");\n\
        }\n\
    }\n\n\
    if (repeat != null) {\n\
        if (requestData(\"getRepeatForAlbum=\" + item.id) == \"1\") {\n\
            repeat.setAttribute(\"fill\", \"white\");\n\
        } else {\n\
            repeat.setAttribute(\"fill\", \"black\");\n\
        }\n\
    }\n\n\
    if (shuffle != null) {\n\
        if (requestData(\"getShuffleForAlbum=\" + item.id) == \"1\") {\n\
            shuffle.setAttribute(\"fill\", \"white\");\n\
        } else {\n\
            shuffle.setAttribute(\"fill\", \"black\");\n\
        }\n\
    }\n\n\
    if (albumArt != null) {\n\
        albumArt.src = requestData(\"getAlbumArt=\" + item.id);\n\
    }\n\
}\n\n\
function songSelection(item) {\n\
    var albumArt = document.getElementById(\"albumArt\");\n\n\
    if (selectedSong != null) {\n\
        selectedSong.className = \"unselectedSong\";\n\
    }\n\
    item.className = \"selectedSong\";\n\
    selectedSong = item;\n\n\
    if (albumArt != null) {\n\
        albumArt.src = requestData(\"getAlbumArt=\" + item.id);\n\
    }\n\
}\n\n\
function onPrevSongClick() {\n\
    requestData(\"previousSongClicked=0\");\n\
    setTimeout(updateStatus, 250);\n\
}\n\n\
function onPlayPauseClick() {\n\
    requestData(\"playPauseClicked=0\");\n\
    setTimeout(updateStatus, 250);\n\
}\n\n\
function onNextSongClick() {\n\
    requestData(\"nextSongClicked=0\");\n\
    setTimeout(updateStatus, 250);\n\
}\n\n\
function onRepeatClick() {\n\
    var repeatSvg = document.getElementById(\"repeatSvg\");\n\
    var response = \"0\";\n\
    var repeat;\n\n\
    if (repeatSvg != null) {\n\
        repeat = repeatSvg.getAttribute(\"fill\");\n\
        if (repeat == \"white\") {\n\
            repeat = 0;\n\
        } else {\n\
            repeat = 1;\n\
        }\n\n\
        response = requestData(\"toggleRepeat=\" + repeat);\n\
        if (response == \"1\") {\n\
            repeatSvg.setAttribute(\"fill\", \"white\");\n\
            repeatSvg.setAttribute(\"stroke\", \"black\");\n\
        } else {\n\
            repeatSvg.setAttribute(\"fill\", \"black\");\n\
            repeatSvg.setAttribute(\"stroke\", \"white\");\n\
        }\n\
    }\n\
}\n\n\
function onShuffleClick() {\n\
    var shuffleSvg = document.getElementById(\"shuffleSvg\");\n\
    var response = \"0\";\n\
    var shuffle;\n\n\
    if (shuffleSvg != null) {\n\
        shuffle = shuffleSvg.getAttribute(\"fill\");\n\
        if (shuffle == \"white\") {\n\
            shuffle = 0;\n\
        } else {\n\
            shuffle = 1;\n\
        }\n\n\
        response = requestData(\"toggleShuffle=\" + shuffle);\n\
        if (response == \"1\") {\n\
            shuffleSvg.setAttribute(\"fill\", \"white\");\n\
            shuffleSvg.setAttribute(\"stroke\", \"black\");\n\
        } else {\n\
            shuffleSvg.setAttribute(\"fill\", \"black\");\n\
            shuffleSvg.setAttribute(\"stroke\", \"white\");\n\
        }\n\
    }\n\
}\n\n\
function onLoad() {\n\
    selectAlbum(requestData(\"getPlayingAlbumId=0\"));\n\
    updateStatus();\n\
    setInterval(onTimer, 1000);\n\
}\n\n\
function frameToString(frame) {\n\
    var msecs = frame * 1152000.0 / 44100;\n\
    var minutes = Math.floor(msecs / 60000);\n\
    var seconds = Math.ceil(msecs / 1000) %% 60;\n\
    var result = minutes;\n\
    result += \":\";\n\
    if (seconds < 10) {\n\
        result += \"0\";\n\
    }\n\
    result += seconds;\n\
    return result;\n\
}\n\n\
function onTimer() {\n\
    if (status == 1) {\n\
        updateStatus();\n\
    }\n\
}\n\n\
function selectAlbum(id) {\n\
    var album = document.getElementById(id);\n\
    var albumArt = document.getElementById(\"albumArt\");\n\
    var songList = document.getElementById(\"songList\");\n\
    var repeat = document.getElementById(\"repeatSvg\");\n\
    var shuffle = document.getElementById(\"shuffleSvg\");\n\
    var response;\n\
    var chevron;\n\n\
    if (album != null) {\n\
        if (selectedAlbum != null) {\n\
            selectedAlbum.className = \"unselectedAlbum\";\n\
        }\n\
        selectedAlbum = album;\n\
        selectedAlbum.className = \"selectedAlbum\";\n\
        album.scrollIntoView();\n\n\
        if (songList != null) {\n\
            songList.innerHTML = requestData(\"getSongListForAlbum=\" + id);\n\
            response = requestData(\"getPlayingSongId=0\");\n\
            chevron = document.getElementById(\"chevron_\" + response);\n\
            if (chevron != null) {\n\
                chevron.setAttribute(\"xlink:href\", \"#chevron\");\n\
            }\n\n\
            response = document.getElementById(response);\n\
            if (response != null) {\n\
                response.scrollIntoView();\n\
            }\n\
        }\n\n\
        if (repeat != null) {\n\
            if (requestData(\"getRepeatForAlbum=\") == \"1\") {\n\
                repeat.setAttribute(\"fill\", \"white\");\n\
            } else {\n\
                repeat.setAttribute(\"fill\", \"black\");\n\
            }\n\
        }\n\n\
        if (shuffle != null) {\n\
            if (requestData(\"getShuffleForAlbum=\" + id) == \"1\") {\n\
                shuffle.setAttribute(\"fill\", \"white\");\n\
            } else {\n\
                shuffle.setAttribute(\"fill\", \"black\");\n\
            }\n\
        }\n\n\
        if (albumArt != null) {\n\
            albumArt.src = requestData(\"getAlbumArt=\" + id);\n\
        }\n\
    }\n\
}\n\n\
</script>\n\n\
<svg display=\"none\">\n<defs>\n\
<path id=\"fastForward\" fill=\"white\" stroke=\"black\" stroke-width=\"40\" \
d=\"m 45,-115 q -19,-19 -32,-13 -13,6 \
-13,32 v 1472 q 0,26 13,32 13,6 32,-13 L 755,685 q 8,-8 13,-19 \
v 710 q 0,26 13,32 13,6 32,-13 l 710,-710 q 8,-8 13,-19 v 678 \
q 0,26 19,45 19,19 45,19 h 128 q 26,0 45,-19 19,-19 19,-45 V \
-64 q 0,-26 -19,-45 -19,-19 -45,-19 h -128 q -26,0 -45,19 \
-19,19 -19,45 v 678 q -5,-10 -13,-19 L 813,-115 q -19,-19 \
-32,-13 -13,6 -13,32 v 710 q -5,-10 -13,-19 z\"/>\n\
<path id=\"playSvg\" fill=\"white\" stroke=\"black\" stroke-width=\"4\" \
d=\"M213.555,150.652l-82.214-47.949\
c-7.492-4.374-13.535-0.877-13.493,7.789l0.421,95.174\
c0.038,8.664,6.155,12.191,13.669,7.851l81.585-47.103\
C221.029,162.082,221.045,155.026,213.555,150.652z\"/>\n\
<rect id=\"noPlaySvg\" x=\"115.9\" y=\"98\" width=\"106\" height=\"121\" \
fill=\"none\"/>\n\
<path id=\"pauseSvg\" fill=\"white\" stroke=\"black\" stroke-width=\"4\" \
d=\"M 5 15 a 10 -10 0 0 1 10 -10 h 25 a \
10 10 0 0 1 10 10 v 120 a -10 10 0 0 1 -10 10 h -25 a -10 -10 \
0 0 1 -10 -10 v -120 M 85 15 a 10 -10 0 0 1 10 -10 h 25 a \
10 10 0 0 1 10 10 v 120 a -10 10 0 0 1 -10 10 h -25 a -10 -10 \
0 0 1 -10 -10 v -120 z\"/>\n\
<path id=\"repeatSvg\" fill=\"black\" stroke=\"white\" stroke-width=\"4\" \
d=\"M314.161,215.951h-31.598v18.377h31.598c1.672,0,3.032,1.36,\
3.032,3.032v53.002c0,1.671-1.36,3.031-3.032,3.031H197.839\
c-1.672,0-3.032-1.36-3.032-3.031V237.36c0-1.671,1.36-3.032,\
3.032-3.032h22.901v13.859c0,0.665,0.355,1.281,0.932,1.613\
c0.288,0.166,0.61,0.249,0.931,0.249c0.322,0,0.643-0.083,\
0.932-0.249l39.921-23.048c0.577-0.333,0.932-0.948,0.932-1.614\
c0-0.665-0.355-1.281-0.932-1.613l-39.921-23.048\
c-0.576-0.333-1.287-0.333-1.863,0\
c-0.576,0.333-0.932,0.948-0.932,1.613v13.859h-22.901\
c-11.805,0-21.409,9.604-21.409,21.409v53.002\
c0,11.805,9.604,21.409,21.409,21.409h116.322c11.805,0,\
21.409-9.604,21.409-21.409V237.36\
C335.57,225.555,325.966,215.951,314.161,215.951z\"/>\n\
<path id=\"shuffleSvg\" fill=\"black\" stroke=\"white\" stroke-width=\"12\" \
d=\"M533.333,133.333L400,0v100c-68.109,0-120.183,16.254-159.192,49.692c-2.734,\
2.344-5.356,4.739-7.896,7.173c14.333,19.877,25.373,40.374,35.298,59.943\
c25.167-30.417,61.486-50.142,131.791-50.142v100l0,0v100\
c-112.731,0-138.086-50.708-170.185-114.908\
c-17.868-35.735-36.344-72.687-70.621-102.067C120.183,116.254,\
68.11,100,0,100v66.667c112.732,0,138.086,50.708,170.185,114.907c17.868,35.737,\
36.344,72.688,70.621,102.068c39.011,33.438,91.085,49.691,159.193,\
49.691v100L533.333,400L400,266.667L533.333,133.333z M0,366.667v66.666\
c68.11,0,120.182-16.253,159.193-49.691c2.734-2.343,5.355-4.739,\
7.896-7.175c-14.333-19.876-25.374-40.373-35.298-59.942C106.625,346.942,70.306,\
366.667,0,366.667z\"/>\n\
<path id=\"chevron\" fill=\"white\" stroke=\"black\" stroke-width=\"24\" \
d=\"M 460 322 L 138 0 0 138 184 322 0 506 138 644 z\"/>\n\
<rect id=\"noChevron\" fill=\"none\" x=\"-6\" y=\"-6\" width=\"472\" \
height=\"656\"/>\n\
</defs>\n</svg>\n\n\
<div style=\"height: %upx; background-color: #000000;\">\n\
    <table width=\"100%%\" height=\"100%%\">\n\
        <tr>\n\
            <td style=\"padding: 10px; cursor: pointer;\" \
onclick=\"onPrevSongClick();\">\n\
                <svg width=\"%u\" height=\"%u\" viewBox=\"-30 -180 1870 1870\">\n\
                    <use xlink:href=\"#fastForward\" \
transform=\"rotate(180 895 640)\"/>\n\
                </svg>\n\
            </td>\n\
            <td style=\"padding: 10px;cursor: pointer;\" \
onclick=\"onPlayPauseClick();\">\n\
                <svg id=\"PlayPauseSvg\" width=\"%u\" height=\"%u\" \
viewBox=\"115.9 98 106 121\">\n\
                    <use id=\"PlayPauseUse\" xlink:href=\"#playSvg\"/>\n\
                </svg>\n\
            </td>\n\
            <td onclick=\"onNextSongClick();\" \
style=\"padding: 10px; cursor: pointer;\">\n\
                <svg width=\"%u\" height=\"%u\" viewBox=\"-30 -180 1870 1870\">\n\
                    <use xlink:href=\"#fastForward\"/>\n\
                </svg>\n\
            </td>\n\n\
            <td width=\"100%%\">\n\
                <div style=\"height: 100%%; border: 1px solid white;\">\n\
                    <div id=\"currentTitle\" \
style=\"text-align: center; height: %u%%; overflow: hidden; font-size: %u%%\">\
&nbsp;</div>\n\
                    <div id=\"currentAlbum\" \
style=\"text-align: center; height: %u%%; overflow: hidden; font-size: %u%%\">\
&nbsp;</div>\n\
                    <div id=\"currentArtist\" \
style=\"text-align: center; height: %u%%; overflow: hidden; font-size: %u%%\">\
&nbsp;</div>\n\
                    <div id=\"currentTime\" \
style=\"text-align: center; height: %u%%; overflow: hidden; font-size: %u%%\">\
&nbsp;</div>\n\
                    <progress id=\"timeProgress\" max=\"10000\" value=\"0\" \
style=\"width:99%%;height:10px;\"></progress>\n\
                </div>\n\
            </td>\n\n\
            <td>\n\
                <img id=\"albumArt\" src=\"/albumArt/blank.jpg\" \
width=\"100px\" height=\"100px\" style=\"display: %s;\">\n\
            </td>\n\n\
            <td>\n\
                <table height=\"100%%\">\n\
                    <tr>\n\
                        <td style=\"cursor: pointer; padding: 7px;\" \
onclick=\"onRepeatClick();\">\n\
                            <svg width=\"%upx\" height=\"%upx\" \
viewBox=\"170 195 170 120\">\n\
                                <use xlink:href=\"#repeatSvg\"/>\n\
                            </svg>\n\
                        </td>\n\
                    </tr>\n\
                    <tr>\n\
                        <td style=\"cursor: pointer; padding: 7px;\" \
onclick=\"onShuffleClick();\">\n\
                            <svg width=\"%upx\" height=\"%upx\" \
viewBox=\"0 0 533.333 533.333\">\n\
                                <use xlink:href=\"#shuffleSvg\"/>\n\
                            </svg>\n\
                        </td>\n\
                    </tr>\n\
                </table>\n\
            </td>\n\
        </tr>\n\
    </table>\n\
</div>\n\
<div style=\"height: calc(99%% - %upx - 5px); \
background-color: #000000; margin-top: 10px;\">\n\
<div class=\"albumList\">%s</div>\n\
<div id=\"songList\" class=\"songList\"></div>\n\
</div>\n\
</body>\n\
</html>"

#endif // HTML_TEXT_H
