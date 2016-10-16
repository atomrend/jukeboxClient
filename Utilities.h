#ifndef UTILITIES_H
#define UTILITIES_H

struct Album;
struct AlbumList;
struct FileInfo;
struct FileList;

unsigned int randomNumber(unsigned int seed = 0);
FileInfo* findFileById(FileList* fileList, const char* id);
Album* findAlbumById(AlbumList *albumList, const char* id);
int fileAlreadyInPlaylist(FileInfo** playlist, const char* id);
void setupPlaylist(Album* album, FileInfo** playlist, FileList* fileList);
char searchString(const char* first, const char* second);
void convertUtf8(char* result, const char* str);

#endif // UTILITIES_H
