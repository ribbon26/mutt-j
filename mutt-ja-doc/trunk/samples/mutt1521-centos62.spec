Summary: A text mode mail user agent
Name: mutt
Version: 1.5.19
Release: 1%{?dist}001
Epoch: 1
# The entire source code is GPLv2+ except
# pgpewrap.c setenv.c sha1.c wcwidth.c which are Public Domain
License: GPLv2+ and Public Domain
Group: Applications/Internet
Source: ftp://ftp.mutt.org/pub/mutt/devel/mutt-%{version}.tar.gz
Source1: mutt_ldap_query
Patch2: mutt-1.5.13-nodotlock.patch
Patch3: mutt-1.5.19-muttrc.patch
Patch4: mutt-1.5.19-manual.patch
#Patch7: mutt-1.5.19-db47.patch
#Patch11: mutt-1.5.19-wcwidth.0
#Patch12: mutt-1.5.19params.0
#Patch13: mutt-1.5.19-pgp_charsethack.0
#Patch14: mutt-1.5.19-sanitize_ja_char.0
#Patch15: mutt-1.5.19-default_japanese.0
#Patch16: mutt-1.5.19-msgid.0
#Patch17: mutt-1.5.19-delete_prefix.0
Url: http://www.mutt.org/
Requires: mailcap 
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
BuildRequires: /usr/sbin/sendmail
BuildRequires: cyrus-sasl-devel db4-devel krb5-devel ncurses-devel
BuildRequires: libidn-devel gettext
# required to build documentation
BuildRequires: docbook-style-xsl libxslt
BuildRequires: automake autoconf

%description
Mutt is a small but very powerful text-based MIME mail client.  Mutt
is highly configurable, and is well suited to the mail power user with
advanced features like key bindings, keyboard macros, mail threading,
regular expression searches and a powerful pattern matching language
for selecting groups of messages.

%prep
%setup -q
# %patch1 -p1 -b .md5
./prepare -V
# Thou shalt use fcntl, and only fcntl
# %patch2 -p1 -b .nodl
%patch3 -p1 -b .muttrc
%patch4 -p1 -b .manual
#%patch7 -p1 -b .db47
#%patch11 -p1 -b .wcwidth.0
#%patch12 -p1 -b .create_params
#%patch13 -p1 -b .pgp_charsethack
#%patch14 -p1 -b .sanitize_ja_char
#%patch15 -p1 -b .default_japanese
#%patch16 -p1 -b .msgid
#%patch17 -p1 -b .delete_prefix
install -p -m644 %{SOURCE1} mutt_ldap_query

%build
%configure \
	--enable-pop --enable-imap --enable-debug\
	--enable-smtp \
	--with-sasl \
	--with-idn \
	--enable-inodesort \
	--enable-hcache \
	 --enable-cjk-ambiguous-width \
	--with-docdir=%{_docdir}/%{name}-%{version}
make %{?_smp_mflags}

%install
rm -rf $RPM_BUILD_ROOT
make install DESTDIR=$RPM_BUILD_ROOT

# we like GPG here
cat contrib/gpg.rc >> \
	$RPM_BUILD_ROOT%{_sysconfdir}/Muttrc

grep -5 "^color" contrib/sample.muttrc >> \
	$RPM_BUILD_ROOT%{_sysconfdir}/Muttrc

# and we use aspell
cat >> $RPM_BUILD_ROOT%{_sysconfdir}/Muttrc <<EOF
# use aspell
set ispell="%{_bindir}/aspell --mode=email check"

source %{_sysconfdir}/Muttrc.local
EOF

echo "# Local configuration for Mutt." > $RPM_BUILD_ROOT%{_sysconfdir}/Muttrc.local

# remove unpackaged files from the buildroot
rm -f $RPM_BUILD_ROOT%{_sysconfdir}/{*.dist,mime.types}
rm -f $RPM_BUILD_ROOT%{_bindir}/{flea,muttbug}
#rm -f $RPM_BUILD_ROOT%{_mandir}/man1/{flea,muttbug,mutt_dotlock}.1*
rm -f $RPM_BUILD_ROOT%{_mandir}/man1/{flea,muttbug}.1*
rm -f $RPM_BUILD_ROOT%{_mandir}/man5/{mbox,mmdf}.5*

