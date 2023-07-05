# MCSEX.X

Yet another high memory MACS loader for Human68k/X680x0

---

## About This

巷に星の数ほど存在するMACSローダ(プレーヤ)の末席に... 自分にとって必要なものだけを実装しました。

 - ハイメモリ専用
 - ディスクからハイメモリに高速ダイレクトロード (SCSIの場合は TS16FILE.X必須)
 - 最大32MBまでのチャンク読み込みなのでVDISKでもok
 - ロードキャンセル可
 - ロードプログレス表示有り
 - ループ回数指定可
 - 終了時キーバッファをクリア

---

## How to Install

MCSEXxxx.ZIP をダウンロードして、MCSEX.X をパスの通ったディレクトリにコピーします。

---

# Usage

    usage: mcsex [options] <filename.mcs>
    options:
           -l<n> ... loop count (0=endless, default:1)
           -b<n> ... bulk load chunk size in MB (1-32, default:16)
           -h    ... show help message

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

SCSIディスクからデータを読み込む場合には TS16FILE.X が必須ですが、特にチェックしていませんのでくれぐれも注意してください。
VDISK(PhantomX), HFS(XEiJ) の場合は必要ありません。Windrvは環境が無いので未確認です。

---

## Special Thanks

以下のMACSプレーヤの仕様および実装を参考にさせて頂きました。この場を借りてお礼申し上げます。

- MACSplay.x (カタさん)
- MCSP.X (みゆ🌹ฅ^•ω•^ฅ さん)

---

## History

* 0.1.1 (2023/07/05) ... CTRL+Cが押された時の挙動修正
* 0.1.0 (2023/07/04) ... 初版