# react_test

Electron+Reactなサンプルアプリ

詳細はQiitaにの記載したので参照下さい.

[Electron+ReactなサンプルアプリをES6で作成する](http://qiita.com/kegamin/items/e08923d46bb9df9cf283)

## アプリの機能

HelloWorld的な機能のみです.

* Reactを使った画面表示
* ボタンクリックを認識して、コンソールログにクリックカウントを出力
* デバッグ用にデベロッパーツールを表示
* babel-registerによるjsxの自動変換
* ESLintによる検証

何故か動かない！という時に便利なデベロッパーツールを表示させたり、クリック制御ってどうやるの？的な事を知りたかったので入れています

## やらなかった点

最終的には以下も試したいが、レポジトリが複雑になると参考にしずらいので今回はやっていません

* bowerを利用したフロントエンドの管理(bootstrapとか)
* reduxを使ったMV*的な作り込み
* react-route等のReactの色々な機能
* gulpを利用した自動化

## 使い方

```bash
cd repository_dir
clone https://github.com/kegamin/electron_test.git electron_test
cd electron_test
npm install
npm start
```

