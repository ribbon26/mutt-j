NAME

    PukiWiki - ��ͳ�˥ڡ������ɲá�������Խ��Ǥ���Web�ڡ�������PHP������ץ�

        PukiWiki 1.4.3
        Copyright (C) 2001,2002,2003 PukiWiki Developers Team.
        License is GNU/GPL.
        Based on "PukiWiki" 1.3 by sng
        http://pukiwiki.org/

SYNOPSIS

        http://pukiwiki.org/

DESCRIPTION

        PukiWiki�ϻ��üԤ���ͳ�˥ڡ������ɲá�������Խ��Ǥ���
        Web�ڡ���������PHP������ץȤǤ���
        Web��ư���Ǽ��ĤȤ���äȻ��Ƥ��ޤ�����
        Web�Ǽ��Ĥ�ñ�˥�å��������ɲä�������ʤΤ��Ф��ơ�
        PukiWiki�ϡ�Web�ڡ������Τ�ͳ���ѹ����뤳�Ȥ��Ǥ��ޤ���

        PukiWiki�ϡ����������YukiWiki�λ��ͤ򻲹ͤˤ����ȼ��˺���ޤ�����
        1.3�ޤǤ�sng���󤬺�������1.3.1b�ʹߤ�PukiWiki Developers Team�ˤ�ä�
        ��ȯ��³�����Ƥ��ޤ���

        PukiWiki��PHP�ǽ񤫤줿PHP������ץȤȤ��Ƽ¸�����Ƥ��ޤ��Τǡ�
        PHP��ư���Web�����Фʤ�����Ū�ưפ����֤Ǥ��ޤ���

        PukiWiki�ϥե꡼���եȤǤ��� ����ͳ�ˤ��Ȥ�����������

