#ifndef JUKEBOX_PLAYLIST_H
#define JUKEBOX_PLAYLIST_H

typedef struct FileInfo
{
    char* id;
    char* filePath;
    char* title;
    char* album;
    char* artist;
    char* genre;
    char* comment;
    char* albumArt;
    unsigned int year;
    unsigned int frameCount;
    char favorite;

    char padding_[7];
} FileInfo;

typedef struct Album
{
    char* id;
    char* name;
    char* algorithm;
    char** songs;
    char* albumArt;
    unsigned int count;
    char shuffle;
    char repeat;

    char padding_[2];
} Album;

typedef struct FileList
{
    FileInfo* list;
    unsigned int count;

    char padding_[4];
} FileList;

typedef struct AlbumList
{
    Album* list;
    unsigned int count;

    char padding_[4];
} AlbumList;

#endif /* JUKEBOX_PLAYLIST_H */
