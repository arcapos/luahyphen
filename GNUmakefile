SRCS=		luahyphen.c
MODULE=		hyphen

CFLAGS+=	$(shell grep -q hnj_hyphen_load_file /usr/include/hyphen.h \
		&& echo "-DHAVE_LOAD_FILE")

LDADD+=		-lhyphen
FILESDIR=	/usr/share/arcapos/hyphen
INSTFILES=	de_ch.dic de_de.dic en_us.dic

include $(MKDIR)lua.module.mk
