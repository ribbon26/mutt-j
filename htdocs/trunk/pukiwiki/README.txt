NAME

    PukiWiki - ��ͳ�˥ڡ������ɲá�������Խ��Ǥ���Web�ڡ�������PHP������ץ�

        PukiWiki 1.4.5
        Copyright (C) 2001-2005 PukiWiki Developers Team
        License: GPL
        Based on "PukiWiki" 1.3 by yu-ji

SYNOPSIS

        http://pukiwiki.org/
        http://pukiwiki.sourceforge.jp/dev/
        http://sourceforge.jp/projects/pukiwiki/

DESCRIPTION

        PukiWiki�ϻ��üԤ���ͳ�˥ڡ������ɲá�������Խ��Ǥ���
        Web�ڡ���������PHP������ץȤǤ���
        Web��ư���Ǽ��ĤȤ���äȻ��Ƥ��ޤ�����
        Web�Ǽ��Ĥ�ñ�˥�å��������ɲä�������ʤΤ��Ф��ơ�
        PukiWiki�ϡ�Web�ڡ������Τ�ͳ���ѹ����뤳�Ȥ��Ǥ��ޤ���

        PukiWiki�ϡ����������YukiWiki�λ��ͤ򻲹ͤˤ����ȼ��˺���ޤ�����
        1.3�ޤǤ�yu-ji���󤬺�������1.3.1b�ʹߤ�PukiWiki Developers Team
        �ˤ�äƳ�ȯ��³�����Ƥ��ޤ���

        PukiWiki��PHP�ǽ񤫤줿PHP������ץȤȤ��Ƽ¸�����Ƥ��ޤ��Τǡ�
        PHP��ư���Web�����Фʤ�����Ū�ưפ����֤Ǥ��ޤ���

        PukiWiki�ϥե꡼���եȤǤ��� ����ͳ�ˤ��Ȥ�����������
        �ǿ��Ǥϡ� http://pukiwiki.org/ ��������Ǥ��ޤ���

