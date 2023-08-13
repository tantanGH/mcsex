# MCSEX.X

Yet another high memory MACS loader for Human68k/X680x0

---

## About This

巷に星の数ほど存在するMACSローダ(プレーヤ)の末席に... 自分にとって必要なものだけを実装しました。

 - ハイメモリ専用
 - ディスクからハイメモリに高速ダイレクトロード (SCSIの場合は TS16FILE.X必須)
 - 最大32MBまでのチャンク読み込みなのでVDISKでもok
 - MercuryUnitが無くてもRaspberry Piを使った16bitPCM外部同期再生対応
 - ロードキャンセル可
 - ロードプログレス表示有り
 - ループ回数指定可
 - 終了時キーバッファをクリア

注意点：
PhantomX 1.03c を 68040/68060 モードで使用する場合、FLOAT4.X は組み込まないようにしてください。

* 68040 ... FLOAT2.X を使用する
* 68060 ... 060turbo.sys を組み込み、FLOATは組み込まない

そもそも整数演算性能の高いPhantomXではFPUエミュレーション機能を使用するFLOAT4.Xは非推奨の上、1.03c + 68040/68060ではFPU命令が呼び出されるとハングアップする場合があります。(ライトスルー・ライトバック共)

---

## How to Install

MCSEXxxx.ZIP をダウンロードして、MCSEX.X をパスの通ったディレクトリにコピーします。

---

## Usage

    usage: mcsex [options] <filename.mcs>
    options:
           -l<n> ... loop count (0=endless, default:1)
           -b<n> ... bulk load chunk size in MB (1-32, default:16)
           -h    ... show help message

           -r <remote-mcs-path> ... remote mcs path (for s44rasp)

単純に、

    mcsex hogehoge.mcs

で再生を開始します。

ループ再生の例：

    mcsex -l hogehoge.mcs
    mcsex -l0 hogehoge.mcs

で無限ループ、

    mcsex -l3 hogehoge.mcs

で3回ループです。


起動時に以下の常駐プログラムの存在をチェックします。
 - MACSDRV.X
 - PCM8A.X または PCM8PP.X のいずれか
 - ハイメモリドライバ (060turbo.sys または TS16DRVp.X)

---

## SCSIディスクから再生を行う場合

SCSIディスクからDMAの届かないハイメモリへダイレクトにデータ転送を行う必要があるため、TS16FILE.X が必須になります。
VDISK(PhantomX), HFS(XEiJ) の場合は必要ありません。Windrvは環境が無いので未確認です。

また、TS16FILE.Xの後にSCSI関連のデバイスドライバを組み込まないようにしてください(SUSIE.Xなど)。

---

## Raspberry Pi を使った16bitPCM外部同期再生

Mercury Unitを所有していないが、16bitPCM対応データを元の品質のまま再生したい場合は、`s44rasp` 及び `s44raspd` を使った外部同期再生に対応しています。

- [s44rasp](https://github.com/tantanGH/s44rasp/)
- [s44raspd](https://github.com/tantanGH/s44rasp-x68k/)

の最新版を導入し、あらかじめ再生するものを同じ .mcs ファイルをラズパイ側に転送しておく必要があります。

例えば `hogehoge.mcs` を data/mcs/ 以下にコピーしておき、

        nohup s44raspd -d /dev/ttyUSB0 -o -a hw:2,0 data/mcs &

としてバックグラウンドで s44raspd を起動した状態で、RS232C - USB クロス接続した PhantomX 実機上から

        mcsex -r hogehoge.mcs hogehoge.mcs

とすることで同期再生が可能です。ただし、一時停止など、再生開始とESCでのアボート以外の操作には対応していません。
また、タイミングには微妙にずれがありますので、あくまでMercuryが入手できない場合の回避策と考えてください。

---

## Special Thanks

以下のMACSプレーヤの仕様および実装を参考にさせて頂きました。この場を借りてお礼申し上げます。

- MACSplay.x (カタさん)
- MCSP.X (みゆ🌹ฅ^•ω•^ฅ さん)

---

## History

* 0.2.2 (2023/07/17) ... MACSデータバージョンを表示するようにした
* 0.2.1 (2023/07/17) ... 外部同期再生の際にMACSDRVのPCMをミュートするようにした
* 0.2.0 (2023/07/10) ... s44rasp/s44raspdと連携した16bitPCM外部同期再生をサポート
* 0.1.3 (2023/07/09) ... インフォメーション表示を少し変更
* 0.1.2 (2023/07/08) ... 終了時にグラフィックを初期化
* 0.1.1 (2023/07/05) ... CTRL+Cが押された時の挙動修正
* 0.1.0 (2023/07/04) ... 初版