%find_lang %{name}

%clean
rm -rf $RPM_BUILD_ROOT

%files -f %{name}.lang
%defattr(-,root,root)
%config(noreplace) %{_sysconfdir}/Muttrc
%config(noreplace) %{_sysconfdir}/Muttrc.local
%doc COPYRIGHT ChangeLog GPL NEWS README* UPDATING mutt_ldap_query
%doc contrib/*.rc contrib/sample.* contrib/ca-bundle.crt contrib/colors.*
%doc doc/manual.txt doc/smime-notes.txt
%{_bindir}/mutt
%{_bindir}/pgpring
%{_bindir}/pgpewrap
%{_bindir}/smime_keys
%{_bindir}/mutt_dotlock
%{_mandir}/man1/mutt.*
%{_mandir}/man1/mutt_dotlock*
%{_mandir}/man5/muttrc.*

%changelog
* Wed Feb 06 2008 Oota Toshiya <ribbon@users.sourceforge.jp> 1.5.17-1
- apply Japanese patch

* Mon Sep 17 2007 Miroslav Lichvar <mlichvar@redhat.com> 5:1.5.16-4
- fix md5 on big-endian systems

* Tue Aug 28 2007 Miroslav Lichvar <mlichvar@redhat.com> 5:1.5.16-3
- replace md5 implementation
- update license tag

* Wed Jul 11 2007 Miroslav Lichvar <mlichvar@redhat.com> 5:1.5.16-2
- split urlview off, fix requires and description (#226167)

* Mon Jun 11 2007 Miroslav Lichvar <mlichvar@redhat.com> 5:1.5.16-1
- update to 1.5.16

* Mon May 28 2007 Miroslav Lichvar <mlichvar@redhat.com> 5:1.5.14-4
- validate msgid in APOP authentication (CVE-2007-1558)
- fix overflow in gecos field handling (CVE-2007-2683)

* Mon Mar 19 2007 Miroslav Lichvar <mlichvar@redhat.com> 5:1.5.14-3
- fix building

* Mon Mar 19 2007 Miroslav Lichvar <mlichvar@redhat.com> 5:1.5.14-2
- add check_mbox_size configuration variable; if enabled, file size is used
  instead of access time when checking for new mail
- bind delete key to delete-char (#232601)

* Fri Feb 23 2007 Miroslav Lichvar <mlichvar@redhat.com> 5:1.5.14-1
- update to 1.5.14

* Thu Feb 15 2007 Miroslav Lichvar <mlichvar@redhat.com> 5:1.5.13-2.20070212cvs
- update to latest CVS
- enable libidn support (#228158)

* Wed Feb 07 2007 Miroslav Lichvar <mlichvar@redhat.com> 5:1.5.13-1.20070126cvs
- update to 1.5.13, and latest CVS (#168183, #220816)
- spec cleanup

* Wed Dec 06 2006 Miroslav Lichvar <mlichvar@redhat.com> 5:1.4.2.2-5
- use correct fcc folder with IMAP (#217469)
- don't require smtpdaemon, gettext

* Tue Oct 31 2006 Miroslav Lichvar <mlichvar@redhat.com> 5:1.4.2.2-4
- fix POP authentication with latest cyrus-sasl (#212816)

* Tue Oct 24 2006 Miroslav Lichvar <mlichvar@redhat.com> 5:1.4.2.2-3
- fix insecure temp file creation on NFS (#211085, CVE-2006-5297)

* Thu Aug 03 2006 Miroslav Lichvar <mlichvar@redhat.com> 5:1.4.2.2-2
- fix a SASL authentication bug (#199591)

* Mon Jul 17 2006 Miroslav Lichvar <mlichvar@redhat.com> 5:1.4.2.2-1
- update to 1.4.2.2
- fix directories in manual.txt (#162207)
- drop bcc patch (#197408)
- don't package flea

* Wed Jul 12 2006 Jesse Keating <jkeating@redhat.com> - 5:1.4.2.1-7.1
- rebuild

* Thu Jun 29 2006 Miroslav Lichvar <mlichvar@redhat.com> 5:1.4.2.1-7
- fix a buffer overflow when processing IMAP namespace (#197152, CVE-2006-3242)

* Fri Feb 10 2006 Jesse Keating <jkeating@redhat.com> - 5:1.4.2.1-6.2.1
- bump again for double-long bug on ppc(64)

* Tue Feb 07 2006 Jesse Keating <jkeating@redhat.com> - 5:1.4.2.1-6.2
- rebuilt for new gcc4.1 snapshot and glibc changes

* Fri Dec 09 2005 Jesse Keating <jkeating@redhat.com>
- rebuilt

* Wed Nov  9 2005 Bill Nottingham <notting@redhat.com> 5:1.4.2.1-6
- rebuild against new ssl libs

* Thu Oct 27 2005 Bill Nottingham <notting@redhat.com> 5:1.4.2.1-5
- add patch from 1.5 branch to fix SASL logging (#157251, #171528)

* Fri Aug 26 2005 Bill Nottingham <notting@redhat.com> 5:1.4.2.1-3
- add patch from 1.5 branch to fix base64 decoding (#166718)

* Mon Mar  7 2005 Bill Nottingham <notting@redhat.com> 5:1.4.2.1-2
- rebuild against new openssl
- fix build with gcc4

* Thu Jan 27 2005 Bill Nottingham <notting@redhat.com> 5:1.4.2.1-1
- update to 1.4.2.1 (#141007, <moritz@barsnick.net>)
- include a /etc/Muttrc.local for site config (#123109)
- add <f2> as a additional help key for terminals that use <f1> internally
  (#139277)

* Wed Sep 15 2004 Nalin Dahyabhai <nalin@redhat.com> 5:1.4.1-10
- expect the server to prompt for additional auth data if we have some to
  send (#129961, upstream #1845)
- use "pop" as the service name instead of "pop-3" when using SASL for POP,
  per rfc1734

* Fri Aug 13 2004 Bill Nottingham <notting@redhat.com> 5:1.4.1-9
- set write_bcc to no by default (since we ship exim)
- build against sasl2 (#126724)

* Mon Jun 28 2004 Bill Nottingham <notting@redhat.com>
- remove autosplat patch (#116769)

* Tue Jun 15 2004 Elliot Lee <sopwith@redhat.com>
- rebuilt

* Tue Jun  8 2004 Bill Nottingham <notting@redhat.com> 5:1.4.1-7
- link urlview against ncursesw (fixes #125530, indirectly)

* Fri Feb 13 2004 Elliot Lee <sopwith@redhat.com>
- rebuilt

* Tue Jan 27 2004 Bill Nottingham <notting@redhat.com> 5:1.4.1-5
- add patch to fix menu padding (CAN-2004-0078, #109317)

* Mon Aug 18 2003 Bill Nottingham <notting@redhat.com> 5:1.4.1-4
- rebuild against ncursesw

* Tue Jul 22 2003 Nalin Dahyabhai <nalin@redhat.com> 5:1.4.1-3.2
- rebuild

* Mon Jul  7 2003 Bill Nottingham <notting@redhat.com> 5:1.4.1-3
- fix auth to windows KDCs (#98662)

* Wed Jun 04 2003 Elliot Lee <sopwith@redhat.com>
- rebuilt

* Wed Mar 19 2003 Bill Nottingham <notting@redhat.com> 5:1.4.1-1
- update to 1.4.1, fixes buffer overflow in IMAP code

* Wed Jan 22 2003 Tim Powers <timp@redhat.com>
- rebuilt

* Mon Jan 20 2003 Bill Nottingham <notting@redhat.com> 5:1.4-9
- add mailcap requires
- change urlview to htmlview as default browser

* Fri Jan 17 2003 Florian La Roche <Florian.LaRoche@redhat.de>
- change urlview to mozilla as default browser

* Tue Jan 7 2003 Nalin Dahyabhai <nalin@redhat.com> 5:1.4-7
- rebuild

* Mon Dec 2 2002 Bill Nottingham <notting@redhat.com> 5:1.4-6
- ship flea

* Fri Nov 29 2002 Tim Powers <timp@redhat.com> 5:1.4-5
- remove unpackaged files from the buildroot

* Fri Jun 21 2002 Tim Powers <timp@redhat.com>
- automated rebuild

* Fri Jun 14 2002 Bill Nottingham <notting@redhat.com> 1.4-3
- rebuild against new slang

* Wed May 29 2002 Nalin Dahyabhai <nalin@redhat.com> 1.4-2
- forcibly enable SSL and GSSAPI support

* Wed May 29 2002 Bill Nottingham <notting@redhat.com> 1.4-1
- whoa, 1.4.

* Sun May 26 2002 Tim Powers <timp@redhat.com>
- automated rebuild

* Thu May 16 2002 Bill Nottingham <notting@redhat.com>
- autoconf fun

* Wed Jan 09 2002 Tim Powers <timp@redhat.com>
- automated rebuild

* Tue Jan  1 2002 Bill Nottingham <notting@redhat.com>
- update to 1.2.5.1

* Mon Jul 23 2001 Bill Nottingham <notting@redhat.com>
- don't explictly require krb5-libs, etc.; that's what find-requires is for
  (#49780, sort of)

* Sat Jul 21 2001 Tim Powers <timp@redhat.com>
- no more applnk entries, it's cluttering our menus

* Fri Jul 20 2001 Bill Nottingham <notting@redhat.com>
- add slang-devel to buildprereqs (#49531)

* Mon Jun 11 2001 Bill Nottingham <notting@redhat.com>
- add some sample color definitions (#19471)

* Thu May 24 2001 Bill Nottingham <notting@redhat.com>
- fix typo in muttrc.man (#41610)

* Mon May 14 2001 Bill Nottingham <notting@redhat.com>
- use mktemp in muttbug

* Wed May  2 2001 Nalin Dahyabhai <nalin@redhat.com>
- require webclient, not weclient

* Wed May  2 2001 Bill Nottingham <notting@redhat.com>
- build urlview here

* Fri Mar  2 2001 Nalin Dahyabhai <nalin@redhat.com>
- rebuild in new environment

* Tue Feb 13 2001 Bill Nottingham <notting@redhat.com>
- change buildprereq to /usr/sbin/sendmail (it's what it should have been
  originally)
- %%langify

* Tue Feb 13 2001 Michael Stefaniuc <mstefani@redhat.com>
- changed buildprereq to smtpdaemon

* Tue Dec 19 2000 Bill Nottingham <notting@redhat.com>
- rebuild; it's just broken
- fix #13196
- buildprereq sendmail

* Fri Dec 01 2000 Bill Nottingham <notting@redhat.com>
- rebuild because of broken fileutils

* Fri Nov 10 2000 Nalin Dahyabhai <nalin@redhat.com>
- include a sample LDAP query script as a doc file

* Mon Nov  6 2000 Nalin Dahyabhai <nalin@redhat.com>
- patch for imap servers that like to volunteer information after AUTHENTICATE

* Thu Aug 24 2000 Nalin Dahyabhai <nalin@redhat.com>
- rebuild in new environment
- force flock() off and fcntl() on in case defaults change

* Tue Aug  8 2000 Nalin Dahyabhai <nalin@redhat.com>
- enable SSL support

* Fri Aug  4 2000 Bill Nottingham <notting@redhat.com>
- add translation to desktop entry

* Fri Jul 28 2000 Bill Nottingham <notting@redhat.com>
- update to 1.2.5i - fixes IMAP bugs

* Wed Jul 12 2000 Prospector <bugzilla@redhat.com>
- automatic rebuild

* Fri Jul  7 2000 Bill Nottingham <notting@redhat.com>
- 1.2.4i

* Tue Jun 27 2000 Nalin Dahyabhai <nalin@redhat.com>
- rebuild in new environment (release 3)
- adjust GSSAPI build logic

* Thu Jun 22 2000 Bill Nottingham <notting@redhat.com>
- fix MD5 code

* Wed Jun 21 2000 Bill Nottingham <notting@redhat.com>
- update to 1.2.2i

* Mon Jun 19 2000 Trond Eivind Glomsrød <teg@redhat.com>
- use aspell

* Sat Jun 10 2000 Bill Nottingham <notting@redhat.com>
- FHS fixes

* Wed May 10 2000 Bill Nottingham <notting@redhat.com>
- add some files

* Tue May  9 2000 Bill Nottingham <notting@redhat.com>
- update to 1.2i

* Tue Apr  4 2000 Bill Nottingham <notting@redhat.com>
- eliminate explicit krb5-configs dependency

* Wed Mar 22 2000 Bill Nottingham <notting@redhat.com>
- auto<foo> is so much fun.

* Wed Mar 01 2000 Nalin Dahyabhai <nalin@redhat.com>
- make kerberos support conditional at compile-time

* Mon Feb 07 2000 Preston Brown <pbrown@redhat.com>
- wmconfig -> desktop

* Fri Feb  4 2000 Bill Nottingham <notting@redhat.com>
- keep the makefiles from re-running autoheader, automake, etc.

* Thu Feb  3 2000 Nalin Dahyabhai <nalin@redhat.com>
- add forward-ported sasl patch

* Thu Feb  3 2000 Bill Nottingham <notting@redhat.com>
- handle compressed man pages, other cleanups

* Wed Jan 19 2000 Bill Nottingham <notting@redhat.com>
- 1.0.1

* Mon Jan  3 2000 Bill Nottingham <notting@redhat.com>
- add the sample mime.types to /usr/doc

* Sat Jan  1 2000 Bill Nottingham <notting@redhat.com>
- fix an odd y2k issue on receiving mail from ancient clients

* Fri Oct 21 1999 Bill Nottingham <notting@redhat.com>
- one-point-oh.

* Fri Sep 25 1999 Bill Nottingham <notting@redhat.com>
- add a buffer overflow patch

* Tue Aug 31 1999 Bill Nottingham <notting@redhat.com>
- update to 1.0pre2

* Tue Aug 17 1999 Bill Nottingham <notting@redhat.com>
- update to 0.95.7
- require urlview since the default muttrc uses it

* Mon Jun 21 1999 Bill Nottingham <notting@redhat.com>
- get correct manual path the Right Way(tm)
- make it so it uses default colors even if COLORFGBG isn't set

* Mon Jun 14 1999 Bill Nottingham <notting@redhat.com>
- update to 0.95.6

* Mon Apr 26 1999 Bill Nottingham <notting@redhat.com>
- try and make sure $RPM_OPT_FLAGS gets passed through

* Fri Apr 23 1999 Bill Nottingham <notting@redhat.com>
- update to 0.95.5

* Mon Mar 29 1999 Bill Nottingham <notting@redhat.com>
- sed correct doc path into /etc/Muttrc for viewing manual

* Sun Mar 21 1999 Cristian Gafton <gafton@redhat.com> 
- auto rebuild in the new build environment (release 3)

* Thu Mar 18 1999 Bill Nottingham <notting@redhat.com>
- strip binary

* Mon Mar  8 1999 Bill Nottingham <notting@redhat.com>
-  update to 0.95.4 - fixes a /tmp race

* Wed Feb 24 1999 Bill Nottingham <notting@redhat.com>
- the RETURN OF WMCONFIG! Aiyeee!

* Fri Feb 12 1999 Bill Nottingham <notting@redhat.com>
- 0.95.3 - fixes mailcap handling

* Mon Jan  4 1999 Bill Nottingham <notting@redhat.com>
- 0.95.1

* Sat Dec 12 1998 Bill Nottingham <notting@redhat.com>
- 0.95

* Fri Jul 31 1998 Bill Nottingham <notting@redhat.com>
- backport some 0.94.2 security fixes
- fix un-setgid
- update to 0.93.2

* Tue Jul 28 1998 Jeff Johnson <jbj@redhat.com>
- security fix
- update to 0.93.1.
- turn off setgid mail.

* Thu May 07 1998 Prospector System <bugs@redhat.com>
- translations modified for de, fr, tr

* Tue Apr 21 1998 Cristian Gafton <gafton@redhat.com>
- updated to 0.91.1

* Fri Apr 10 1998 Cristian Gafton <gafton@redhat.com>
- updated to mutt-0.89.1

* Thu Oct 16 1997 Otto Hammersmith <otto@redhat.com>
- Updated to mutt 0.85.
- added wmconfig entries.
- removed mime.types

* Mon Sep 1 1997 Donnie Barnes <djb@redhat.com>
- Rebuilt to insure all sources were fresh and patches were clean.

* Wed Aug 6 1997 Manoj Kasichainula <manojk@io.com>
- Initial version for 0.81(e)