������ˡ

    ����

        PukiWiki�κǿ��Ǥϡ� http://pukiwiki.org/ ��������Ǥ��ޤ���

    ���󥹥ȡ���

    1.  ���������֤�򤭤ޤ���

    2.  ɬ�פ˱���������ե�����(*.ini.php)�����Ƥ��ǧ���ޤ���
        1.11 ��������ե����뤬�̥ե������pukiwiki.ini.php�ˤʤ�ޤ�����
        1.4 ��������ե����뤬ʬ�䤵��ޤ�����

        *   ��������          : pukiwiki.ini.php

        *   �����������������
                I-MODE,AirH"  : i_mode.ini.php
                J-PHONE       : jphone.ini.php
                ����¾        : default.ini.php

        *   �桼������롼��  : rules.ini.php

    3.  ���������֤����Ƥ򥵡��Ф�ž�����ޤ���
        �ե������ž���⡼�ɤˤĤ��Ƥϼ���򻲾Ȥ��Ƥ���������

    4.  pukiwiki.ini.php��ǻ��ꤷ���ʲ��Υǥ��쥯�ȥ��������ޤ���

        �ǡ����γ�Ǽ�ǥ��쥯�ȥ�               (�ǥե���Ȥ�wiki)
        ��ʬ�ե������Ǽ�ǥ��쥯�ȥ�           (�ǥե���Ȥ�diff)
        �Хå����åץե������Ǽ�ǥ��쥯�ȥ�   (�ǥե���Ȥ�backup)
        ����å���ե������Ǽ�ǥ��쥯�ȥ�     (�ǥե���Ȥ�cache)
        ź�եե������Ǽ�ǥ��쥯�ȥ�           (�ǥե���Ȥ�attach)
        �����󥿥ե������Ǽ�ǥ��쥯�ȥ�       (�ǥե���Ȥ�counter)
        TrackBack�ե������Ǽ�ǥ��쥯�ȥ�      (�ǥե���Ȥ�trackback)

        �ǥ��쥯�ȥ���˥ե����뤬������ˤϡ����Υե������°����
        666���ѹ����Ƥ���������

    5.  �����о�Υե����뤪��ӥǥ��쥯�ȥ�Υѡ��ߥå������ǧ���ޤ���
        �ե�����Υѡ��ߥå����ˤĤ��Ƥϼ���򻲾Ȥ��Ƥ���������

    6.  pukiwiki.php�˥֥饦�����饢���������ޤ���

    �ѡ��ߥå����

        �ǥ��쥯�ȥ�   �ѡ��ߥå����
        attach         777
        backup         777
        cache          777
        counter        777
        diff           777
        face           755
        image          755
        plugin         755
        skin           755
        trackback      777
        wiki           777

        �ե�����       �ѡ��ߥå���� ž���⡼��
        *.php          644            ASCII
        *.lng          644            ASCII
        pukiwiki.png   644            BINARY

        cache/*        666            ASCII
        face/*         644            BINARY
        image/*        644            BINARY
        plugin/*       644            ASCII
        skin/*         644            ASCII
        wiki/*         666            ASCII

�ǡ����ΥХå����å���ˡ

        �ǡ����ե�����ǥ��쥯�ȥ�ʲ���Хå����åפ��ޤ���
        (�ǥե���ȥǥ��쥯�ȥ�̾�� wiki)

        ɬ�פ˱�����¾�Υǥ��쥯�ȥ�����Ƥ�Хå����åפ��ޤ���
        (�ǥե���ȥǥ��쥯�ȥ�̾�� attach,backup,counter,cache,diff,trackback)

�������ڡ����κ����

    1.  �ޤ���Ŭ���ʥڡ������㤨��FrontPage�ˤ����ӡ�
        �ڡ����β��ˤ�����Խ��ץ�󥯤򤿤ɤ�ޤ���

    2.  ����ȥƥ��������Ϥ��Ǥ�����֤ˤʤ�Τǡ� ������NewPage�Τ褦��ñ��
        ����ʸ����ʸ�����ߤ��Ƥ����ʸ����� ��񤤤ơ���¸�פ��ޤ���

    3.  ��¸����ȡ�FrontPage�Υڡ������񤭴���ꡢ
        ���ʤ����񤤤�NewPage�Ȥ���ʸ����θ��� ?
        �Ȥ�����󥯤�ɽ������ޤ��� ���� ?
        �Ϥ��Υڡ������ޤ�¸�ߤ��ʤ����Ȥ򼨤����Ǥ���

    4.  ���� ? �򥯥�å�����ȿ������ڡ���NewPage���Ǥ��ޤ��Τǡ�
        ���ʤ��ι�����ʸ�Ϥ򤽤ο������ڡ����˽񤤤���¸���ޤ���

    5.  NewPage�ڡ������Ǥ����FrontPage�� ? �Ͼä��ơ���󥯤Ȥʤ�ޤ���

�ƥ����������Υ롼��

        [[�����롼��]] �ڡ����򻲾Ȥ��Ƥ���������

InterWiki

        1.11 ����InterWiki����������ޤ�����

        InterWiki �Ȥϡ�Wiki�����С���Ĥʤ��뵡ǽ�Ǥ���
        �ǽ�Ϥ������ä���� InterWiki �Ȥ���̾���ʤΤ������Ǥ�����
        ���ϡ�Wiki�����С������ǤϤʤ��ơ������ʥ����С�������ޤ���
        �ʤ��ʤ������Ǥ��������ʤ�� InterWiki �Ȥ���̾���Ϥ��ޤ굡ǽ��
        ɽ���Ƥ��ʤ����Ȥˤʤ�ޤ���
        ���ε�ǽ�� Tiki ����ܴۤ����˰ܿ����Ƥ��ޤ���

        �ܺ٤� [[InterWiki�ƥ��˥���]] �ڡ����򻲾Ȥ��Ƥ���������

RDF/RSS

        1.2.1���顢RecentChanges��RDF/RSS����ϤǤ���褦�ˤʤ�ޤ�����
        ���ѤǤ��뤫�Ϥ狼��ʤ��Ǥ��������貿���˻Ȥ���С��ȻפäƤޤ���

    *   RSS 0.91 �ν�����ˡ����

        *   http://pukiwiki/index.php?cmd=rss

    *   RSS 1.0 �ν�����ˡ����

        *   http://pukiwiki.org/index.php?cmd=rss10


PukiWiki/1.3.x�Ȥ���ߴ���

    1.  [[WikiName]]��WikiName��Ʊ���ڡ�����ؤ��ޤ���
    2.  ����ꥹ�Ȥν񼰤��㤤�ޤ��� :term:description -> :term|description
    3.  �ꥹ�Ȥ����ʸ�ϡ����̥�٥�Υꥹ�Ȥ����ʸ����ޤ��뤳�Ȥ��Ǥ��ޤ���
        (1.3.x�Ǥϡ��ꥹ�Ȥ�Ʊ��Τߡ�������ˤϰ��Ѥ�����ޤǤ��ޤ���Ǥ�����)

PukiWiki/1.4.2�Ȥ���ߴ���

    1.  trackback/�ǥ��쥯�ȥ��۲��˺��������ե������̾����������§���Ѥ��ޤ�����
        trackback/referer��ǽ�򤪻Ȥ��ǡ�1.4.2����1.4.3�ذܹԤ������ϡ�PukiWiki.dev������
        (http://pukiwiki.sourceforge.jp/dev/)�Ρ���ȯ����/2004-03-18�ڡ����򻲾Ȥ��ƺ�Ȥ�ԤäƤ���������

��������
    *   2004-03-21 1.4.3 by PukiWiki Developers Team
        BugTrack/493 ��󥯸���ʸ������������ꤹ���ʸ�����������Ѥ���ȡ�related��ȿ�����ʤ� 
            [[cvs:make_link.php]](v1.4:r1.64)
        BugTrack/494 LANG=en�λ���template�ץ饰����
            [[cvs:en.lng]](v1.4:r1.25)
        $script�Υ����å���is_url�����
            [[cvs:func.php]](v1.4:r1.55)
            [[cvs:init.php]](v1.4:r1.69)
        BugTrack/496 ChaSen��Ȥä����ڥ������ɤߤ������������ʤ�
            [[cvs:func.php]](v1.4:r1.56)
        ����Ȣ/344 ���ϻ��β��Ԥΰ����ˤĤ��ƺƤ�
        ����Ȣ/280 ���Ԥ����Τޤ�ȿ�Ǥ���ʤ��Τ�?
        ����¾ �����θ�ľ��
             [cvs:convert_html.php]](v1.4:r1.58)
        $line_break�ɲ� ���Ԥ�ȿ�Ǥ���(���Ԥ�<br />���ִ�����)�Ȥ�1
             [[cvs:pukiwiki.ini.php]](v1.4:r1.52)
        BugTrack/491 RecentDelete���ۤ���
             [[cvs:file.php]](v1.4:r1.38)
             [[cvs:pukiwiki.ini.php]](v1.4:r1.53)
        BugTrack/498 ����ꥹ�Ȥ�����ʸ����ˤ���ȥ֥�å�����λ����
             [[cvs:convert_html.php]](v1.4:r1.58)
        BugTrack/462 IIS �Ķ����ˤ����� Basicǧ�ڤ�ư���ʤ�
             [[cvs:auth.php]](v1.4:r1.3)
        ����Ȣ/343 readme.txt�˵����¸ʸ��?
             [[cvs:readme.txt]](v1.4:r1.15)
        ����Ȣ/346 counter�ץ饰����ǥ�����ɽ���оݤ�yesterday�ˤĤ���
             [[cvs:plugin/counter.inc.php]](v1.4:r1.12)
        $_SERVER����Υ����å�����(Warning�к�)
             [[cvs:init.php]](v1.4:r1.70)
        ���Ѥ���Ƥ��ʤ���ʬ�򥳥��ȥ�����(Warning�к�)
             [[cvs:plugin/bugtrack.inc.php]](v1.4:r1.15)
        ����������å�(Warinig�к�)
             [[cvs:plugin/md5.inc.php]](v1.4:r1.3)
        rss�����˼��Ԥ����Ȥ��Υ��顼�������ɲ�(Warning�к�)
             [[cvs:plugin/showrss.inc.php]](v1.4:r1.11)
        ���줿�쥳���ɤ򥹥��å�(Warning�к�)
             [[cvs:plugin/tb.inc.php]](v1.4:r1.6)
        ���顼�����å�����
             [[cvs:plugin/tracker.inc.php]](v1.4:r1.20)
        BugTrack/499 #contents ��ľ��˥ꥹ�����Ǥ��ɲä����ɽ������ʤ�
             [[cvs:convert_html.php]](v1.4:r1.59)
        TableCell�����ΤȤ�<td>~</td>��������ά����Ƥ��ޤ�
             [[cvs:convert_html.php]](v1.4:r1.60)
        Content-Type�إå���charset����Ϥ��ʤ��褦��(r1.37)
        �ե������������ɽ�����mb_http_output('pass')���ɲ�(r1.38)
             [[cvs:attach.inc.php]](v1.4:r1.38)
        join('',...)������
             [[cvs:plugin/tb.inc.php]](v1.4:r1.7)
        ���顼�����å�����
             [[cvs:plugin/tracker.inc.php]](v1.4:r1.20)
        rss�����˼��Ԥ����Ȥ��Υ��顼�������ɲ�
             [[cvs:plugin/showrss.inc.php]](v1.4:r1.11)
        ����������å�
             [[cvs:plugin/md5.inc.php]](v1.4:r1.3)
        mode=submit�����ꤵ��Ƥ��ʤ��Ȥ��ϲ��⤷�ʤ��褦��
             [[cvs:plugin/bugtrack.inc.php]](v1.4:r1.15)
        BugTrack/514 �����ȥ�󥯤���ʤ��ڡ��������� (BugTrack/502�ν����ߥ�)
             [[cvs:func.php]](v1.4:r1.58)
        BugTrack/518 referer��Ʊ��URL���̤Τ�ΤȤ��Ƶ�Ͽ�����
             [[cvs:trackback.php]](v1.4.r1.15)
             [[cvs:plugin/tb.inc.php]](v1.4:r1.8)
        BugTrack/519 ����̾�Υڡ�����RecentChanges��������ɽ������ʤ���
             [[cvs:file.php]](v1.4:r1.39)
        BugTrack/523 ��PukiWiki/1.4/����ä�������/ñ�측���η��ɽ�����ĥ�פ�Ƴ������ȡ���&�פ򸡺���������ɽ��������
             [[cvs:html.php]](v1.4:r1.97)
        BugTrack/525 ����å��幹����¹Ԥ����Ȥ��˥������notice��ɽ�������Τ��ɤ�
             [[cvs:plugin/links.inc.php]](v1.4:r1.18)
        BugTrack/526 rss10.inc.php��typo
             [[cvs:plugin/rss10.inc.php]](v1.4:r1.10)
        BugTrack/538 clear�ץ饰�����Ʊ��
             [[cvs:plugin/clear.inc.php]](v1.4:r1.1)
             [[cvs:skin/default.en.css]](v1.4:r1.28)
             [[cvs:skin/default.ja.css]](v1.4:r1.29)
        BugTrack/539 �إ�פν���(��󥯡������ꥢ��)
             [[cvs:wiki/C0B0B7C1A5EBA1BCA5EB.txt]](v1.4:r1.7)
        BugTrack/541 source.inc.php��ǧ�ڥ����å�ϳ��
             [[cvs:plugin/source.inc.php]](v1.4:r1.11)
        BugTrack/543 backup.inc.php�ǥ�����������
             [[cvs:plugin/backup.inc.php]](v1.4.r1.10)
        BugTrack/549 ref�ץ饰�����noicon��noimg���ץ����ΤȤ���ʸ����θ��˶�������
             [[cvs:plugin/ref.inc.php]](v1.4:r1.21)
        BugTrack/521 ����ʸ��¿�Ų��ǡ������~���ά��������XHTML 1.1 not valid�ˤʤ�
        BugTrack/524 ����ꥹ�Ȥǰ�Ĥ�������Ф�ʣ�����������Ĥ��ʤ�
        BugTrack/545 �����ﲼ��blockquote���p�ǰϤ鷺�ƥ�����ľ��
             [[cvs:convert_html.php]](v1.4:r1.61)
        BugTrack/555 encode_hint��Ƚ��ߥ�
        BugTrack/536 i-mode�б��������Ư���Ƥ��ʤ�?
             [[cvs:init.php]](v1.4:r1.71) 
        BugTrack/552 backup.inc.php�ǥ�����������ˤ��ѡ������顼
             [[cvs:plugin/backup.inc.php]](v1.4:r1.11)
        BugTrack/530 TouchGraph Plugin
             [[cvs:plugin/touchgraph.inc.php]](v1.4:r1.3)
        BugTrack/534 ref�ץ饰����ǻ��ȥڡ�����BracketName�˥���ޤ��ޤޤ�Ƥ���ȥե�����򻲾ȤǤ��ʤ���
             [[cvs:convert_html.php]](v1.4:r1.62)
             [[cvs:func.php]](v1.4:r1.59)
             [[cvs:plugin.php]](v1.4:r1.10)
        BugTrack/539 �إ�פν���(��󥯡������ꥢ��)
             [[cvs:wiki/C0B0B7C1A5EBA1BCA5EB.txt]](v1.4:r1.7)
        BugTrack/540 trackback��URL��trackback list��ɽ�����줺FrontPage��ɽ�������
             [[cvs:trackback.php]](v1.4:r1.16)
        BugTrack/558 trackback ping��GET�����äƤ��륵���Ȥ��б���
             [[cvs:plugin/rss10.inc.php]](v1.4:r1.11)
             [[cvs:plugin/tb.inc.php]](v1.4:r1.9)
             [[cvs:init.php]](v1.4:r1.72)
             [[cvs:trackback.php]](v1.4:r1.16)
        BugTrack/559 $whatsnew�����ܸ�ڡ���̾�����Ǥ��ʤ�
             [[cvs:plugin/rss.inc.php]](v1.4:r1.6)
             [[cvs:plugin/rss10.inc.php]](v1.4:r1.11)
        �ե������å�������Ĵ��
             [[cvs:file.php]](v1.4:r1.40)
             [[cvs:plugin/attach.inc.php]](v1.4:r1.39)
             [[cvs:plugin/counter.inc.php]](v1.4:r1.13)
             [[cvs:plugin/online.inc.php]](v1.4:r1.8)
        Notice: Undefined offset: 1 �к�
             [[cvs:html.php]](v1.4:r1.98)
        �Хå����åפκݡ��ڡ����˴ޤޤ��Хå����åפζ��ڤ�ʸ����̵��������褦��
             [[cvs:backup.php]](v1.4:r1.15)
        ���פ�set_time_limit()����
             [[cvs:plugin/rename.inc.php]](v1.4:r1.10)
        �����ץ饰����  $line_break��ڡ���������ڤ��ؤ���
             [[cvs:plugin/setlinebreak.inc.php]](v1.4:r1.1)
        BugTrack/561 �ե�����ǥ����ܡ��ɤ��饿�֥����ɤ����ϤǤ��ʤ��Τǡ�
             [[cvs:rules.ini.php]](v1.4:r1.2)
             [[cvs:wiki/C0B0B7C1A5EBA1BCA5EB.txt]](v1.4:r1.8)
        BugTrack/562 ChaSen/KAKASI̵���Ǥ⤢���������ܸ�ڡ�������ʬ����ǽ��
             [[cvs:file.php]](v1.4:r1.41)
             [[cvs:pukiwiki.ini.php]](v1.4:r1.54)
             [[cvs:wiki/3A636F6E6669672F5061676552656164696E67.txt]](v1.4:r1.1)
             [[cvs:wiki/3A636F6E6669672F5061676552656164696E672F64696374.txt]](v1.4:r1.1)
        BugTrack/563 rename�ץ饰����ǡ�./hogehoge�פȤ����褦�ʥڡ����������Ǥ��Ƥ��ޤ�
             [[cvs:plugin/rename.inc.php]](v1.4:r1.11)
        BugTrack/564 versionlist�ץ饰�����lng�ե����뤬ɽ������ʤ�
             [[cvs:plugin/versionlist.inc.php]](v1.4:r1.7)
             [[cvs:plugin/versionlist.inc.php]](v1.3:r1.2.2.1)
        csv_explode, csv_implode�ؿ���Ĵ��
             [[cvs:func.php]](v1.4:r1.61)
        BugTrack/566 [cvs] $Id ��̵��/��Ĥ��� (�ޥ����졼������ȸ���) 
             [[cvs:default.ini.php]](v1.4:r1.4)
             [[cvs:i_mode.ini.php]](v1.4:r1.5)
             [[cvs:jphone.ini.php]](v1.4:r1.6)
             [[cvs:pukiwiki.ini.php]](v1.4:r1.55)
             [[cvs:plugin/clear.inc.php]](v1.4:r1.1) (cvs admin -kkv)
             [[cvs:plugin/versionlist.inc.php]](v1.4:r1.8)
             [[cvs:skin/default.en.css]](v1.4:r1.29)
             [[cvs:skin/default.ja.css]](v1.4:r1.30)
             [[cvs:skin/default.js]](v1.4:r1.2)
             [[cvs:skin/keitai.skin.ja.php]](v1.4:r1.5)
             [[cvs:skin/print.en.css]](v1.4:r1.3)
             [[cvs:skin/print.ja.css]](v1.4:r1.3)
             [[cvs:skin/pukiwiki.skin.en.php]](v1.4:r1.31)
             [[cvs:skin/pukiwiki.skin.ja.php]](v1.4:r1.32)
             [[cvs:skin/trackback.css]](v1.4:r1.2)
             [[cvs:skin/trackback.js]](v1.4:r1.2)
        BugTrack/553 �ڡ����ΥХå����åפ������Ƥ⡢��ʬ�ϻĤäƤ��ޤ�
             [[cvs:plugin/diff.inc.php]](v1.4:r1.4)
             [[cvs:en.lng]](v1.4:r1.26)
             [[cvs:ja.lng]](v1.4:r1.25)
        BugTrack/570 �ɲä������롼���ɽ��������Խ��ˤʤ�
             [[cvs:html.php]](v1.4:r1.99)

    *   2003-11-17 1.4.2 by PukiWiki Developers Team
        BugTrack/487 autolink��ʸ������
            [[cvs:func.php]](v1.4:r1.54)
        BugTrack/488 mbstring̵���ξ��֤�AutoLink�����ꤹ��ȥڡ�����������
            [[cvs:mbstring.php]](v1.4:r1.9)
        �ؿ�̾�����󥹥ȥ饯���Ⱦ���
            [[cvs:convert_html.php]](v1.4:r1.57)
        tracker_list()����2�����ǥڡ���̾�����л��꤬�Ȥ���褦��
        tracker()����1��������ά���줿�Ȥ���'default'��Ȥ�
            [[cvs:plugin/tracker.inc.php]](v1.4:r1.18)
        ���顼������Ĵ��
            [[cvs:plugin/template.inc.php]](v1.4:r1.16)
        �ѿ�̾�ְ㤤
            [[cvs:plugin/rename.inc.php]](v1.4:r1.9)

    *   2003-11-10 1.4.1 by PukiWiki Developers Team

        BugTrack/478    �ꥹ�Ȥλ����Ǥ�������������Ϥ���ʤ�
        BugTrack/479    CGI��PHP�ξ�硢HTTPS�����ѤǤ��ʤ�
        BugTrack/480    online.inc.php ��Υǥ��쥯�ȥ����������
        BugTrack/482    AutoLink��ư���Ĵ��
        BugTrack/483    ������HTML����ƥ��ƥ���񤯤���᤬����ʤ�
        BugTrack/485    lookup��InterWikiName�Ρָ���������¹Ԥ����
                        &�Ǥʤ�&amp;������
        BugTrack/486    header�ǥ���å���̵����
        tracker.inc.php radio/select/checkbox�ǡ�����褬�ҤȤĤ�����
                        ����ʤ��ä��Ȥ��ϡ��ͤ����Ȥ���
        backup.php      data�����ξ���warning�޻�

    *   2003-11-03 1.4 by PukiWiki Developers Team

        1.4�Ϻǽ�Υ�꡼��

TODO

    http://pukiwiki.org/?BugTrack

���

    PukiWiki 1.4
    Copyright (C) 2001,2002,2003 PukiWiki Developers Team. License is GNU/GPL.
    Based on "PukiWiki" 1.3 by sng
    http://pukiwiki.org/

    ���䡢�ո����Х����� http://pukiwiki.org/ �ޤǤ��ꤤ���ޤ���

���۾��

    PukiWiki�ϡ� GNU General Public License�ˤƸ������ޤ���

    PukiWiki�ϥե꡼���եȤǤ��� ����ͳ�ˤ��Ȥ�����������

�ռ�

    PukiWiki Develpers Team�γ�����PukiWiki�桼���γ�����˴��դ��ޤ���

    PukiWiki ��ȯ������sng����˴��դ��ޤ���

    YukiWiki �Υ����󲽤���Ĥ��Ƥ�����������������˴��դ��ޤ���

    �ܲȤ�WikiWiki���ä�Cunningham & Cunningham, Inc.�� ���դ��ޤ���

���ȥ��

    *   PukiWiki�ۡ���ڡ��� http://pukiwiki.org/

    *   sng�Υۡ���ڡ��� http://factage.com/sng/

    *   ��������Υۡ���ڡ��� http://www.hyuki.com/

    *   YukiWiki�ۡ���ڡ��� http://www.hyuki.com/yukiwiki/

    *   Tiki http://todo.org/cgi-bin/jp/tiki.cgi

    *   �ܲȤ�WikiWiki http://c2.com/cgi/wiki?WikiWikiWeb

    *   �ܲȤ�WikiWiki�κ��(Cunningham & Cunningham, Inc.) http://c2.com/