������ˡ

        �ʲ��ϰ���Ǥ���Web�����С��ؤΥ����륢����������ǽ�Ǥ���С�
        ���������֤򤽤Τޤޥ����С���ž�����������С���ǲ���
        (tar pzxf pukiwiki*.tar.gz) ��������Ǥ�ư���Ϥ��Ǥ���

    1.  ���������֤�򤭤ޤ���

    2.  ɬ�פ˱���������ե�����(*.ini.php)�����Ƥ��ǧ���ޤ���
        1.11  ��������ե����뤬�̥ե������pukiwiki.ini.php�ˤʤ�ޤ�����
        1.4   ��������ե����뤬ʬ�䤵��ޤ�����
        1.4.4 ����������ä����PDA����������ե����뤬��Ĥ˽��󤵤�ޤ�����
          (i_mode.ini.php, jphone.ini.php ������+���� keitai.ini.php �˽���)

        * ��������
          ����               : pukiwiki.ini.php
          �桼�����         : rules.ini.php

        * �����������������
          �������ä����PDA  : keitai.ini.php
          ����¾             : default.ini.php

    3.  ���������֤����Ƥ򥵡��Ф�ž�����ޤ���
        �ե������ž���⡼�ɤˤĤ��Ƥϼ���򻲾Ȥ��Ƥ���������

    4.  �����о�Υե����뤪��ӥǥ��쥯�ȥ�Υѡ��ߥå������ǧ���ޤ���

        �ǥ��쥯�ȥ�   �ѡ��ߥå����
        attach         777	ź�եե������Ǽ�ǥ��쥯�ȥ�
        backup         777	�Хå����åץե������Ǽ�ǥ��쥯�ȥ�
        cache          777	����å���ե������Ǽ�ǥ��쥯�ȥ�
        counter        777	�����󥿥ե������Ǽ�ǥ��쥯�ȥ�
        diff           777	��ʬ�ե������Ǽ�ǥ��쥯�ȥ�
        image          755	�����ե�����
        image/face     755 	(�����ե�����)�ե������ޡ���  
        lib            755	�饤�֥��
        plugin         755	�ץ饰����
        skin           755	������CSS��JavaScirpt�ե�����
        trackback      777	TrackBack�ե������Ǽ�ǥ��쥯�ȥ�
        wiki           777	�ǡ����γ�Ǽ�ǥ��쥯�ȥ�

        �ե�����       �ѡ��ߥå���� ž���⡼��
        *.php          644            ASCII
        cache/*        666            ASCII
        image/*        644            BINARY
        image/face/*   644            BINARY
        lib/*          644            ASCII
        plugin/*       644            ASCII
        skin/*         644            ASCII
        wiki/*         666            ASCII

    5.  index.php ���뤤�� pukiwiki.php �˥֥饦�����饢���������ޤ���
        ɬ�פ˱����ơ�����������ǥ������Ĵ�����Ʋ�������

�ǡ����ΥХå����å���ˡ

        �ǡ����ե�����ǥ��쥯�ȥ�ʲ���Хå����åפ��ޤ���
        (�ǥե���ȥǥ��쥯�ȥ�̾�� wiki)

        ɬ�פ˱�����¾�Υǥ��쥯�ȥ�����Ƥ�Хå����åפ��ޤ���
        (�ǥե���ȥǥ��쥯�ȥ�̾�� attach, backup, counter, cache,
         diff, trackback)


�������ڡ����κ����

        �ֿ����ץ�󥯤��鿷�����ڡ������������ʳ��ˡ��ڡ��������
        �񤤤���礫�餽�Υڡ���̾�Υڡ�����������뤳�Ȥ��Ǥ��ޤ���

    1.  �ޤ���Ŭ���ʥڡ������㤨��FrontPage�ˤ����ӡ�
        �ڡ����ξ岼�ˤ�����Խ��ץ�󥯤򤿤ɤ�ޤ���

    2.  ����ȥƥ��������Ϥ��Ǥ�����֤ˤʤ�Τǡ� ������NewPage�Τ褦��ñ��
        ����ʸ����ʸ�����ߤ��Ƥ����ʸ����ˤ䡢 [[�������ڡ���̾]] ���ͤ�
        ��ŤΥ֥饱�åȤǰϤ������񤤤ơ���¸�פ��ޤ���

    3.  ��¸����ȡ�FrontPage�Υڡ������񤭴���ꡢ
        ���ʤ����񤤤�NewPage�Ȥ���ñ���ֿ������ڡ���̾�פȤ�������
        �θ��� '?' �Ȥ��������ʥ�󥯤�ɽ������ޤ��� ���Υ��
        �Ϥ��Υڡ������ޤ�¸�ߤ��ʤ����Ȥ򼨤����Ǥ���

    4.  ���� '?' �򥯥�å�����ȿ������ڡ������Ǥ��ޤ��Τǡ�
        ���ʤ��ι�����ʸ�Ϥ򤽤ο������ڡ����˽񤤤���¸���ޤ���

    5.  �������ڡ������Ǥ����FrontPage�Τ����θ�礫�� '?' �Ͼä��ơ�
        ���̤Υϥ��ѡ���󥯤Ȥʤ�ޤ���

�ƥ����������Υ롼��

        [[�����롼��]] �Υڡ����򻲾Ȥ��Ƥ���������

InterWiki

        1.11 ����InterWiki����������ޤ�����

        InterWiki �Ȥϡ�Wiki�����С���Ĥʤ��뵡ǽ�Ǥ���
        �ǽ�Ϥ������ä���� InterWiki �Ȥ���̾���ʤΤ������Ǥ�����
        ���ϡ�Wiki�����С������ǤϤʤ��ơ������ʥ����С�������ޤ���
        �ʤ��ʤ������Ǥ��������ʤ�� InterWiki �Ȥ���̾���Ϥ��ޤ굡ǽ��
        ɽ���Ƥ��ʤ����Ȥˤʤ�ޤ���
        ���ε�ǽ�� Tiki ����ܴۤ����˰ܿ����Ƥ��ޤ���

        �ܺ٤� [[InterWiki�ƥ��˥���]] �Υڡ����򻲾Ȥ��Ƥ���������

RDF/RSS�ν���

        1.2.1���顢RecentChanges��RDF/RSS����ϤǤ���褦�ˤʤ�ޤ�����
        1.4.5���顢RSS 2.0 ����ϤǤ���褦�ˤʤ�ޤ�����

        * ������ˡ����
          RSS 0.91 http://path/to/pukiwiki/index.php?plugin=rss
          RSS 1.0  http://path/to/pukiwiki/index.php?plugin=rss&ver=1.0
          RSS 2.0  http://path/to/pukiwiki/index.php?plugin=rss&ver=2.0

FAQ

        PukiWiki.org�Τ��줾��Υڡ���������å����Ʋ�������

        FAQ        http://pukiwiki.org/?FAQ
        ����Ȣ     http://pukiwiki.org/?%E8%B3%AA%E5%95%8F%E7%AE%B1
        ³������Ȣ http://pukiwiki.org/?%E7%B6%9A%E3%83%BB%E8%B3%AA%E5%95%8F%E7%AE%B1

BUG

        �Х����� dev�����ȤޤǤ��ꤤ���ޤ���
        (�桹��PukiWiki��PukiWiki�ΥХ��ȥ�å��󥰤�ԤäƤ��ޤ�)

        dev:BugTrack
        http://pukiwiki.sourceforge.jp/dev/?BugTrack

�ռ�

    PukiWiki Develpers Team�γ�����PukiWiki�桼���γ�����˴��դ��ޤ���
    PukiWiki ��ȯ������yu-ji(��sng)����˴��դ��ޤ���
    YukiWiki �Υ����󲽤���Ĥ��Ƥ�����������������˴��դ��ޤ���
    �ܲȤ�WikiWiki���ä�Cunningham & Cunningham, Inc.�� ���դ��ޤ���

���ȥ��

    * PukiWiki�ۡ���ڡ���      http://pukiwiki.org/
    * yu-ji����Υۡ���ڡ���   http://factage.com/yu-ji/
    * ��������Υۡ���ڡ���  http://www.hyuki.com/
    * YukiWiki�ۡ���ڡ���      http://www.hyuki.com/yukiwiki/
    * Tiki                      http://todo.org/cgi-bin/tiki/tiki.cgi
    * �ܲ�WikiWikiWeb           http://c2.com/cgi/wiki?WikiWikiWeb
    * WikiWikiWeb�κ��(Cunningham & Cunningham, Inc.) http://c2.com/
    
