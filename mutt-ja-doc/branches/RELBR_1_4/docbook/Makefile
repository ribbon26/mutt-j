HTML2TXT = w3m -dump -T text/html
XML2HTML = ./xsl/docbook2xhtml
WWWDIR = /home/taki/www/mutt/1.4/doc
XSL = xsl/docbook-wrapper.xsl

all : prepare text

prepare: xsl/docbook-wrapper.xsl

xsl/docbook-wrapper.xsl:
	./prepare

text : prepare README.JA-PATCH.ja INSTALL.JA-PATCH.ja \
manual-ja-patch.ja.txt manual-ja-patch.en.txt usage-japanese.ja.txt

html : prepare README.JA-PATCH.ja.html INSTALL.JA-PATCH.ja.html \
manual-ja-patch.ja.html manual-ja-patch.en.html usage-japanese.ja.html

%.html : %.xml
	$(XML2HTML) $(XSL) $<

%.txt : %.html
	$(HTML2TXT) $< > $@

%.ja : %.ja.html
	$(HTML2TXT) $< > $@

%.en : %.en.html
	$(HTML2TXT) $< > $@

dist : dist-text dist-html

dist-text : README.JA-PATCH.ja INSTALL.JA-PATCH.ja
	cp $^ ../

dist-html : INSTALL.JA-PATCH.ja.html manual-ja-patch.en.html \
usage-japanese.ja.html README.JA-PATCH.ja.html manual-ja-patch.ja.html
	test -d ../html || (cd ..;mkdir html);cp $^ ../html/

upload : 
	cp *.txt *.ja *.html $(WWWDIR)/

clean :
	rm -f *.txt *.ja *.html xsl/docbook-wrapper.xsl

clean-all : clean
	rm -rf ../README.JA-PATCH.ja ../INSTALL.JA-PATCH.ja ../html

