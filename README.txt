= rimo.tv.xyzzy - Rimo を xyzzy 上に表示する。

  * Author: MIYAMUKO Katsuyuki ((<URL:mailto:miyamuko (at) gmail.com>))
  * URL: ((<URL:http://miyamuko.s56.xrea.com/xyzzy/rimo.tv.xyzzy/intro.htm>))
  * Version: 0.1.1


== SYNOPSIS

  M-x rimo.tv.on
  M-x rimo.tv.off

== DESCRIPTION

rimo.tv.xyzzy は ((<Rimo|URL:http://rimo.tv/>)) を xyzzy 上に表示するためのソフトウェアです。

rimo.tv.xyzzy を使えばウィンドウの隅に Rimo を表示させながら xyzzy で hack できます。


== INSTALL

((<NetInstaller|URL:http://www7a.biglobe.ne.jp/~hat/xyzzy/ni.html>)) でインストールした場合は 3 以降を、
NetInstaller + ni-autoload を使っている人は 4 以降で OK です。

(1) アーカイブをダウンロードします。

    ((<URL:http://miyamuko.s56.xrea.com/xyzzy/archives/rimo.tv.xyzzy.zip>))

(2) アーカイブを展開して、$XYZZY/site-lisp 配下にファイルをコピーします。

(3) ~/.xyzzy または $XYZZY/site-lisp/siteinit.l に以下のコードを追加します。

        ;; rimo.tv.xyzzy
        (require "rimo.tv.xyzzy")
        (use-package :rimo)

(4) 設定を反映させるため xyzzy を再起動してください。

    ※siteinit.l に記述した場合には再ダンプが必要です。

(5) M-x rimo.tv.on で Rimo の画面を表示します。

    終了するには M-x rimo.tv.off を実行してください。


== MODULE

=== PACKAGE

rimo.tv.xyzzy は以下のパッケージを利用しています。

  * rimo
  * rimo-ffi

=== EXPORT

rimo.tv.xyzzy は以下のシンボルを export しています。

  * rimo パッケージ

    * ((<*rimo-width*>))
    * ((<*rimo-height*>))
    * ((<*rimo-position*>))
    * ((<rimo.tv.on>))
    * ((<rimo.tv.off>))
    * ((<rimo.tv.show>))
    * ((<rimo.tv.hide>))
    * ((<rimo.tv.move x y pos>))
    * ((<rimo.tv.window>))

  * rimo-ffi パッケージ

    内部パッケージなので省略。

=== VARIABLE

--- *rimo-width*

    Rimo を表示する子ウィンドウの幅をピクセル単位で指定します。
    デフォルトは 400 (px) です。

--- *rimo-height*

    Rimo を表示する子ウィンドウの高さをピクセル単位で指定します。
    デフォルトは 300 (px) です。

--- *rimo-position*

    Rimo を表示する子ウィンドウの位置をプロパティリストで指定します。
    基本的に css と同じように指定します。

      '((:top    . <xyzzy ウィンドウの上からの位置>)
        (:bottom . <xyzzy ウィンドウの下からの位置>)
        (:left   . <xyzzy ウィンドウの左からの位置>)
        (:right  . <xyzzy ウィンドウの右からの位置>))

    * :top と :bottom はどちらか片方を指定します。
      両方指定した場合は :top が優先されます。
    * :left と :right はどちらか片方を指定します。
      両方指定した場合は :left が優先されます。

    デフォルトは以下のとおりです (だいたい画面右下に表示されます)。

      '((:right  . 20)
        (:bottom . 60))

    例)

      ;; rimo を右上に表示
      (setf rimo:*rimo-position* '((:top . 40)    ;; ツールバーとかを表示しているのならもっと多めに
                                   (:right . 20)))


=== COMMAND

--- rimo.tv.on

    Rimo のウィンドウを表示します。
    すでに表示している場合は何もしません。

--- rimo.tv.off

    Rimo のウィンドウを閉じます。
    すでに閉じている場合は何もしません。

--- rimo.tv.show

    ((<rimo.tv.hide>)) で非表示にした Rimo のウィンドウを表示します。

--- rimo.tv.hide

    Rimo を一時的に非表示にします。
    非表示にするだけで再生は続けられます (音声も出ます)。

    再度表示するには ((<rimo.tv.show>)) を実行してください。


=== FUNCTION

--- rimo.tv.move x y pos

    Rimo のウィンドウを移動します。
    x, y, pos の指定方法はそれぞれ
    ((<*rimo-width*>))、
    ((<*rimo-height*>))、
    ((<*rimo-position*>)) と同じです。

--- rimo.tv.window

    Rimo ウィンドウのウィンドウハンドルを取得します。
    ウィンドウが存在しない場合は 0 を返します。


== TODO

* Window の半透明化
* xyzzy のウィンドウの位置・サイズが変わったら追従する
* メニュー
* 閉じるボタン
* ウィンドウを移動できるように
* チャンネルやボリュームの変更を xyzzy からできるようにする
  (Flash Player の API が叩けるのなら。無理かな?)


== KNOWN BUGS

* もしかすると時々 xyzzy ごと落ちるかもしれません
  (auto-save-buffers のご利用を推奨しておきます)。


== AUTHOR

みやむこ かつゆき (((<URL:mailto:miyamuko (at) gmail.com>)))


== SEE ALSO

  : Rimo
        ((<URL:http://rimo.tv/>))

== COPYRIGHT

rimo.tv.xyzzy は MIT/X ライセンスに基づいて利用可能です。

See rimo.tv.xyzzy/docs/MIT-LICENSE for full license.

