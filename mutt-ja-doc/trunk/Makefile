# Makefile for mutt-ja's documents
DOCDIR = /usr/local/doc/mutt
HTMLDIR = ${DOCDIR}/html
SAMPLESDIR = ${DOCDIR}/samples
INSTALL = /usr/bin/install -c

default: usage

usage:
	-@ echo "Usage:  At first, open this Makefile and edit parameters."
	-@ echo "        Then, type 'make install' as root."

install: doc-ja html-ja html-en samplefiles

doc-ja: README.JA-PATCH.ja INSTALL.JA-PATCH.ja \
INSTALL.ja NEWS.ja README.SECURITY.ja README.SSL.ja README.ja \
PGP-Notes.ja.txt applying-patches.ja.txt smime-notes.ja.txt
	${INSTALL} -m 0644 $? ${DOCDIR}/

#doc-en: README.JA-PATCH.en INSTALL.JA-PATCH.en
#	${INSTALL} -m 0644 $? ${DOCDIR}/

html-ja: html/README.JA-PATCH.ja.html html/INSTALL.JA-PATCH.ja.html \
	html/manual-ja-patch.ja.html html/usage-japanese.ja.html
	${INSTALL} -m 0644 $? ${HTMLDIR}/

#html-en: html/README.JA-PATCH.en.html html/INSTALL.JA-PATCH.en.html \
#	 html/manual-ja-patch.en.html html/usage-japanese.en.html
#	${INSTALL} -m 0644 $? ${HTMLDIR}/
html-en: html/manual-ja-patch.en.html
	${INSTALL} -m 0644 $? ${HTMLDIR}/

samplefiles: samples/mutt-en.rc samples/mutt-ja.rc samples/sample.muttrc-tt
	${INSTALL} -m 0644 $? ${SAMPLESDIR}/

