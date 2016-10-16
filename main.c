#include <fcgi_config.h>
#include <fcgi_stdio.h>
#include "HtmlText.h"
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include "Utilities.h"
#include "XmlReader.h"

#define PLAYLIST_PATH "/etc/jukeboxPlaylist.xml"
#define SOCKET_PATH "/var/run/jukebox.socket"
#define BUFFER_SIZE 32
#define PLAY "00"
#define PAUSE "01"
#define NEXT_SONG "02"
#define PREV_SONG "03"
#define SET_ALBUM "04" // 04=id
#define SET_SONG "05" // 05=id
#define SET_SHUFFLE "06" // 06(?)=id
#define SET_REPEAT "07" // 07(?)=id
#define GET_SEED "08" // seed=?
#define GET_ALBUM "09" // album=id
#define GET_SONG "10" // song=id
#define GET_FRAME "11" // frame=?
#define GET_STATUS "12" // status=?

/* AJAX commands
    getSongListForAlbum=<id>
    getAlbumArt=<id>
    getRepeatForAlbum=<id>
    getShuffleForAlbum=<id>
    setCurrentAlbum=<id>
    toggleRepeat=<0 or 1>
    toggleShuffle=<0 or 1>
    playPauseClicked=<unused parameter>
    previousSongClicked=<unused parameter>
    nextSongClicked=<unused parameter>
    getPlayingTitle=<unused parameter>
    getPlayingAlbum=<unused parameter>
    getPlayingArtist=<unused parameter>
    getPlayingTime=<unused parameter>
    getStatus=<unused parameter>
*/

