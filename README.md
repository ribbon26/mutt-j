

# このプロジェクトではパワフルなテキストベースのメールクライアント Mutt に対する日本語パッチの開発と日本語の文書の整備を行っています。

## 日本語マニュアル 

最新版の日本語マニュアル(2.2.10対応)は、 <a href="manual.ja.html"> manual.ja.html </a>です。暫定版で、まだ未訳の所が残っています。

## 最新ニュース
<ul>
<li>2023年3月25日 mutt-2.2.10 がリリースされました。</li>
<li>2020年11月7日 mutt-2.0.0 がリリースされました。</li>
<li>2020年8月22日: mutt-1.14.6 に対応する<a href="manual.ja.html">マニュアル</a>を更新しました。ただし、暫定版で、まだ未訳の所が残っています。</li>
<li>2020年7月10日: mutt-1.13.4 に対応する<a href="manual.ja.html">マニュアル</a>を更新しました。ただし、暫定版で、まだ未訳の所が残っています。</li>
<li>2015年1月2日:mutt-1.5.23に対応するパッチが<a href="https://osdn.jp/projects/mutt-j/releases/">公開されています</a>。</li>
<li>2007年3月10日:開発版 <a href="http://prdownloads.sourceforge.net/mutt/mutt-1.5.14.tar.gz?download">Mutt 1.5.14</a> に対する<a href="http://www.emaillab.org/mutt/download1514.html">日本語パッチ</a>が公開されています。なお、<a href="http://dev.mutt.org/nightlies/">snapshot</a>には日本語パッチのかなりの部分が取り入れられています。ぜひお試しください。現時点の snapshot で UTF-8 環境において日本語を使いたい場合には <a href="http://www.emaillab.org/mutt/1.5.14/patch-1.5.14.tt.wcwidth.3">wcwidth.3 パッチ</a>を当て、<tt>--enable-cjk-ambiguous-width</tt> オプションを指定して <tt>configure</tt> すると良いようです。</li>
<li>2007年3月10日:安定版 <a href="http://prdownloads.sourceforge.net/mutt/mutt-1.4.2.2i.tar.gz?download">Mutt 1.4.2.2i</a> は IMAP サーバからの攻撃に対するセキュリティ修正です。日本語パッチは <a href="http://www.emaillab.org/mutt/download14.html">1.4.2.1i 用のもの</a>が使えるはずです。信用できない IMAP サーバをご利用の場合にはアップデートしてください。</li>
<li>2005年8月13日:開発版 Mutt 1.5.10i 用に<a href="manual.ja.html">マニュアル</a>を更新しました。<a href="manual.ja-20050813.tar.gz">tar + gz アーカイブ</a> もあります。</li>
<li>2005年4月5日:ribbon.or.jp 上の mutt-j メーリングリストは閉鎖されます。<a href="http://lists.sourceforge.jp/mailman/listinfo/mutt-j-users">mutt-j-users</a> をご利用ください。</li>
<li>2005年3月19日:メーリングリストを sourceforge.jp 上に用意しました。<a href="http://lists.sourceforge.jp/mailman/listinfo/mutt-j-users">フォーム</a>から簡単にご登録いただけます。</li>
<li>2004年2月14日:安定版 Mutt 1.4.2.1i に対する日本語パッチ 1.4.2.1i-ja.1 を公開しました。バッファーオーバーフローに対する修正が行われているので 1.4.1i以前のバージョンを使用している方は 1.4.2.1i にバージョンアップするようにしてください。</li>
<li>2004年2月14日:開発版 Mutt 1.5.6i に対する日本語パッチ 1.5.6i-ja.1 を公開しました。</li>
<li>2004年2月12日:mutt-1.4.1i以前のバージョンにバッファーオーバーフローが見つかりました。詳しくは "<a href="security-20040212.txt">buffer overflow</a>" をご覧ください。</li>
</ul>

## プロジェクト概要
<p><a class="uri" href="http://sourceforge.jp/projects/mutt-j/">
http://sourceforge.jp/projects/mutt-j/</a></p>

## ダウンロード
<p><a href="http://sourceforge.jp/frs/index.php?group_id=351">
リリースファイル・リスト</a> (sourceforge.jp)</p>

## ports, rpm等
<ul>
<li><a href="http://www.cetus-net.org/ftp/NetBSD/pkgsrc-test/">
NetBSD の非公式 pkgsrc</a>
(前原さん)</li>
<li><a href="http://linux.matchy.net/?MuttJaRpm">
linux.matchy.net - 私家版 rpm - mutt 日本語版</a>
(町田さん)</li>
<li><a href="http://kimux.org/liza/mutt.shtml">
[SL-C700] Mutt と仲間たち</a>
(Linux 塔載 Zaurus 用パッケージ)</li>
</ul>

## メーリングリスト
<p>Mutt に関する日本語のメーリングリスト <strong>mutt-j-users</strong> があります。
有意義な情報交換のために積極的にご利用ください。</p>

<p><strong>mutt-j-users</strong> は、sourceforge.jp 上の
<a href="http://lists.sourceforge.jp/mailman/listinfo/mutt-j-users">リストの案内</a>
にあるフォームから簡単に登録することができます。また、
<a href="http://lists.sourceforge.jp/mailman/archives/mutt-j-users">保存書庫</a>
で過去のメールを読むことができます。</p>

## リンク
<dl>
<dt><a href="http://www.mutt.org/">
Mutt Home Page</a>
<!-- (<a href="http://www.emaillab.org/mutt/www.mutt.org/">mirror</a>) --></dt>
<dd>Mutt 本家のページ<!-- 及び そのミラー (<span class="uri">www.emaillab.org</span>) --></dd>
<dt><a href="http://dev.mutt.org/">
Mutt Development Pages</a></dt>
<dd>最新版マニュアル、Mercurial ミラー、その他開発者に有用な情報。(英語)</dd>
<dt><a href="http://wiki.mutt.org/">
Mutt Wiki</a></dt>
<dd>だれでも書き込める Mutt の Wiki です。
パッチリストなどもあります。(英語)</dd>
<dt><a href="http://www.emaillab.org/mutt/">
Mutt Japanese Edition</a></dt>
<dd>日本語パッチのメンテナーの滝澤さんのページ。
Muttを使うために必要な情報はここで手に入ります。</dd>
<dt><a href="http://www.momonga-linux.org/docs/Mutt-QuickStart-HOWTO/ja/">
Mutt-QuickStart-HOWTO</a></dt>
<dd>Momonga Project の Mutt QuickStart HOWTO</dd>
<dt><a href="manual.ja.html">
1.5.10 マニュアル邦訳</a>
と<a href="manual.ja.sgml">
そのソース SGML</a> および <a href="manual.ja.txt">
plain text 版</a>
(<a href="manual.ja-20040808.tar.gz">20040808</a> と
<a href="manual.ja-20041208.tar.gz">20041208</a> と
<a href="manual.ja-20050201.tar.gz">20050201</a> と
<a href="manual.ja-20050213.tar.gz">20050213</a> と
<a href="manual.ja-20050813.tar.gz">20050813</a> の
tar + gz アーカイブもあります)</dt>
<dd><code>diff -du manual.sgml manual.ja.sgml</code>
とすると対応が見える邦訳です。(tamomo)</dd>

