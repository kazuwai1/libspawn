# libspawn
spawn*() for elf2x68k

Project libc group の libc 由来の spawn 関数群を elf2x68k で動くようにしてみました。  
オリジナル同様に HUPAIR 対応です。

## ビルド
ローカルで make すると、以下のファイルが生成されます
* libspawn.a
* test1.x : 実際にspawn*()を使用するテストプログラムです。呼び出しているプログラムはソースコードを確認してください。
* test2.x : argv[1]の内容を表示する HUPAIR 対応のテストプログラムで test1.x から呼び出します。

## 使い方
libspawn.a と spawn.h を使いたいプロジェクトのディレクトリにコピーして使用してください。  
elf2x68kの既存のヘッダファイルと衝突するシンボルがある場合は spawn.h の定義をコメントアウトするなどしてと調整してください。  
以下の関数を実装しています。使い方は libcの同名の関数と同じです。
 * spawnv()
 * spawnve()
 * spawnvp()
 * spawnl()
 * spawnle()
 * spawnlp()
 * ( spawnvpe() は spawnve() になるように#defineで定義されています )

## 注意点
libc の spawn() と比較して以下の点を変更しています。  
 * libc のシグナル機構には非対応です
 * libc の spawn は標準入出力のファイルハンドルを保存・復帰するコードが含まれていますが本ライブラリではコメントアウトしてビルドしています。
 * elf2x68k の gccで -O2/-Os/-O0 のいずれでライブラリを再構成しても動くことを確認済みです

## ライセンスについて
Project libc group のライセンスに従いますのでソース改編も含めて自由に利用できます。詳細は README_libc を参照してください。  
