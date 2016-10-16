#include "Utilities.h"
#include "Filter.h"
#include "JukeboxPlaylist.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned int
randomNumber(unsigned int seed)
{
    static unsigned int randomNum = 0;

    if (seed == 0) {
        	randomNum = (randomNum * 323727) % 6073;
    } else {
        randomNum = seed;
    }

    return randomNum;
}

FileInfo*
findFileById(FileList* fileList, const char* id)
{
    FileInfo *result = 0;
    unsigned int index = 0;
    FileInfo *curr = fileList->list;

    while (index < fileList->count) {
        if (strcmp(curr->id, id) == 0) {
            result = curr;
            break;
        }
        ++curr;
        ++index;
    }

    return result;
}

Album*
findAlbumById(AlbumList *albumList, const char* id)
{
    Album *result = 0;
    unsigned int index = 0;
    Album *curr = albumList->list;

    while (index < albumList->count) {
        if (strcmp(curr->id, id) == 0) {
            result = curr;
            break;
        }
        ++curr;
        ++index;
    }

    return result;
}

int
fileAlreadyInPlaylist(FileInfo** playlist, const char* id)
{
    int result = -1;
    int index = 0;

    if (playlist != 0 && id != 0) {
        while (playlist[index] != 0) {
            if (strcmp(playlist[index]->id, id) == 0) {
                result = index;
                break;
            }
            ++index;
        }
    }

    return result;
}

void
setupPlaylist(Album* album, FileInfo** playlist, FileList* fileList)
{
    FileInfo *file = 0;
    char *filtered = 0;
    unsigned int index = 0;
    unsigned int playlistIndex = 0;
    unsigned int count = 0;
    int rand = 0;
    int alreadyPresent = 0;

    if (album != 0) {
        if (album->count == 0 && album->algorithm != 0 &&
                strlen(album->algorithm) > 0 && album->songs == 0) {
            filtered = filterPlaylist(&count, album->algorithm, fileList);
            album->count = count;
            album->songs = static_cast<char**>(malloc(count * sizeof(char*)));

            index = 0;
            while (index < count) {
                album->songs[index] =
                    static_cast<char*>(calloc(17, sizeof(char)));
                strcpy(album->songs[index], filtered + (index * 17));
                ++index;
            }

            free(filtered);
        }

        count = album->count;
        if (album->shuffle == 1) {
            while (count > 0) {
                file = 0;
                while (file == 0) {
                    rand = static_cast<int>(randomNumber());
                    index = count / 6073;
                    while (index > 0) {
                        rand += static_cast<int>(randomNumber());
                        --index;
                    }
                    rand %= static_cast<int>(count);

                    index = 0;
                    while (rand >= 0) {
                        alreadyPresent = fileAlreadyInPlaylist(playlist,
                            album->songs[index]);
                        if (alreadyPresent == -1) {
                            --rand;
                        }

                        if (rand < 0) {
                            break;
                        }

                        ++index;
                    }

                    if (index < album->count) {
                        file = findFileById(fileList, album->songs[index]);
                    } else {
                        file = 0;
                    }
                }

                if (file != 0) {
                    playlist[playlistIndex] = file;
                    playlist[playlistIndex + 1] = 0;
                    ++playlistIndex;
                    --count;
                }
            }

            playlist[playlistIndex] = 0;
        } else {
            index = 0;
            while (index < count) {
                file = findFileById(fileList, album->songs[index]);
                if (file != 0) {
                    playlist[playlistIndex] = file;
                    ++playlistIndex;
                }

                ++index;
            }

            playlist[playlistIndex] = 0;
        }
    }
}

char
searchString(const char* first, const char* second)
{
    char result = 0;
    unsigned int index = 0;
    unsigned int sizeFirst = 0;
    unsigned int sizeSecond = 0;

    if (first != 0) {
        sizeFirst = strlen(first);
    }
    if (second != 0) {
        sizeSecond = strlen(second);
    }

    if (sizeSecond > 0) {
        while (index < sizeFirst) {
            if (strncmp(first + index, second, sizeSecond) == 0) {
                result = 1;
                break;
            }
            ++index;
        }
    }

    return result;
}

void
convertUtf8(char* result, const char* str)
{
    char buffer[33];
    unsigned int index = 0;
    unsigned int resultIndex = 0;
    unsigned int stringLength = 0;
    unsigned int newValue;
    unsigned int byte1;
    unsigned int byte2;
    unsigned int byte3;
    unsigned int byte4;

    if (str != 0) {
        stringLength = strlen(str);
    }

    result[0] = '\0';

    while (index < stringLength && resultIndex < 256) {
        byte1 = static_cast<unsigned int>(str[index]);
        if (byte1 > 127) {
            if ((byte1 & 0xf8) == 0xf0 && index + 3 < stringLength) {
                byte2 = static_cast<unsigned int>(str[index + 1]);
                byte3 = static_cast<unsigned int>(str[index + 2]);
                byte4 = static_cast<unsigned int>(str[index + 3]);
                newValue = ((byte1 & 0x07) << 18) | ((byte2 & 0x3f) << 12) |
                    ((byte3 & 0x3f) << 6) | (byte4 & 0x3f);
                snprintf(buffer, 33, "&#%i;", newValue);
                if (resultIndex + strlen(buffer) < 256) {
                    sprintf(result + resultIndex, "%s", buffer);
                }
                resultIndex += strlen(buffer) - 1;
                index += 3;
            } else if ((byte1 & 0xf0) == 0xe0 && index + 2 < stringLength) {
                byte2 = static_cast<unsigned int>(str[index + 1]);
                byte3 = static_cast<unsigned int>(str[index + 2]);
                newValue = ((byte1 & 0x0f) << 12) | ((byte2 & 0x3f) << 6) |
                    (byte3 & 0x3f);
                snprintf(buffer, 33, "&#%i;", newValue);
                if (resultIndex + strlen(buffer) < 256) {
                    sprintf(result + resultIndex, "%s", buffer);
                }
                resultIndex += strlen(buffer) - 1;
                index += 2;
            } else if ((byte1 & 0xe0) == 0xc0 && index + 1 < stringLength) {
                byte2 = static_cast<unsigned int>(str[index + 1]);
                newValue = ((byte1 & 0x1f) << 6) | (byte2 & 0x3f);
                snprintf(buffer, 33, "&#%u;", newValue);
                if (resultIndex + strlen(buffer) < 256) {
                    sprintf(result + resultIndex, "%s", buffer);
                }
                resultIndex += strlen(buffer) - 1;
                index += 1;
            } else {
                result[resultIndex] = static_cast<char>(byte1);
            }
        } else {
            result[resultIndex] = static_cast<char>(byte1);
        }

        ++resultIndex;
        ++index;
    }

    if (resultIndex < 256) {
        result[resultIndex] = '\0';
    } else {
        result[256] = '\0';
    }
}
