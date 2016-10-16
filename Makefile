MODULE=jukeboxClient
EXECUTABLE=jukeboxClient
DAEMON=jukeboxClient
DEST=/usr

LIBS+= \
    -L/usr/local/lib \
    -L/usr/lib/x86_64-linux-gnu \
    -lexpat \
    -lcurl \
    /usr/local/lib/libfcgi.a \

SOURCES= \
    Filter.c \
    main.c \
    Utilities.c \
    XmlReader.c \

include Makefile.template