int
main(int /*argc*/, char** /*argv*/)
{
    FileList fileList;
    AlbumList albumList;
    char convertedUtf8[257];
    char buffer[BUFFER_SIZE + 1];
    char id[17];
    struct sockaddr_un outputAddress_;
    FileInfo *fileInfoPtr = 0;
    Album *albumPtr = 0;
    Album *currAlbum = 0;
    FileInfo *playingSong = 0;
    Album *playingAlbum = 0;
    FileInfo **playlist = 0;
    char *queryString = 0;
    char *userAgent = 0;
    unsigned int index = 0;
    unsigned int mainButtonSize = 100;
    unsigned int lineHeight = 0;
    unsigned int fontSizePercentage = 0;
    unsigned int buttonSize = 0;
    unsigned int headerHeight = 0;
    unsigned int shuffleRepeatSize = 0;
    int socketHandle = 0;
    int receiveSize = 0;
    char paddingSize = 0;
    char status = 0;
//FILE *handle = 0; // DBG handle = fopen("/home/web/remote.txt", "a");

    convertedUtf8[256] = '\0';
    buffer[BUFFER_SIZE] = '\0';
    id[16] = '\0';

    memset(&outputAddress_, 0, sizeof(outputAddress_));
    outputAddress_.sun_family = AF_UNIX;
    strcpy(outputAddress_.sun_path, SOCKET_PATH);

    socketHandle = socket(PF_UNIX, SOCK_STREAM, 0);

    if (readPlaylistFile(PLAYLIST_PATH, &fileList, &albumList) == 0 &&
            socketHandle >= 0) {
        playlist = static_cast<FileInfo**>(
            malloc((fileList.count + 1) * sizeof(FileInfo*)));
    }

    if (playlist != 0) {
        while (FCGI_Accept() >= 0) {
            queryString = getenv("QUERY_STRING");

            // getSongListForAlbum
            if (searchString(queryString, "getSongListForAlbum=") == 1) {
                printf("content-type: text/html\r\n\r\n");

                memset(id, '\0', 17);
                strncpy(id, queryString + 20, 16);
                id[16] = '\0';

                albumPtr = findAlbumById(&albumList, id);
                if (albumPtr != 0) {
                    socketHandle = socket(PF_UNIX, SOCK_STREAM, 0);
                    if (connect(socketHandle,
                            reinterpret_cast<struct sockaddr*>(&outputAddress_),
                            sizeof(struct sockaddr_un)) == 0) {
                        // GET_SEED
                        snprintf(buffer, BUFFER_SIZE, GET_SEED);
                        receiveSize = write(socketHandle, buffer,
                            strlen(buffer) + 1);

                        receiveSize = read(socketHandle, buffer, BUFFER_SIZE);
                        buffer[receiveSize] = '\0';
                        randomNumber(
                            static_cast<unsigned int>(atoi(buffer + 5)));
                        close(socketHandle);
                    }

                    setupPlaylist(albumPtr, playlist, &fileList);

                    paddingSize = 0;
                    mainButtonSize = albumPtr->count;
                    while (mainButtonSize > 0) {
                        ++paddingSize;
                        mainButtonSize /= 10;
                    }

                    receiveSize = 0;
                    userAgent = getenv("HTTP_USER_AGENT");
                    if (searchString(userAgent, "Nexus 7") == 1) {
                        receiveSize = 1;
                    }

                    index = 0;
                    while (playlist[index] != 0) {
                        fileInfoPtr = playlist[index];
                        convertUtf8(convertedUtf8, fileInfoPtr->title);

                        printf(SONG_TEXT,
                            fileInfoPtr->id,
                            paddingSize,
                            index + 1,
                            receiveSize == 1 ? 25 : 15,
                            receiveSize == 1 ? 25 : 15,
                            fileInfoPtr->id,
                            convertedUtf8);

                        ++index;
                    }
                }

            // getAlbumArt
            } else if (searchString(queryString, "getAlbumArt=") == 1) {
                printf("content-type: text/html\r\n\r\n");

                memset(id, '\0', 17);
                strncpy(id, queryString + 12, 16);
                id[16] = '\0';

                fileInfoPtr = findFileById(&fileList, id);
                albumPtr = findAlbumById(&albumList, id);
                if (fileInfoPtr != 0 || albumPtr != 0) {
                    if (fileInfoPtr != 0 && fileInfoPtr->albumArt != 0) {
                        if (strlen(fileInfoPtr->albumArt) > 0) {
                            printf("%s", fileInfoPtr->albumArt + 9);
                        } else {
                            printf("/albumArt/blank.jpg");
                        }
                    } else if (albumPtr != 0 && albumPtr->albumArt != 0) {
                        if (strlen(albumPtr->albumArt) > 0) {
                            printf("%s", albumPtr->albumArt + 9);
                        } else {
                            printf("/albumArt/blank.jpg");
                        }
                    } else {
                        printf("/albumArt/blank.jpg");
                    }
                } else {
                    printf("/albumArt/blank.jpg");
                }

            // getRepeatForAlbum
            } else if (searchString(queryString, "getRepeatForAlbum=") == 1) {
                printf("content-type: text/html\r\n\r\n");

                memset(id, '\0', 17);
                strncpy(id, queryString + 18, 16);
                id[16] = '\0';

                albumPtr = findAlbumById(&albumList, id);
                if (albumPtr != 0 && albumPtr->repeat == 1) {
                    printf("1");
                } else {
                    printf("0");
                }

            // getShuffleForAlbum
            } else if (searchString(queryString, "getShuffleForAlbum=") == 1) {
                printf("content-type: text/html\r\n\r\n");

                memset(id, '\0', 17);
                strncpy(id, queryString + 19, 16);
                id[16] = '\0';

                albumPtr = findAlbumById(&albumList, id);
                if (albumPtr != 0 && albumPtr->shuffle == 1) {
                    printf("1");
                } else {
                    printf("0");
                }

            // setCurrentAlbum
            } else if (searchString(queryString, "setCurrentAlbum=") == 1) {
                printf("content-type: text/html\r\n\r\n");

                memset(id, '\0', 17);
                strncpy(id, queryString + 16, 16);
                id[16] = '\0';

                albumPtr = findAlbumById(&albumList, id);
                if (albumPtr != 0) {
                    currAlbum = albumPtr;
                }

            // toggleRepeat
            } else if (searchString(queryString, "toggleRepeat=") == 1) {
                printf("content-type: text/html\r\n\r\n");

                memset(id, '\0', 17);
                strncpy(id, queryString + 13, 16);
                id[16] = '\0';

                if (currAlbum != 0) {
                    if (id[0] == '1') {
                        currAlbum->repeat = 1;
                        printf("1");
                    } else {
                        currAlbum->repeat = 0;
                        printf("0");
                    }
                } else {
                    printf("0");
                }

            // toggleShuffle
            } else if (searchString(queryString, "toggleShuffle=") == 1) {
                printf("content-type: text/html\r\n\r\n");

                memset(id, '\0', 17);
                strncpy(id, queryString + 14, 16);
                id[16] = '\0';

                if (currAlbum != 0) {
                    if (id[0] == '1') {
                        currAlbum->shuffle = 1;
                        printf("1");
                    } else {
                        currAlbum->shuffle = 0;
                        printf("0");
                    }
                } else {
                    printf("0");
                }

            // playPauseClicked
            } else if (searchString(queryString, "playPauseClicked=") == 1) {
                printf("content-type: text/html\r\n\r\n");

                socketHandle = socket(PF_UNIX, SOCK_STREAM, 0);
                if (connect(socketHandle,
                        reinterpret_cast<struct sockaddr*>(&outputAddress_),
                        sizeof(struct sockaddr_un)) == 0) {
                    // GET_STATUS
                    snprintf(buffer, BUFFER_SIZE, GET_STATUS);
                    receiveSize = write(socketHandle, buffer,
                        strlen(buffer) + 1);

                    receiveSize = read(socketHandle, buffer, BUFFER_SIZE);
                    buffer[receiveSize] = '\0';
                    close(socketHandle);

                    status = 0;
                    if (strcmp(buffer, "status=1") == 0) {
                        status = 1;
                    } else if (strcmp(buffer, "status=2") == 0) {
                        status = 2;
                    }
                }

                if (status == 1) {
                    // Pause the playback.
                    socketHandle = socket(PF_UNIX, SOCK_STREAM, 0);
                    if (connect(socketHandle,
                            reinterpret_cast<struct sockaddr*>(&outputAddress_),
                            sizeof(struct sockaddr_un)) == 0) {
                        // PAUSE
                        snprintf(buffer, BUFFER_SIZE, PAUSE);
                        receiveSize = write(socketHandle, buffer,
                            strlen(buffer) + 1);
                        close(socketHandle);
                        status = 2;
                    }
                } else if (status == 2) {
                    // Unpause the play back.
                    // If the current album had changed, play that new album.

                    socketHandle = socket(PF_UNIX, SOCK_STREAM, 0);
                    if (connect(socketHandle,
                            reinterpret_cast<struct sockaddr*>(&outputAddress_),
                            sizeof(struct sockaddr_un)) == 0) {
                        // GET_ALBUM
                        snprintf(buffer, BUFFER_SIZE, GET_ALBUM);
                        receiveSize = write(socketHandle, buffer,
                            strlen(buffer) + 1);

                        receiveSize = read(socketHandle, buffer, BUFFER_SIZE);
                        buffer[receiveSize] = '\0';
                        close(socketHandle);

                        memset(id, '\0', 17);
                        strncpy(id, buffer + 6, 16);
                        id[16] = '\0';

                        playingAlbum = findAlbumById(&albumList, id);
                    }

                    if (playingAlbum != 0 && currAlbum != playingAlbum &&
                            currAlbum != 0) {
                        socketHandle = socket(PF_UNIX, SOCK_STREAM, 0);
                        if (connect(socketHandle,
                                reinterpret_cast<struct sockaddr*>(
                                    &outputAddress_),
                                sizeof(struct sockaddr_un)) == 0) {
                            // SET_ALBUM
                            snprintf(buffer, BUFFER_SIZE, "%s=%s", SET_ALBUM,
                                currAlbum->id);
                            receiveSize = write(socketHandle, buffer,
                                strlen(buffer) + 1);
                            close(socketHandle);
                        }
                    }

                    socketHandle = socket(PF_UNIX, SOCK_STREAM, 0);
                    if (connect(socketHandle,
                            reinterpret_cast<struct sockaddr*>(&outputAddress_),
                            sizeof(struct sockaddr_un)) == 0) {
                        // PLAY
                        snprintf(buffer, BUFFER_SIZE, PLAY);
                        receiveSize = write(socketHandle, buffer,
                            strlen(buffer) + 1);
                        close(socketHandle);
                        status = 1;
                    }
                } else {
                    if (currAlbum != 0) {
                        socketHandle = socket(PF_UNIX, SOCK_STREAM, 0);
                        if (connect(socketHandle,
                                reinterpret_cast<struct sockaddr*>(
                                    &outputAddress_),
                                sizeof(struct sockaddr_un)) == 0) {
                            // SET_SHUFFLE
                            snprintf(buffer, BUFFER_SIZE, "%s(%u)=%s",
                               SET_SHUFFLE,  currAlbum->shuffle, currAlbum->id);
                            receiveSize = write(socketHandle, buffer,
                                strlen(buffer) + 1);
                            close(socketHandle);
                        }

                        socketHandle = socket(PF_UNIX, SOCK_STREAM, 0);
                        if (connect(socketHandle,
                                reinterpret_cast<struct sockaddr*>(
                                    &outputAddress_),
                                sizeof(struct sockaddr_un)) == 0) {
                            // SET_REPEAT
                            snprintf(buffer, BUFFER_SIZE, "%s(%u)=%s",
                                SET_REPEAT, currAlbum->repeat, currAlbum->id);
                            receiveSize = write(socketHandle, buffer,
                                strlen(buffer) + 1);
                            close(socketHandle);
                        }

                        socketHandle = socket(PF_UNIX, SOCK_STREAM, 0);
                        if (connect(socketHandle,
                                reinterpret_cast<struct sockaddr*>(
                                    &outputAddress_),
                                sizeof(struct sockaddr_un)) == 0) {
                            // SET_ALBUM
                            snprintf(buffer, BUFFER_SIZE, "%s=%s", SET_ALBUM,
                                currAlbum->id);
                            receiveSize = write(socketHandle, buffer,
                                strlen(buffer) + 1);
                            close(socketHandle);
                        }
                    }

                    socketHandle = socket(PF_UNIX, SOCK_STREAM, 0);
                    if (connect(socketHandle,
                            reinterpret_cast<struct sockaddr*>(&outputAddress_),
                            sizeof(struct sockaddr_un)) == 0) {
                        // PLAY
                        snprintf(buffer, BUFFER_SIZE, PLAY);
                        receiveSize = write(socketHandle, buffer,
                            strlen(buffer) + 1);
                        close(socketHandle);
                        status = 1;
                    }
                }

            // previousSongClicked
            } else if (searchString(queryString, "previousSongClicked=") == 1) {
                printf("content-type: text/html\r\n\r\n");

                socketHandle = socket(PF_UNIX, SOCK_STREAM, 0);
                if (connect(socketHandle,
                        reinterpret_cast<struct sockaddr*>(&outputAddress_),
                        sizeof(struct sockaddr_un)) == 0) {
                    // PREV_SONG
                    snprintf(buffer, BUFFER_SIZE, PREV_SONG);
                    receiveSize = write(socketHandle, buffer,
                        strlen(buffer) + 1);
                    close(socketHandle);
                }

            // nextSongClicked
            } else if (searchString(queryString, "nextSongClicked=") == 1) {
                printf("content-type: text/html\r\n\r\n");

                socketHandle = socket(PF_UNIX, SOCK_STREAM, 0);
                if (connect(socketHandle,
                        reinterpret_cast<struct sockaddr*>(&outputAddress_),
                        sizeof(struct sockaddr_un)) == 0) {
                    // NEXT_SONG
                    snprintf(buffer, BUFFER_SIZE, NEXT_SONG);
                    receiveSize = write(socketHandle, buffer,
                        strlen(buffer) + 1);
                    close(socketHandle);
                }

            // getPlayingTitle
            } else if (searchString(queryString, "getPlayingTitle=") == 1) {
                printf("content-type: text/html\r\n\r\n");

                socketHandle = socket(PF_UNIX, SOCK_STREAM, 0);
                if (connect(socketHandle,
                        reinterpret_cast<struct sockaddr*>(&outputAddress_),
                        sizeof(struct sockaddr_un)) == 0) {
                    // GET_SONG
                    snprintf(buffer, BUFFER_SIZE, GET_SONG);
                    receiveSize = write(socketHandle, buffer,
                        strlen(buffer) + 1);

                    receiveSize = read(socketHandle, buffer, BUFFER_SIZE);
                    buffer[receiveSize] = '\0';
                    close(socketHandle);

                    memset(id, '\0', 17);
                    strncpy(id, buffer + 5, 16);
                    id[16] = '\0';

                    playingSong = findFileById(&fileList, id);
                }

                if (playingSong != 0) {
                    printf("%s", playingSong->title);
                } else {
                    printf("&nbsp;");
                }

            // getPlayingAlbum
            } else if (searchString(queryString, "getPlayingAlbum=") == 1) {
                printf("content-type: text/html\r\n\r\n");

                socketHandle = socket(PF_UNIX, SOCK_STREAM, 0);
                if (connect(socketHandle,
                        reinterpret_cast<struct sockaddr*>(&outputAddress_),
                        sizeof(struct sockaddr_un)) == 0) {
                    // GET_SONG
                    snprintf(buffer, BUFFER_SIZE, GET_SONG);
                    receiveSize = write(socketHandle, buffer,
                        strlen(buffer) + 1);

                    receiveSize = read(socketHandle, buffer, BUFFER_SIZE);
                    buffer[receiveSize] = '\0';
                    close(socketHandle);

                    memset(id, '\0', 17);
                    strncpy(id, buffer + 5, 16);
                    id[16] = '\0';

                    playingSong = findFileById(&fileList, id);
                }

                if (playingSong != 0) {
                    printf("%s", playingSong->album);
                } else {
                    printf("&nbsp;");
                }

            // getPlayingArtist
            } else if (searchString(queryString, "getPlayingArtist=") == 1) {
                printf("content-type: text/html\r\n\r\n");

                socketHandle = socket(PF_UNIX, SOCK_STREAM, 0);
                if (connect(socketHandle,
                        reinterpret_cast<struct sockaddr*>(&outputAddress_),
                        sizeof(struct sockaddr_un)) == 0) {
                    // GET_SONG
                    snprintf(buffer, BUFFER_SIZE, GET_SONG);
                    receiveSize = write(socketHandle, buffer,
                        strlen(buffer) + 1);

                    receiveSize = read(socketHandle, buffer, BUFFER_SIZE);
                    buffer[receiveSize] = '\0';
                    close(socketHandle);

                    memset(id, '\0', 17);
                    strncpy(id, buffer + 5, 16);
                    id[16] = '\0';

                    playingSong = findFileById(&fileList, id);
                }

                if (playingSong != 0) {
                    printf("%s", playingSong->artist);
                } else {
                    printf("&nbsp;");
                }

            // getPlayingSongId
            } else if (searchString(queryString, "getPlayingSongId=") == 1) {
                printf("content-type: text/html\r\n\r\n");

                socketHandle = socket(PF_UNIX, SOCK_STREAM, 0);
                if (connect(socketHandle,
                        reinterpret_cast<struct sockaddr*>(&outputAddress_),
                        sizeof(struct sockaddr_un)) == 0) {
                    // GET_SONG
                    snprintf(buffer, BUFFER_SIZE, GET_SONG);
                    receiveSize = write(socketHandle, buffer,
                        strlen(buffer) + 1);

                    receiveSize = read(socketHandle, buffer, BUFFER_SIZE);
                    buffer[receiveSize] = '\0';
                    close(socketHandle);

                    memset(id, '\0', 17);
                    strncpy(id, buffer + 5, 16);
                    id[16] = '\0';

                    playingSong = findFileById(&fileList, id);
                }

                if (playingSong != 0) {
                    printf("%s", playingSong->id);
                } else {
                    printf("0");
                }

            // getPlayingAlbumId
            } else if (searchString(queryString, "getPlayingAlbumId=") == 1) {
                printf("content-type: text/html\r\n\r\n");

                socketHandle = socket(PF_UNIX, SOCK_STREAM, 0);
                if (connect(socketHandle,
                        reinterpret_cast<struct sockaddr*>(&outputAddress_),
                        sizeof(struct sockaddr_un)) == 0) {
                    // GET_ALBUM
                    snprintf(buffer, BUFFER_SIZE, GET_ALBUM);
                    receiveSize = write(socketHandle, buffer,
                        strlen(buffer) + 1);

                    receiveSize = read(socketHandle, buffer, BUFFER_SIZE);
                    buffer[receiveSize] = '\0';
                    close(socketHandle);

                    memset(id, '\0', 17);
                    strncpy(id, buffer + 6, 16);
                    id[16] = '\0';

                    playingAlbum = findAlbumById(&albumList, id);
                }

                if (playingAlbum != 0) {
                    printf("%s", playingAlbum->id);
                } else {
                    printf("0");
                }

            // getCurrentFrame
            } else if (searchString(queryString, "getCurrentFrame=") == 1) {
                printf("content-type: text/html\r\n\r\n");

                socketHandle = socket(PF_UNIX, SOCK_STREAM, 0);
                if (connect(socketHandle,
                        reinterpret_cast<struct sockaddr*>(&outputAddress_),
                        sizeof(struct sockaddr_un)) == 0) {
                    // GET_FRAME
                    snprintf(buffer, BUFFER_SIZE, GET_FRAME);
                    receiveSize = write(socketHandle, buffer,
                        strlen(buffer) + 1);

                    receiveSize = read(socketHandle, buffer, BUFFER_SIZE);
                    buffer[receiveSize] = '\0';
                    close(socketHandle);
                    printf("%s", buffer + 6);
                } else {
                    printf("0");
                }

            // getNumberOfFrames
            } else if (searchString(queryString, "getNumberOfFrames=") == 1) {
                printf("content-type: text/html\r\n\r\n");

                socketHandle = socket(PF_UNIX, SOCK_STREAM, 0);
                if (connect(socketHandle,
                        reinterpret_cast<struct sockaddr*>(&outputAddress_),
                        sizeof(struct sockaddr_un)) == 0) {
                    // GET_SONG
                    snprintf(buffer, BUFFER_SIZE, GET_SONG);
                    receiveSize = write(socketHandle, buffer,
                        strlen(buffer) + 1);

                    receiveSize = read(socketHandle, buffer, BUFFER_SIZE);
                    buffer[receiveSize] = '\0';
                    close(socketHandle);

                    memset(id, '\0', 17);
                    strncpy(id, buffer + 5, 16);
                    id[16] = '\0';

                    playingSong = findFileById(&fileList, id);
                }

                if (playingSong != 0) {
                    printf("%u", playingSong->frameCount);
                } else {
                    printf("0");
                }

            // getStatus
            } else if (searchString(queryString, "getStatus=") == 1) {
                printf("content-type: text/html\r\n\r\n");

                socketHandle = socket(PF_UNIX, SOCK_STREAM, 0);
                if (connect(socketHandle,
                        reinterpret_cast<struct sockaddr*>(&outputAddress_),
                        sizeof(struct sockaddr_un)) == 0) {
                    // GET_STATUS
                    snprintf(buffer, BUFFER_SIZE, GET_STATUS);
                    receiveSize = write(socketHandle, buffer,
                        strlen(buffer) + 1);

                    receiveSize = read(socketHandle, buffer, BUFFER_SIZE);
                    buffer[receiveSize] = '\0';
                    close(socketHandle);
                    printf("%s", buffer + 7);
                } else {
                    printf("0");
                }

            // Display Html page.
            } else {
                userAgent = getenv("HTTP_USER_AGENT");
                receiveSize = 0;
                if (searchString(userAgent, "Nexus 7") == 1) {
                    receiveSize = 1;
                }

                queryString =
                    static_cast<char*>(calloc(albumList.count * 256,
                        sizeof(char)));
                albumPtr = albumList.list;
                index = 0;
                while (index < albumList.count) {
                    convertUtf8(convertedUtf8, albumPtr->name);
                    snprintf(queryString + strlen(queryString),
                        albumList.count * 256 - strlen(queryString),
                        ALBUM_TEXT,
                        albumPtr->id,
                        receiveSize == 1 ? 25 : 15,
                        receiveSize == 1 ? 25 : 15,
                        albumPtr->id,
                        convertedUtf8);
                    ++albumPtr;
                    ++index;
                }

                status = 0;
                socketHandle = socket(PF_UNIX, SOCK_STREAM, 0);
                if (connect(socketHandle,
                        reinterpret_cast<struct sockaddr*>(&outputAddress_),
                        sizeof(struct sockaddr_un)) == 0) {
                    // GET_STATUS
                    snprintf(buffer, BUFFER_SIZE, GET_STATUS);
                    receiveSize = write(socketHandle, buffer,
                        strlen(buffer) + 1);

                    receiveSize = read(socketHandle, buffer, BUFFER_SIZE);
                    buffer[receiveSize] = '\0';
                    close(socketHandle);

                    if (strcmp(buffer, "status=1") == 0) {
                        status = 1;
                    } else if (strcmp(buffer, "status=2") == 0) {
                        status = 2;
                    }
                }

                socketHandle = socket(PF_UNIX, SOCK_STREAM, 0);
                if (connect(socketHandle,
                        reinterpret_cast<struct sockaddr*>(&outputAddress_),
                        sizeof(struct sockaddr_un)) == 0) {
                    // GET_ALBUM
                    snprintf(buffer, BUFFER_SIZE, GET_ALBUM);
                    receiveSize = write(socketHandle, buffer,
                        strlen(buffer) + 1);

                    receiveSize = read(socketHandle, buffer, BUFFER_SIZE);
                    buffer[receiveSize] = '\0';
                    close(socketHandle);

                    memset(id, '\0', 17);
                    strncpy(id, buffer + 6, 16);
                    id[16] = '\0';

                    playingAlbum = findAlbumById(&albumList, id);
                }

                socketHandle = socket(PF_UNIX, SOCK_STREAM, 0);
                if (connect(socketHandle,
                        reinterpret_cast<struct sockaddr*>(&outputAddress_),
                        sizeof(struct sockaddr_un)) == 0) {
                    // GET_SONG
                    snprintf(buffer, BUFFER_SIZE, GET_SONG);
                    receiveSize = write(socketHandle, buffer,
                        strlen(buffer) + 1);

                    receiveSize = read(socketHandle, buffer, BUFFER_SIZE);
                    buffer[receiveSize] = '\0';
                    close(socketHandle);

                    memset(id, '\0', 17);
                    strncpy(id, buffer + 5, 16);
                    id[16] = '\0';

                    playingSong = findFileById(&fileList, id);
                }

                mainButtonSize = 100;
                lineHeight = 30;
                fontSizePercentage = 100;
                buttonSize = 50;
                headerHeight = 100;
                shuffleRepeatSize = 33;
                receiveSize = 0;
                if (searchString(userAgent, "Nexus 7") == 1) {
                    receiveSize = 1;
                    mainButtonSize = 150;
                    lineHeight = 60;
                    fontSizePercentage = 150;
                    buttonSize = 75;
                    headerHeight = 150;
                    shuffleRepeatSize = 50;
                }

                printf(HTML_TEXT,
                    lineHeight, /* selected album text height */
                    lineHeight, /* selected album line height */
                    fontSizePercentage, /* selected album font size percentage */
                    lineHeight, /* unselected album text height */
                    lineHeight, /* unselected album line height */
                    fontSizePercentage, /* unselected album font size percentage */
                    lineHeight, /* selected song text height */
                    lineHeight, /* selected song line height */
                    fontSizePercentage, /* selected song font size percentage */
                    lineHeight, /* unselected song text height */
                    lineHeight, /* unselected song line height */
                    fontSizePercentage, /* unselected song font size percentage */
                    headerHeight, /* height of the header */
                    buttonSize, /* width of prev song button */
                    buttonSize, /* height of prev song button */
                    buttonSize, /* width of play/pause button */
                    buttonSize, /* height of play/pause button */
                    buttonSize, /* width of next song button */
                    buttonSize, /* height of next song button */
                    receiveSize == 1 ? 30 : 20, /* title div height */
                    fontSizePercentage, /* song title font size percentage */
                    receiveSize == 1 ? 30 : 20, /* title div height */
                    fontSizePercentage, /* song album font size percentage */
                    receiveSize == 1 ? 30 : 20, /* title div height */
                    fontSizePercentage, /* song artist font size percentage */
                    receiveSize == 1 ? 30 : 20, /* title div height */
                    fontSizePercentage, /* current time font size percentage */
                    receiveSize == 1 ? "none" : "block", /* album shown or hidden */
                    shuffleRepeatSize, /* width of repeat button */
                    shuffleRepeatSize, /* height of repeat button */
                    shuffleRepeatSize, /* width of shuffle button */
                    shuffleRepeatSize, /* height of shuffle button */
                    headerHeight, /* height of the header */
                    queryString /* html list of albums */);

                free(queryString);
            }
        } /* while (FCGI_Accept() >= 0) */
    } /* if (readPlaylistFile(PLAYLIST_PATH, &fileList, &albumList) == 0) */

    return 0;
}
